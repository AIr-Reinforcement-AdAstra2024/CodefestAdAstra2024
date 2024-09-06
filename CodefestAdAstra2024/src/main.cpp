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
BIGNUM *Ps = BN_new();
BIGNUM *Gs = BN_new();

// Generador común (Gs) para valores Ps de 2048
BN_set_word(Gs, 2);

// Llaves públicas RSA Satellite (e) y (n)
BIGNUM* public_satellite_exponent = BN_new();
BIGNUM* public_satellite_modulus = BN_new();

// Llaves públicas RSA Base (n)
BIGNUM* public_base_modulus = BN_new();

// Declaraciones de funciones
void loadKeysFromFile(const std::string& fileName, BIGNUM* &n, BIGNUM* &n2, BIGNUM* &e, BIGNUM* &d, BIGNUM* &d2, BIGNUM* &Ps);
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

    // Crear las clases a utilizar para cifrar o descifrar
    Satellite satellite;
    GroundStation groundStation;

    // Definir aquí cosas compartidas
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

void loadKeysFromFile(const std::string& fileName, BIGNUM* &n, BIGNUM* &n2, BIGNUM* &e, BIGNUM* &d, BIGNUM* &d2, BIGNUM* &Ps) {
    std::ifstream file(fileName);
    
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << fileName << std::endl;
        exit(EXIT_FAILURE);  // Detenemos el programa si no se puede abrir el archivo
    }
    
    std::string line;
    while (getline(file, line)) {
        if (line.find("n_s = ") != std::string::npos) {
            std::string n_str = line.substr(line.find("n_s = ") + 6);
            BN_hex2bn(&n, n_str.c_str());
        } else if (line.find("n_b = ") != std::string::npos) {
            std::string nb_str = line.substr(line.find("n_b = ") + 6);
            BN_hex2bn(&n2, nb_str.c_str());
        } else if (line.find("e_s -> ") != std::string::npos) {
            std::string e_str = line.substr(line.find("e_s -> ") + 7);
            BN_hex2bn(&e, e_str.c_str());
        } else if (line.find("d_s = ") != std::string::npos) {
            std::string d_str = line.substr(line.find("d_s = ") + 6);
            BN_hex2bn(&d, d_str.c_str());
        } else if (line.find("d_b = ") != std::string::npos) {
            std::string db_str = line.substr(line.find("d_b = ") + 6);
            BN_hex2bn(&d2, db_str.c_str());
        } else if (line.find("Ps = ") != std::string::npos) {
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
