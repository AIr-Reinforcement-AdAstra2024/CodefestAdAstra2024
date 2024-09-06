#include <iostream>
#include <string>
#include <fstream>       
#include <openssl/bn.h> 
#include <openssl/evp.h> 
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <cstring>
#include <sstream>
#include <cstdlib>      

// Declaraciones de librerías de proyecto
#include "Satellite.hpp"
#include "GroundStation.hpp"

// Llaves públicas Diffie-Hellman
BN_CTX *ctx = BN_CTX_new();
BIGNUM *Ps = BN_new();
BIGNUM *Gs = BN_new(); 

// Llaves públicas RSA Satellite (e) y (n)

BIGNUM* public_satellite_exponent = BN_new();
BIGNUM* public_satellite_modulus = BN_new();

// Llaves públicas RSA Base (n)

BIGNUM* public_base_modulus = BN_new();

// Declaraciones de funciones
void loadKeysFromFile(const std::string& fileName, BIGNUM* &public_satellite_modulus, BIGNUM* &public_base_modulus, BIGNUM* &public_satellite_exponent, BIGNUM* &Ps);
void encrypt(const std::string& input_path, const std::string& output_path, Satellite& satellite);
void decrypt(const std::string& input_path, const std::string& output_path, GroundStation& groundStation);

int main(int argc, char* argv[]) {
    // Crear las clases a utilizar para cifrar o descifrar
    Satellite satellite;
    GroundStation groundStation; // 2 publicos


    // Definir aquí cosas compartidas

    satellite.setGroundStation(groundStation);
    std::string filename = "claves.txt";
    loadKeysFromFile(filename, public_satellite_modulus, public_base_modulus, public_satellite_exponent, Ps);
    BN_set_word(Gs, 2);
    
    // Funciones de modulos infos 
    BIGNUM* response_satellite = satellite.give_me_info(Ps, Gs, ctx);
    BIGNUM* response_groundstation = groundStation.give_me_info(Ps, Gs, ctx);
    
    // receive

    satellite.receive_info(response_groundstation, Ps, ctx);
    groundStation.receive_info(response_satellite, Ps, ctx);

    // image encryption

    
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <operation> <input_path> <output_path>" << std::endl;
        return 1;
    }

    std::string operation = argv[1];
    std::string input_path = argv[2];
    std::string output_path = argv[3];

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

void loadKeysFromFile(const std::string& fileName, BIGNUM* &public_satellite_modulus, BIGNUM* &public_base_modulus, BIGNUM* &public_satellite_exponent, BIGNUM* &Ps) {
    std::ifstream file(fileName);
    
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << fileName << std::endl;
        exit(EXIT_FAILURE); 
    }
    
    std::string line;
    while (getline(file, line)) {
        if (line.find("n_s = ") != std::string::npos) {
            std::string n_str = line.substr(line.find("n_s = ") + 6);
            BN_hex2bn(&public_satellite_modulus, n_str.c_str());
        } else if (line.find("n_b = ") != std::string::npos) {
            std::string nb_str = line.substr(line.find("n_b = ") + 6);
            BN_hex2bn(&public_base_modulus, nb_str.c_str());
        } else if (line.find("e_s -> ") != std::string::npos) {
            std::string e_str = line.substr(line.find("e_s -> ") + 7);
            BN_hex2bn(&public_satellite_exponent, e_str.c_str());
        }  else if (line.find("Ps = ") != std::string::npos) {
            std::string Ps_str = line.substr(line.find("Ps = ") + 5);
            BN_hex2bn(&Ps, Ps_str.c_str());
        }
    }
    
    file.close();  // Cerrar el archivo después de leerlo
}


void print_openssl_error() {
    ERR_print_errors_fp(stderr);
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
