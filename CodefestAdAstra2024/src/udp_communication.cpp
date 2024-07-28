#include "udp_communication.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>  // For close()

UDPCommunication::UDPCommunication(const std::string& ip, int port)
    : ip_(ip), port_(port) {
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0) {
        std::cerr << "Error creating socket." << std::endl;
        exit(EXIT_FAILURE);
    }
}

UDPCommunication::~UDPCommunication() {
    close(sockfd_);
}

bool UDPCommunication::send(const std::string& message) {
    struct sockaddr_in servaddr;
    std::memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port_);
    if (inet_pton(AF_INET, ip_.c_str(), &servaddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported." << std::endl;
        return false;
    }

    if (sendto(sockfd_, message.c_str(), message.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Send failed." << std::endl;
        return false;
    }
    return true;
}

std::string UDPCommunication::receive(int timeoutSeconds) {
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);
    char buffer[2048];
    std::memset(buffer, 0, sizeof(buffer));

    fd_set readfds;
    struct timeval timeout;
    timeout.tv_sec = timeoutSeconds;
    timeout.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(sockfd_, &readfds);

    int ret = select(sockfd_ + 1, &readfds, nullptr, nullptr, &timeout);
    if (ret == -1) {
        std::cerr << "Select error." << std::endl;
        return "";
    } else if (ret == 0) {
        std::cerr << "Timeout occurred." << std::endl;
        return "";
    } else {
        ssize_t recvlen = recvfrom(sockfd_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&servaddr, &len);
        if (recvlen < 0) {
            std::cerr << "Receive failed." << std::endl;
            return "";
        }
        buffer[recvlen] = '\0';
        return std::string(buffer);
    }
}
