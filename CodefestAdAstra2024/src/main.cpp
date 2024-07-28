#include "satellite.hpp"
#include "ground_station.hpp"
#include <chrono>
#include <thread>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <ctime>

namespace fs = std::__fs::filesystem;

void generateRSAKeys(const std::string& privateKeyPath, const std::string& publicKeyPath, const std::string& satellitePublicKeyPath) {
    int keyLength = 2048;
    unsigned long exponent = RSA_F4;
    RSA* rsa = RSA_generate_key(keyLength, exponent, nullptr, nullptr);
    if (!rsa) {
        std::cerr << "Error generating RSA key." << std::endl;
        return;
    }

    BIO* privateKeyBio = BIO_new_file(privateKeyPath.c_str(), "w+");
    BIO* publicKeyBio = BIO_new_file(publicKeyPath.c_str(), "w+");
    BIO* satellitePublicKeyBio = BIO_new_file(satellitePublicKeyPath.c_str(), "w+");

    if (!PEM_write_bio_RSAPrivateKey(privateKeyBio, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
        std::cerr << "Error writing private key." << std::endl;
    }
    if (!PEM_write_bio_RSAPublicKey(publicKeyBio, rsa)) {
        std::cerr << "Error writing public key." << std::endl;
    }
    if (!PEM_write_bio_RSAPublicKey(satellitePublicKeyBio, rsa)) {
        std::cerr << "Error writing satellite public key." << std::endl;
    }

    BIO_free_all(privateKeyBio);
    BIO_free_all(publicKeyBio);
    BIO_free_all(satellitePublicKeyBio);
    RSA_free(rsa);
}

int main() {
    const int waitBetweenCycles = 30;
    const int imagesPerCycle = 3;
    const int cyclesPerKey = 5;
    const int totalCycles = 20;
    
    

    // Initialize Satellite and GroundStation
    Satellite satellite("127.0.0.1", 8080);
    GroundStation groundStation("127.0.0.1", 8080);

    // Generate RSA keys
    generateRSAKeys("keys/ground_station-rsa_private.key", "keys/ground_station-rsa_public.key", "keys/satellite-ground_station_public.key");

    // Initial key exchange
    groundStation.listenForKey();
    satellite.generateAndSendKey();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    for (int cycle = 0; cycle < totalCycles; ++cycle) {
        if (cycle % cyclesPerKey == 0) {
            groundStation.listenForKey();
            satellite.generateAndSendKey();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        } else {
            // Encrypt images
            int imageCount = 0;
            for (const auto& entry : fs::directory_iterator("images")) {
                if (imageCount >= imagesPerCycle) break;

                std::string inputPath = entry.path().string();
                std::time_t now = std::time(nullptr);
                std::string outputPath = "outputs/encrypted/" + entry.path().stem().string() + "_" + std::to_string(now) + ".bin";

                fs::create_directories("outputs/encrypted");
                satellite.encryptImage(inputPath, outputPath);
                ++imageCount;
            }
        }

        std::cout << "Cycle " << cycle + 1 << " completed. Waiting for " << waitBetweenCycles << " seconds." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(waitBetweenCycles));
    }

    std::cout << "Simulation completed." << std::endl;
    return 0;
}
