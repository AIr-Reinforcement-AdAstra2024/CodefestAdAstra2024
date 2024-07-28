#ifndef GROUNDSTATION_HPP
#define GROUNDSTATION_HPP

#include "udp_communication.hpp"
#include <string>

class GroundStation {
public:
    GroundStation(const std::string& satelliteIp, int satellitePort);
    ~GroundStation();

    void listenForKey();
    void decryptImage(const std::string& inputPath, const std::string& outputPath);

private:
    UDPCommunication udpComm_;

    std::string readFile(const std::string& filePath);
    bool writeFile(const std::string& filePath, const std::string& content);
    std::string signMessage(const std::string& message, const std::string& privateKeyPath);
    bool decryptWithKey(const std::string& key, const std::string& inputPath, const std::string& outputPath);
};

#endif // GROUNDSTATION_HPP
