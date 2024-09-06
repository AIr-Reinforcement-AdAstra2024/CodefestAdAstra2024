#include <iostream>
#include <string>
#include <fstream>       
#include <openssl/bn.h>  // Para trabajar con BIGNUM (números grandes)
#include <openssl/evp.h> // Para funciones criptográficas
#include <openssl/bio.h> // Para manejar BIOs (entrada/salida)
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <cstring>
#include <sstream>
#include <cstdlib>      

// Declaraciones de librerías de proyecto
#include "Satellite.hpp"
#include "GroundStation.hpp"

// Llaves públicas Diffie-Hellman
BN_CTX *ctx = BN_CTX_new(); // Contexto para las operaciones de BIGNUM
BIGNUM *Ps = BN_new();      // Ps es el valor primo grande
BIGNUM *Gs = BN_new();      // Gs es el generador

// Llaves públicas RSA del Satellite (e y n)
BIGNUM* public_satellite_exponent = BN_new();  // Exponente público (e) del Satellite
BIGNUM* public_satellite_modulus = BN_new();   // Módulo (n) público del Satellite

// Llave pública RSA Base (n)
BIGNUM* public_base_modulus = BN_new(); // Módulo (n) público de la Base

// Declaraciones de funciones
void loadKeysFromFile(const std::string& fileName, BIGNUM* &public_satellite_modulus, BIGNUM* &public_base_modulus, BIGNUM* &public_satellite_exponent, BIGNUM* &Ps);
void encrypt(const std::string& input_path, const std::string& output_path, Satellite& satellite);
void decrypt(const std::string& input_path, const std::string& output_path, GroundStation& groundStation);

int main(int argc, char* argv[]) {
    // Crear las clases Satellite y GroundStation para cifrar o descifrar
    Satellite satellite;
    GroundStation groundStation;

    // Definir aquí las variables compartidas para el cifrado/descifrado
    satellite.setGroundStation(groundStation);  // Asociar la estación base con el satellite
    std::string filename = "claves.txt";        // Archivo con las claves
    loadKeysFromFile(filename, public_satellite_modulus, public_base_modulus, public_satellite_exponent, Ps);  // Cargar las llaves desde el archivo

    // Configurar Gs (generador común) para el intercambio Diffie-Hellman
    BN_set_word(Gs, 2);
    
    // Obtener las respuestas Diffie-Hellman de ambos lados
    BIGNUM* response_satellite = satellite.give_me_info(Ps, Gs, ctx);  // Llave pública del Satellite
    BIGNUM* response_groundstation = groundStation.give_me_info(Ps, Gs, ctx);  // Llave pública de la estación base

    // Intercambiar información para obtener la clave compartida
    satellite.receive_info(response_groundstation, Ps, ctx);  // Satellite recibe la respuesta de la estación base
    groundStation.receive_info(response_satellite, Ps, ctx);  // La estación base recibe la respuesta del Satellite

    // Verificar la cantidad de argumentos para cifrado/descifrado de imágenes
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <operation> <input_path> <output_path>" << std::endl;
        return 1;
    }

    // Determinar la operación (cifrar o descifrar)
    std::string operation = argv[1];
    std::string input_path = argv[2];
    std::string output_path = argv[3];

    // Ejecutar la operación
    if (operation == "encrypt") {
        encrypt(input_path, output_path, satellite);  // Cifrar la imagen
    } else if (operation == "decrypt") {
        decrypt(input_path, output_path, groundStation);  // Descifrar la imagen
    } else {
        std::cerr << "Operación no válida: " << operation << std::endl;
        return 1;
    }

    return 0;
}

// Función para cargar las claves desde un archivo
void loadKeysFromFile(const std::string& fileName, BIGNUM* &public_satellite_modulus, BIGNUM* &public_base_modulus, BIGNUM* &public_satellite_exponent, BIGNUM* &Ps) {
    std::ifstream file(fileName);
    
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << fileName << std::endl;
        exit(EXIT_FAILURE);  // Detener el programa si no se puede abrir el archivo
    }
    
    std::string line;
    while (getline(file, line)) {
        // Cargar el módulo público del Satellite
        if (line.find("n_s = ") != std::string::npos) {
            std::string n_str = line.substr(line.find("n_s = ") + 6);
            BN_hex2bn(&public_satellite_modulus, n_str.c_str());
        } 
        // Cargar el módulo público de la estación base
        else if (line.find("n_b = ") != std::string::npos) {
            std::string nb_str = line.substr(line.find("n_b = ") + 6);
            BN_hex2bn(&public_base_modulus, nb_str.c_str());
        } 
        // Cargar el exponente público del Satellite
        else if (line.find("e_s -> ") != std::string::npos) {
            std::string e_str = line.substr(line.find("e_s -> ") + 7);
            BN_hex2bn(&public_satellite_exponent, e_str.c_str());
        }
        // Cargar Ps (primo grande) para Diffie-Hellman
        else if (line.find("Ps = ") != std::string::npos) {
            std::string Ps_str = line.substr(line.find("Ps = ") + 5);
            BN_hex2bn(&Ps, Ps_str.c_str());
        }
    }
    
    file.close();  // Cerrar el archivo después de leerlo
}

// Función para mostrar errores de OpenSSL
void print_openssl_error() {
    ERR_print_errors_fp(stderr);
}

// Función para cifrar una imagen usando la clase Satellite
void encrypt(const std::string& input_path, const std::string& output_path, Satellite& satellite) {
    satellite.encrypt(input_path, output_path);  // Cifrar la imagen
    std::cout << "input_path=" << input_path << std::endl;
    std::cout << "output_path=" << output_path << std::endl;
    std::cout << "Encrypted image" << std::endl;  // Mostrar confirmación de cifrado
}

// Función para descifrar una imagen usando la clase GroundStation
void decrypt(const std::string& input_path, const std::string& output_path, GroundStation& groundStation) {
    groundStation.decrypt(input_path, output_path);  // Descifrar la imagen
    std::cout << "input_path=" << input_path << std::endl;
    std::cout << "output_path=" << output_path << std::endl;
    std::cout << "Decrypted image" << std::endl;  // Mostrar confirmación de descifrado
}
