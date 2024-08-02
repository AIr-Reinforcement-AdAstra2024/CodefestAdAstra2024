#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>

#include "GroundStation.hpp"
#include "cipher.h"

GroundStation::GroundStation(){
    
}

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

void GroundStation::storeImgKeyPair(const std::string& imgName, const std::string& encryptedKey){
    this->imgKeys.insert({imgName, encryptedKey});


    std::ofstream outputFile("imgKeys.txt");
    outputFile << imgName +","+ encryptedKey+"\n";
    
    outputFile.close();

    
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


std::string GroundStation::pathJoin(const std::string& p1, const std::string& p2)
{
    char sep = '/';
    std::string tmp = p1;

#ifdef _WIN32
    sep = '\\';
#endif

    // Add separator if it is not included in the first path:
    if (p1[p1.length() - 1] != sep) {
        tmp += sep;
        return tmp + p2;
    } else {
        return p1 + p2;
    }
}