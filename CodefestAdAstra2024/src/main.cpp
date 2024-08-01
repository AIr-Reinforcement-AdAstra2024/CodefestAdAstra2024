#include <iostream>
#include <string>
#include <cstdlib>
// Declaraciones de librerías usadas

#include "Satellite.hpp"
#include "GroundStation.hpp"

// Declaraciones de funciones
void encrypt(const std::string& input_path, const std::string& output_path, Satellite& satellite);
void decrypt(const std::string& input_path, const std::string& output_path, GroundStation& groundStation);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <operation> <input_path> <output_path>" << std::endl;
        return 1;
    }

    std::string operation = argv[1];
    std::string input_path = argv[2];
    std::string output_path = argv[3];


    Satellite satellite;
    GroundStation groundStation;

    satellite.setGroundStation(groundStation);


    if (operation == "encrypt") {
        encrypt(input_path, output_path, satellite);
    } else if (operation == "decrypt") {
        decrypt(input_path, output_path, groundStation);
    } else {
        std::cerr << "Operación no válida: " << operation << std::endl;
        return 1;
    }

    return 0;
}

void encrypt(const std::string& input_path, const std::string& output_path, Satellite& satellite) {
    
    satellite.encrypt(input_path, output_path);

    std::cout << "input_path=" << input_path << std::endl;
    std::cout << "output_path=" << output_path << std::endl;
    std::cout << "Encrypted image" << std::endl;
}

void decrypt(const std::string& input_path, const std::string& output_path, GroundStation& groundStation) {
    
    groundStation.decrypt(input_path, output_path);
    
    std::cout << "input_path=" << input_path << std::endl;
    std::cout << "output_path=" << output_path << std::endl;
    std::cout << "Decrypted image" << std::endl;
}