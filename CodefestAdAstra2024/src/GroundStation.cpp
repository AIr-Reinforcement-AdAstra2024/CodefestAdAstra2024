#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>

#include "GroundStation.hpp"


void GroundStation::decrypt(const std::string& inputPath, const std::string& outputPath){
    std::cout << "Recieved " << inputPath[0] << std::endl;
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