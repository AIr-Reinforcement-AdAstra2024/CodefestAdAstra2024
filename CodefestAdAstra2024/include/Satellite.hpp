#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include "GroundStation.hpp"

class GroudStation;

class Satellite {
    public:
        void encrypt(const std::string& inputPath, const std::string& outputPath);

        void setGroundStation(const GroundStation& groundStation);

    private:
        GroundStation groundStation;

        std::string generateKey();

        void encryptImg(std::string inputPath, std::string outputPath, std::string aesKey);

        std::string encryptKey(std::string publicKey, std::string aesKey);

        std::string getImgName(const std::string& outputPath);

};

#endif // SATELLITE_HPP
