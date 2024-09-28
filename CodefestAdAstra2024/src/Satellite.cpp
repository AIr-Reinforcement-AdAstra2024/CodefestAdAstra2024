#include <iostream>
#include <sstream>
#include <string>
#include <vector> 
#include <filesystem>
#include <fstream>
#include <random>
#include <openssl/bn.h>  // Para BIGNUM
#include <openssl/evp.h> // Para funciones criptográficas
#include <openssl/bio.h> // Para el manejo de BIOs
#include <openssl/buffer.h>
#include <openssl/err.h>

#include "Satellite.hpp"
#include "GroundStation.hpp"
#include "cipher.h"

#define MAX_NUMBER 100000000

std::random_device rd;
static long int fixedSeed = 128123123114124;  // Definir la semilla fija
static std::mt19937 generator(rd()); 
static std::uniform_int_distribution<int> distribution(0, MAX_NUMBER);  // [0 - MAX_NUMBER]

// Constructor de la clase Satellite
Satellite::Satellite() {
    // Inicializa las variables BIGNUM para Diffie-Hellman
    dh_secret_key = BN_new();  // Asignar memoria para dh_secret_key
    dh_shared_key = BN_new();  // Asignar memoria para dh_shared_key
}

// Destructor de la clase Satellite
Satellite::~Satellite() {
    // Liberar la memoria asignada para dh_secret_key y dh_shared_key
    if (dh_secret_key != nullptr) {
        BN_free(dh_secret_key);  // Libera la memoria asignada a dh_secret_key
    }
    if (dh_shared_key != nullptr) {
        BN_free(dh_shared_key);  // Libera la memoria asignada a dh_shared_key
    }
}   

// Generar la clave Diffie-Hellman para el Satellite
void Satellite::generateDHKey() {
    int satellite_personalkey = distribution(generator);  // Generar clave personal del Satellite
    this->dh_secret_key = BN_new();  // Crear un nuevo BIGNUM para la clave secreta
    BN_set_word(this->dh_secret_key, satellite_personalkey);  // Asignar la clave generada al BIGNUM
}

// Función de exponenciación modular: (g^h) % Ps
BIGNUM* Satellite::mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx) {
    BIGNUM* result = BN_new();  // Crear un nuevo BIGNUM para almacenar el resultado
    BN_mod_exp(result, g, h, Ps, ctx); // Realiza la exponenciación modular
    return result;
}

// Generar la información para Diffie-Hellman y retornar g^dh_secret_key % Ps
BIGNUM* Satellite::give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx) {
    generateDHKey();  // Generar la clave Diffie-Hellman
    return mod_exp(Gs, this->dh_secret_key, Ps, ctx);  // Calcular y devolver g^dh_secret_key mod Ps
}

// Recibir información de GroundStation y calcular la clave compartida
void Satellite::receive_info(BIGNUM* response_groundstation, BIGNUM* Ps, BN_CTX* ctx) {
    // Guardar la clave compartida calculada usando la respuesta de GroundStation
    this->dh_shared_key = mod_exp(response_groundstation, this->dh_secret_key, Ps, ctx);
}

// Establecer la referencia a GroundStation
void Satellite::setGroundStation(GroundStation &groundStation) {
    this->groundStation = &groundStation;
}

// Función para cifrar y almacenar una imagen
void Satellite::encrypt(const std::string& inputPath, const std::string& outputPath) {
    std::string aesKey = generateKey();  // Generar una clave AES aleatoria
    encryptImg(inputPath, outputPath, aesKey);  // Cifrar la imagen usando la clave AES
    std::string imgName = getImgName(outputPath);  // Obtener el nombre del archivo cifrado
    this->groundStation->storeImgKeyPair(imgName);  // Almacenar la relación entre imagen y clave en GroundStation
}

