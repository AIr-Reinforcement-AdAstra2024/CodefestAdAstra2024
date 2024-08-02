#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>

#include "GroundStation.hpp"
#include "cipher.h"

GroundStation::GroundStation(){
    loadImgKeys();

}

void GroundStation::decrypt(const std::string& inputPath, const std::string& outputPath){
    std::string fileName = getImgName(inputPath);

    std::map<std::string, std::string>::iterator iterator = this->imgKeys.find(fileName);

    if (iterator != this->imgKeys.end()) {
        std::string aesKey = this->imgKeys[fileName];
        decryptImg(inputPath, outputPath,aesKey);
    }
    else{
        throw std::runtime_error("File \""+ fileName + "\" not found !!!");
    }

    
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

void GroundStation::storeImgKeyPair(const std::string& imgName, const std::string& encryptedKey){
    this->imgKeys.insert({imgName, encryptedKey});

    std::string directoryPath = "groundStationSecrets";
    if (!std::filesystem::exists(directoryPath)){
        std::filesystem::create_directory(directoryPath);
    }


    char path[] = "groundStationSecrets imgKeys.txt";
    path[20] = std::filesystem::path::preferred_separator;
    insertLine(path, imgName +","+ encryptedKey);
}

void GroundStation::loadImgKeys()
{
    char path[] = "groundStationSecrets imgKeys.txt";
    path[20] = std::filesystem::path::preferred_separator;

    std::ifstream inputFile(path);
    std::string linea; 
    while (std::getline(inputFile, linea)) {
        std::string item;
        std::vector<std::string> splitString;
        std::stringstream ss2(linea); 
        while (std::getline(ss2, item, ',')) 
        {
            splitString.push_back(item);    
        }
        this->imgKeys.insert({splitString.front(), splitString.back()});
    }   
}


void GroundStation::insertLine(const std::string& filename, const std::string& lineToInsert) {
    std::ofstream outputFile(filename, std::ios::app);
    outputFile << lineToInsert << std::endl;
}


void GroundStation::decryptImg(std::string inputPath, std::string outputPath, std::string aesKey){
    
    Cipher cipher("aes-256-cbc", "sha256");
    std::string text = cipher.file_read(inputPath);
    
    std::string decryptedImg = cipher.decrypt(text, aesKey);

    Cipher::kv1_t dataNew = cipher.decode_base64(decryptedImg);

    unsigned char* dataNewPointer = dataNew.first;

    unsigned int dataNewLength = dataNew.second; 

    std::ofstream outputFile(outputPath, std::ios::binary);

    outputFile.write(reinterpret_cast<const char*>(dataNewPointer), dataNewLength);

}
