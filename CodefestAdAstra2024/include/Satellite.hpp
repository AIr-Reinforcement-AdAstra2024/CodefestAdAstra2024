#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include "GroundStation.hpp"

class Satellite {
    public:
        Satellite();

        void encrypt(std::string inputPath, std::string outputPath);

        void setGroundStation(GroundStation groundStation);

    private:
        std::string generateKey();

        void encryptImg(std::string inputPath, std::string outputPath, std::string aesKey);

        std::string encryptKey(std::string publicKey, std::string aesKey);

        std::string getImgName(std::string outputPath);

};

#endif // SATELLITE_HPP
