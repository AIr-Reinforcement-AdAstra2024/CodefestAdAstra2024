#include "satellite.hpp"
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
#include <unistd.h>  // For sleep

Satellite::Satellite(const std::string& groundStationIp, int groundStationPort)
    : udpComm_(groundStationIp, groundStationPort) {
    // Constructor
}

Satellite::~Satellite() {
    // Destructor
}

std::string Satellite::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "[satellite] Error opening file for reading: " << filePath << std::endl;
        return "";
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

bool Satellite::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "[satellite] Error opening file for writing: " << filePath << std::endl;
        return false;
    }
    file.write(content.data(), content.size());
    return true;
}

std::string Satellite::generateSymmetricKey() {
    std::vector<unsigned char> key(32);  // 256-bit key
    if (!RAND_bytes(key.data(), key.size())) {
        std::cerr << "[satellite] Error generating symmetric key." << std::endl;
        return "";
    }
    return std::string(reinterpret_cast<char*>(key.data()), key.size());
}

bool Satellite::sendSymmetricKey(const std::string& key) {
    std::cout << "[satellite] Sending symmetric key to ground station..." << std::endl;
    return udpComm_.send(key);
}

bool Satellite::receiveConfirmation() {
    std::cout << "[satellite] Waiting for confirmation from ground station..." << std::endl;
    std::string confirmation = udpComm_.receive(timeoutSeconds_);
    if (confirmation.empty()) {
        return false;
    }
    return validateConfirmation(confirmation);
}

bool Satellite::validateConfirmation(const std::string& confirmation) {
    std::string publicKeyPem = readFile("keys/satellite-ground_station_public.key");
    if (publicKeyPem.empty()) {
        std::cerr << "[satellite] Error reading public key file." << std::endl;
        return false;
    }

    BIO* bio = BIO_new_mem_buf(publicKeyPem.data(), -1);
    if (!bio) {
        std::cerr << "[satellite] Error creating BIO." << std::endl;
        return false;
    }

    EVP_PKEY* pubKey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!pubKey) {
        std::cerr << "[satellite] Error reading public key." << std::endl;
        return false;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        std::cerr << "[satellite] Error creating EVP_MD_CTX." << std::endl;
        EVP_PKEY_free(pubKey);
        return false;
    }

    EVP_PKEY_CTX* pkey_ctx = EVP_PKEY_CTX_new(pubKey, nullptr);
    if (!pkey_ctx) {
        std::cerr << "[satellite] Error creating EVP_PKEY_CTX." << std::endl;
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pubKey);
        return false;
    }

    if (EVP_PKEY_verify_init(pkey_ctx) <= 0) {
        std::cerr << "[satellite] Error initializing PKEY verification." << std::endl;
        EVP_PKEY_CTX_free(pkey_ctx);
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pubKey);
        return false;
    }

    if (EVP_PKEY_verify(pkey_ctx, reinterpret_cast<const unsigned char*>(confirmation.data()), confirmation.size(), reinterpret_cast<const unsigned char*>(confirmation.data()), confirmation.size()) <= 0) {
        std::cerr << "[satellite] Confirmation verification failed." << std::endl;
        EVP_PKEY_CTX_free(pkey_ctx);
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pubKey);
        return false;
    }

    EVP_PKEY_CTX_free(pkey_ctx);
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pubKey);

    return true;
}

void Satellite::generateAndSendKey() {
    std::string newKey = generateSymmetricKey();
    if (newKey.empty()) {
        std::cerr << "[satellite] Error generating new symmetric key." << std::endl;
        return;
    }

    if (!sendSymmetricKey(newKey)) {
        std::cerr << "[satellite] Error sending symmetric key." << std::endl;
        return;
    }

    if (receiveConfirmation()) {
        std::cout << "[satellite] Confirmation received and validated." << std::endl;
        if (!writeFile("keys/satellite-symmetric.key", newKey)) {
            std::cerr << "[satellite] Error writing new symmetric key to file." << std::endl;
        }
    } else {
        std::cerr << "[satellite] No confirmation received within the timeout period." << std::endl;
    }
}

void Satellite::encryptImage(const std::string& inputPath, const std::string& outputPath) {
    // Read the symmetric key
    std::string key = readFile("keys/satellite-symmetric.key");
    if (key.empty()) {
        std::cerr << "[satellite] Error reading symmetric key file." << std::endl;
        return;
    }

    // Read the image file
    std::ifstream inputFile(inputPath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "[satellite] Error opening image file: " << inputPath << std::endl;
        return;
    }
    std::ostringstream imageStream;
    imageStream << inputFile.rdbuf();
    std::string imageData = imageStream.str();
    inputFile.close();

    std::vector<unsigned char> encryptedData(imageData.size() + AES_BLOCK_SIZE);
    std::vector<unsigned char> iv(AES_BLOCK_SIZE);
    if (!RAND_bytes(iv.data(), iv.size())) {
        std::cerr << "[satellite] Error generating IV." << std::endl;
        return;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "[satellite] Error creating EVP_CIPHER_CTX." << std::endl;
        return;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), iv.data()) != 1) {
        std::cerr << "[satellite] Error initializing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    int outlen1;
    if (EVP_EncryptUpdate(ctx, encryptedData.data(), &outlen1, reinterpret_cast<const unsigned char*>(imageData.data()), imageData.size()) != 1) {
        std::cerr << "[satellite] Error during encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    int outlen2;
    if (EVP_EncryptFinal_ex(ctx, encryptedData.data() + outlen1, &outlen2) != 1) {
        std::cerr << "[satellite] Error finalizing encryption." << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    EVP_CIPHER_CTX_free(ctx);

    // Write the IV and encrypted data to the output file
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "[satellite] Error opening output file: " << outputPath << std::endl;
        return;
    }
    outputFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
    outputFile.write(reinterpret_cast<const char*>(encryptedData.data()), outlen1 + outlen2);
    outputFile.close();

    std::cout << "[satellite] Image encrypted and written to: " << outputPath << std::endl;
}
