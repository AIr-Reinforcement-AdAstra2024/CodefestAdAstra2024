#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>

#include "GroundStation.hpp"
#include "cipher.h"


void GroundStation::decrypt(const std::string& inputPath, const std::string& outputPath){
    decryptImg(inputPath, outputPath, "hola");
}


std::string GroundStation::getPublicKey(){
    return this->publicKey ;
}


std::string GroundStation::getImgName(const std::string& outputPath){
    std::stringstream ss(outputPath);
    std::string item;
    std::vector<std::string> splitString;
    while (std::getline(ss, item, std::filesystem::path::preferred_separator)) {
        splitString.push_back(item);
    }
    return splitString.back();

}


void GroundStation::decryptImg(std::string inputPath, std::string outputPath, std::string aesKey){
    
    Cipher cipher("aes-256-cbc", "sha256");
    std::string text = cipher.file_read(inputPath);
    
    std::string decryptedImg = cipher.decrypt(text, aesKey, "restoooo");

    Cipher::kv1_t dataNew = cipher.decode_base64(decryptedImg);

    unsigned char* dataNewPointer = dataNew.first;

    unsigned int dataNewLength = dataNew.second; 

    std::ofstream outputFile(outputPath, std::ios::binary);

    outputFile.write(reinterpret_cast<const char*>(dataNewPointer), dataNewLength);

}