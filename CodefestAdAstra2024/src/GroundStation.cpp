#include "ground_station.hpp"
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

GroundStation::GroundStation(const std::string& satelliteIp, int satellitePort)
    : udpComm_(satelliteIp, satellitePort) {
    // Constructor
}

GroundStation::~GroundStation() {
    // Destructor
}

std::string GroundStation::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "[ground_station] Error opening file for reading: " << filePath << std::endl;
        return "";
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

bool GroundStation::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "[ground_station] Error opening file for writing: " << filePath << std::endl;
        return false;
    }
    file.write(content.data(), content.size());
    return true;
}

std::string GroundStation::signMessage(const std::string& message, const std::string& privateKeyPath) {
    std::string privateKeyPem = readFile(privateKeyPath);
    if (privateKeyPem.empty()) {
        std::cerr << "[ground_station] Error reading private key file." << std::endl;
        return "";
    }

    BIO* bio = BIO_new_mem_buf(privateKeyPem.data(), -1);
    if (!bio) {
        std::cerr << "[ground_station] Error creating BIO." << std::endl;
        return "";
    }

    EVP_PKEY* privKey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!privKey) {
        std::cerr << "[ground_station] Error reading private key." << std::endl;
        return "";
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        std::cerr << "[ground_station] Error creating EVP_MD_CTX." << std::endl;
        EVP_PKEY_free(privKey);
        return "";
    }

    if (EVP_DigestSignInit(mdctx, nullptr, EVP_sha256(), nullptr, privKey) <= 0) {
        std::cerr << "[ground_station] Error initializing DigestSign." << std::endl;
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(privKey);
        return "";
    }

    if (EVP_DigestSignUpdate(mdctx, message.data(), message.size()) <= 0) {
        std::cerr << "[ground_station] Error during DigestSignUpdate." << std::endl;
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(privKey);
        return "";
    }

    size_t siglen;
    if (EVP_DigestSignFinal(mdctx, nullptr, &siglen) <= 0) {
        std::cerr << "[ground_station] Error getting signature length." << std::endl;
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(privKey);
        return "";
    }

    std::vector<unsigned char> sig(siglen);
    if (EVP_DigestSignFinal(mdctx, sig.data(), &siglen) <= 0) {
        std::cerr << "[ground_station] Error during DigestSignFinal." << std::endl;
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(privKey);
        return "";
    }

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(privKey);

    return std::string(reinterpret_cast<char*>(sig.data()), siglen);
}

void GroundStation::listenForKey() {
    std::cout << "[ground_station] Listening for new symmetric key from satellite..." << std::endl;
    std::string newKey = udpComm_.receive(60);

    if (!newKey.empty()) {
        std::cout << "[ground_station] New symmetric key received." << std::endl;
        if (writeFile("keys/ground_station-candidate_symmetric.key", newKey)) {
            std::cout << "[ground_station] Candidate symmetric key updated." << std::endl;
            std::string confirmation = signMessage("OK", "keys/ground_station-rsa_private.key");
            if (!confirmation.empty()) {
                udpComm_.send(confirmation);
            } else {
                std::cerr << "[ground_station] Error signing confirmation message." << std::endl;
            }
        } else {
            std::cerr << "[ground_station] Error writing candidate symmetric key to file." << std::endl;
        }
    } else {
        std::cerr << "[ground_station] No new key received." << std::endl;
    }
}

bool GroundStation::decryptWithKey(const std::string& key, const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "[ground_station] Error opening image file: " << inputPath << std::endl;
        return false;
    }
    std::ostringstream imageStream;
    imageStream << inputFile.rdbuf();
    std::string encryptedData = imageStream.str();
    inputFile.close();

    std::vector<unsigned char> iv(AES_BLOCK_SIZE);
    std::memcpy(iv.data(), encryptedData.data(), AES_BLOCK_SIZE);

    std::vector<unsigned char> decryptedData(encryptedData.size() - AES_BLOCK_SIZE);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "[ground_station] Error creating EVP_CIPHER_CTX." << std::endl;
        return false;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), iv.data()) != 1) {
        std::cerr << "[ground_station] Error initializing decryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int outlen1;
    if (EVP_DecryptUpdate(ctx, decryptedData.data(), &outlen1, reinterpret_cast<const unsigned char*>(encryptedData.data() + AES_BLOCK_SIZE), encryptedData.size() - AES_BLOCK_SIZE) != 1) {
        std::cerr << "[ground_station] Error during decryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int outlen2;
    if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + outlen1, &outlen2) != 1) {
        std::cerr << "[ground_station] Error finalizing decryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    EVP_CIPHER_CTX_free(ctx);

    decryptedData.resize(outlen1 + outlen2);
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "[ground_station] Error opening output file: " << outputPath << std::endl;
        return false;
    }
    outputFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
    outputFile.close();

    std::cout << "[ground_station] Image decrypted and written to: " << outputPath << std::endl;
    return true;
}

void GroundStation::decryptImage(const std::string& inputPath, const std::string& outputPath) {
    std::string currentKey = readFile("keys/ground_station-current_symmetric.key");
    std::string candidateKey = readFile("keys/ground_station-candidate_symmetric.key");

    if (!currentKey.empty() && decryptWithKey(currentKey, inputPath, outputPath)) {
        std::cout << "[ground_station] Decryption with current key succeeded." << std::endl;
    } else if (!candidateKey.empty() && decryptWithKey(candidateKey, inputPath, outputPath)) {
        std::cout << "[ground_station] Decryption with candidate key succeeded. Updating current key." << std::endl;
        writeFile("keys/ground_station-current_symmetric.key", candidateKey);
    } else {
        std::cerr << "[ground_station] Decryption failed with both current and candidate keys." << std::endl;
    }
}
