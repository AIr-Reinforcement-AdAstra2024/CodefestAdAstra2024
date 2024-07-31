#ifndef GROUNDSTATION_HPP
#define GROUNDSTATION_HPP

#include <string>
#include <map>
#include "Satellite.hpp"

class GroundStation {
    public:
        GroundStation();

        void decrypt(std::string inputPath, std::string outputPath);

        void storeImgKeyPair(std::string imgName, std::string encryptedKey);

        std::string getPublicKey();

        void setSatellite(Satellite satellite);




    private:
        std::string privateKey;
        std::string publicKey;
        std::map<std::string, std::string> imgKeys;


        void decryptImg(std::string inputPath, std::string outputPath, std::string aesKey);
    
        std::string decryptAESKey(std::string encryptedAESKey);
        
        bool keyRenewalCriterion();

        void generateRSAKeys();
};

#endif 
