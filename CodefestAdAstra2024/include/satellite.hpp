#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include "udp_communication.hpp"

class Satellite {
public:
    Satellite(const std::string& groundStationIp, int groundStationPort);
    ~Satellite();

    void generateAndSendKey();
    void encryptImage(const std::string& inputPath, const std::string& outputPath);

private:
    std::string readFile(const std::string& filePath);
    bool writeFile(const std::string& filePath, const std::string& content);
    bool sendSymmetricKey(const std::string& key);
    bool receiveConfirmation();
    bool validateConfirmation(const std::string& confirmation);
    std::string generateSymmetricKey();

    UDPCommunication udpComm_;
    int timeoutSeconds_ = 60;  // Timeout in seconds
};

#endif // SATELLITE_HPP