// Función para cifrar una imagen usando AES
void Satellite::encryptImg(const std::string& inputPath, const std::string& outputPath, const std::string& aesKey) {
    // Leer la imagen en binario
    std::ifstream inputFile(inputPath, std::ios::binary);
    inputFile.seekg(0, std::ios::end);  // Mover el puntero del archivo al final para obtener el tamaño
    Cipher::uint fileSize = inputFile.tellg(); // Obtener el tamaño del archivo
    inputFile.seekg(0, std::ios::beg);  // Devolver el puntero al inicio

    // Crear un arreglo de bytes para almacenar la imagen
    Cipher::uint bufferSize = 1024;
    Cipher::uchar* buffer = new unsigned char[bufferSize];
    
    // Crear un cifrador AES con CTR y SHA-256
    Cipher cipher("aes-256-ctr", "sha256");

    for (unsigned int i = 0; i < fileSize; i+= bufferSize){
        size_t bytesToRead = bufferSize;
        if (i + bufferSize > fileSize) {
            bytesToRead = fileSize - i;
        }

        inputFile.seekg(i, std::ios::beg);
        inputFile.read((char*) buffer, bytesToRead);

        // Convertir la imagen a base64
        std::string imgInBase64 = cipher.encode_base64(buffer, bytesToRead); 

        // Cifrar la imagen usando la clave AES
        std::string encryptedImg = cipher.encrypt(imgInBase64, aesKey);
        encryptedImg = encryptedImg.substr(0, encryptedImg.length() - 2);
        // Guardar la imagen cifrada en el archivo de salida
        insertLine(outputPath, encryptedImg);
    }
    
    // Leer el archivo de imagen en el arreglo
    inputFile.close();  // Cerrar el archivo
}

    

// Obtener el nombre del archivo a partir del path
std::string Satellite::getImgName(const std::string& outputPath) {
    std::stringstream ss(outputPath);
    std::string item;
    std::vector<std::string> splitString;
    while (std::getline(ss, item, std::filesystem::path::preferred_separator)) {
        splitString.push_back(item);  // Dividir el path por el separador de directorios
    }
    return splitString.back();  // Devolver el último componente del path (nombre del archivo)
}

// Generar una clave aleatoria usando la clave Diffie-Hellman compartida
std::string Satellite::generateKey() {
    // Convertir dh_shared_key a una cadena decimal
    char* dh_shared_key_str = BN_bn2dec(dh_shared_key);
    std::string dh_shared_key_string(dh_shared_key_str);

    // Tomar los primeros 19 dígitos de la clave compartida
    std::string f_19_digits = dh_shared_key_string.substr(0, 19);

    // Convertir los primeros 19 dígitos a un entero de tipo unsigned long long
    unsigned long long val = std::stoull(f_19_digits);

    // Liberar la memoria del string creado por OpenSSL
    OPENSSL_free(dh_shared_key_str);

    // Definir el conjunto de caracteres que pueden componer una llave 
    std::string CHARACTERS = "zABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    // Generar un número aleatorio con la clave Diffie-Hellman
    std::mt19937 generator(val);

    // Seleccionar caracteres aleatorios del conjunto de caracteres
    std::uniform_int_distribution<> distribution(1, CHARACTERS.size() - 1);

    // Determinar aleatoriamente el tamaño de la llave
    std::uniform_int_distribution<> keyLengthGenerator(1, CHARACTERS.size() * 4 - 1);

    std::string random_string;
    int length = keyLengthGenerator(generator);

    // Construir la clave aleatoria
    for (int i = 0; i < length; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;  // Devolver la clave generada
}


void Satellite::insertLine(const std::string& filename, const std::string& lineToInsert) {
    std::ofstream outputFile(filename, std::ios::app);  // Abrir archivo en modo append
    outputFile << lineToInsert;  // Escribir la línea en el archivo
}

// Código comentado para desencriptar con RSA (no utilizado actualmente)

// std::string Satellite::desencriptarRSA(std::vector<BIGNUM*> encrypted_msg, BIGNUM* d, BIGNUM* n) {
//     BN_CTX* ctx = BN_CTX_new();
//     std::string resultado;

//     for (auto& enc : encrypted_msg) {
//         BIGNUM* decrypted = BN_new();
//         BN_mod_exp(decrypted, enc, d, n, ctx); 
//         resultado += static_cast<char>(BN_get_word(decrypted));
//         BN_free(decrypted);
//     }

//     BN_CTX_free(ctx);
//     return resultado;
// }

// Código comentado para encriptar con RSA (no utilizado actualmente)

// std::vector<BIGNUM*> Satellite::encriptarRSA(std::string msg, BIGNUM* e, BIGNUM* n){
//     BN_CTX* ctx = BN_CTX_new();
//     std::vector<BIGNUM*> resultado;

//     for (char c : msg) {
//         BIGNUM* m = BN_new();
//         BIGNUM* encrypted = BN_new();

//         BN_set_word(m, c); // Convertir el carácter a BIGNUM
//         BN_mod_exp(encrypted, m, e, n, ctx); // Realizar la encriptación: c^e mod n

//         resultado.push_back(encrypted);
//         BN_free(m);
//     }

//     BN_CTX_free(ctx);
//     return resultado;
// }
