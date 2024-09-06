#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include "GroundStation.hpp"

class GroundStation;

class Satellite {
    public:
        void encrypt(const std::string& inputPath, const std::string& outputPath);

        void setGroundStation(GroundStation & groundStation);

    private:
        GroundStation groundStation;

        std::string generateKey();

        void encryptImg(const std::string& inputPath, const std::string& outputPath,const std::string& aesKey);

        std::string getImgName(const std::string& outputPath);
        
        void generateDHKey();

        BIGNUM* mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx); 
};

#endif // SATELLITE_HPP
