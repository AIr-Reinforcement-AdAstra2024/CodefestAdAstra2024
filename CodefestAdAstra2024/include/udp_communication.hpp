#ifndef UDP_COMMUNICATION_HPP
#define UDP_COMMUNICATION_HPP

#include <string>

class UDPCommunication {
public:
    UDPCommunication(const std::string& ip, int port);
    ~UDPCommunication();

    bool send(const std::string& message);
    std::string receive(int timeoutSeconds);

private:
    std::string ip_;
    int port_;
    int sockfd_;
};

#endif // UDP_COMMUNICATION_HPP
