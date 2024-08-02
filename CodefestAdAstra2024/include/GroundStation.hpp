#ifndef GROUNDSTATION_HPP
#define GROUNDSTATION_HPP

#include <string>
#include <map>

class GroundStation {
    public:
        GroundStation();

        void decrypt(const std::string& inputPath, const std::string& outputPath);

        void storeImgKeyPair(const std::string& imgName, const std::string& encryptedKey);

        std::string getPublicKey();
        
        
     

    private:
        std::string privateKey;
        std::string publicKey;
        std::map<std::string, std::string> imgKeys;

        void decryptImg(std::string inputPath, std::string outputPath, std::string aesKey);
    
        std::string decryptAESKey(std::string encryptedAESKey);
        
        bool keyRenewalCriterion();

        void generateRSAKeys();

        std::string getImgName(const std::string& outputPath);

        void insertLine(const std::string& filename, const std::string& lineToInsert);

        void loadImgKeys();
};

#endif 