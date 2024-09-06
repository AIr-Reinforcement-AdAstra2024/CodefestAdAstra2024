#ifndef GROUNDSTATION_HPP
#define GROUNDSTATION_HPP

#include <string>
#include <map>
#include <openssl/bn.h> 
#include <vector>

class GroundStation {
    public:
        GroundStation();

        void decrypt(const std::string& inputPath, const std::string& outputPath);

        void storeImgKeyPair(const std::string& imgName, const std::string& encryptedKey);

        std::string getPublicKey();

        std::vector<BIGNUM*> encriptarRSA(std::string msg, BIGNUM* e, BIGNUM* n);

        BIGNUM* give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx);

        void receive_info(BIGNUM* response_satellite);
        
    private:

        BIGNUM* dh_secret_key;
        BIGNUM* dh_shared_key;

        std::string privateKey;
        std::string publicKey;
        std::map<std::string, std::string> imgKeys;

        void decryptImg(std::string inputPath, std::string outputPath, std::string aesKey);

        std::string getImgName(const std::string& outputPath);

        void insertLine(const std::string& filename, const std::string& lineToInsert);

        void loadImgKeys();

        std::string desencriptarRSA(std::vector<BIGNUM*> encrypted_msg, BIGNUM* d, BIGNUM* n);
        
        void generateDHKey();

        BIGNUM* mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx); 
};

#endif 
