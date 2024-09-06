#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include "GroundStation.hpp"

class GroundStation;

class Satellite {
    public:
        void encrypt(const std::string& inputPath, const std::string& outputPath);

        void setGroundStation(GroundStation & groundStation);

        BIGNUM* give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx);

        void receive_info(BIGNUM* response_groundstation, BIGNUM* Ps, BN_CTX* ctx);

    private:
        GroundStation groundStation;

        BIGNUM* dh_secret_key;
        BIGNUM* dh_shared_key;

        std::string generateKey();

        void encryptImg(const std::string& inputPath, const std::string& outputPath,const std::string& aesKey);

        std::string getImgName(const std::string& outputPath);
        
        void generateDHKey();

        BIGNUM* mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx); 
};

#endif // SATELLITE_HPP
