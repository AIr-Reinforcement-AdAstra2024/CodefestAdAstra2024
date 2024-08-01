#include <iostream>
#include <sstream>
#include <string>
#include <vector> 
#include <filesystem>

#include "Satellite.hpp"
#include "GroundStation.hpp"
#include "cipher.h"


void Satellite::setGroundStation(const GroundStation& groundStation){
    this->groundStation = groundStation;
}

void Satellite::encrypt(const std::string& inputPath, const std::string& outputPath){
    
    Cipher cipher("aes-256-cbc", "sha256");
   
    cipher.encrypt_file(inputPath, outputPath, "password", "saltsalt");

    cipher.decrypt_file(outputPath, "newImg.p", "password", "saltsalt");
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