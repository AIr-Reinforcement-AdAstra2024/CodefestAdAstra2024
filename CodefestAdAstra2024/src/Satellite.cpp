#include <iostream>
#include <sstream>
#include <string>
#include <vector> 
#include <filesystem>
#include <fstream>
#include <random>

#include "Satellite.hpp"
#include "GroundStation.hpp"
#include "cipher.h"


void Satellite::encrypt(const std::string& inputPath, const std::string& outputPath){
    
    std::string aesKey = generateKey();

    encryptImg(inputPath, outputPath, aesKey);

    std::string imgName = getImgName(outputPath);

    this->groundStation.storeImgKeyPair(imgName, aesKey);
}

void Satellite::encryptImg(const std::string& inputPath, const std::string& outputPath,const std::string& aesKey){
    
    std::ifstream inputFile(inputPath, std::ios::binary);

    inputFile.seekg(0, std::ios::end);
    Cipher::uint fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    Cipher::uchar* data = new unsigned char[fileSize];
    inputFile.read((char*) data, fileSize);

    inputFile.close();

    Cipher cipher("aes-256-ctr", "sha256");
    std::string imgInBase64 = cipher.encode_base64(data, fileSize); 

    std::string encryptedImg = cipher.encrypt(imgInBase64, aesKey);

    cipher.file_write(outputPath, encryptedImg, true);


}

std::string Satellite::getImgName(const std::string& outputPath){
    std::stringstream ss(outputPath);
    std::string item;
    std::vector<std::string> splitString;
    while (std::getline(ss, item, std::filesystem::path::preferred_separator)) {
        splitString.push_back(item);
    }
    return splitString.back();

}

std::string Satellite::generateKey()
{
    
    std::string CHARACTERS= "zABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution <> distribution(1, CHARACTERS.size() - 1);
    std::uniform_int_distribution <> keyLengthGenerator(1, CHARACTERS.size()*4 - 1);
    std::string random_string;
    int length = keyLengthGenerator(generator);

    for (int i = 0; i < length; ++i) {
        random_string
            += CHARACTERS[distribution(generator)];
    }

    return random_string;
}