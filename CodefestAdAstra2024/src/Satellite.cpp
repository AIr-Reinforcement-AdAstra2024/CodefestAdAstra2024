#include <iostream>
#include <sstream>
#include <string>
#include <vector> 
#include <filesystem>
#include <fstream>

#include "Satellite.hpp"
#include "GroundStation.hpp"
#include "cipher.h"


void Satellite::setGroundStation(const GroundStation& groundStation){
    this->groundStation = groundStation;
}

void Satellite::encrypt(const std::string& inputPath, const std::string& outputPath){
    
    std::string aesKey = "hola";

    encryptImg(inputPath, outputPath, aesKey);

    std::string imgName = getImgName(inputPath);

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

    Cipher cipher("aes-256-cbc", "sha256");
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