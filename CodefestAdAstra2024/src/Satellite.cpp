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

long int fixedSeed = 128123123114124;  // Definir la semilla fija
std::mt19937 generator(fixedSeed); 
std::uniform_int_distribution<int> distribution(0, MAX_NUMBER);  // [0 - MAX_NUMBER]

void Satellite::generateDHKey()
{
    int satellite_personalkey = distribution(generator);
    this->dh_secret_key = BN_new();
    BN_set_word(this->dh_secret_key, satellite_personalkey);
}

BIGNUM* Satellite::mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx) {
    // return g^h mod Ps
    // mi llave privada es h
    BIGNUM* result = BN_new();
    BN_mod_exp(result, g, h, Ps, ctx); // Realiza la exponenciación modular
    return result;
}

BIGNUM* Satellite::DH_public_key(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx) {
    BIGNUM* satellite_public_key = BN_new();
    satellite_public_key = mod_exp(Gs, this->dh_secret_key, Ps, ctx);
    return satellite_public_key;
}

void print_openssl_error() {
    ERR_print_errors_fp(stderr);
}

void Satellite::setGroundStation(GroundStation & groundStation){
    this->groundStation = groundStation;
}

void Satellite::encrypt(const std::string& inputPath, const std::string& outputPath){
    //Genera una llave aleatoria como un string
    std::string aesKey = generateKey();
    //Con la llave aleatoria se cifra la imagen y se guarda en un archivo
    encryptImg(inputPath, outputPath, aesKey);
    //Se extrae el nombre del archivo cifrado 
    std::string imgName = getImgName(outputPath);
    //Se comunica la llave y el nombre del archivo cifrado para que posteriormente se pueda decifrar
    this->groundStation.storeImgKeyPair(imgName, aesKey);
}

void Satellite::encryptImg(const std::string& inputPath, const std::string& outputPath,const std::string& aesKey){
    //Se lee la imagen en binario
    std::ifstream inputFile(inputPath, std::ios::binary);
    //Se mueve el apuntador del archivo al final para extraer el tamaño del archivo, luego se devuelve al inico
    inputFile.seekg(0, std::ios::end);
    Cipher::uint fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    //Se genera un arreglo de bytes para almacenar la imagen y se guarda allí
    Cipher::uchar* data = new unsigned char[fileSize];
    inputFile.read((char*) data, fileSize);

    inputFile.close();
    //Se crea un clase para cifrar el arreglo de caracteres 
    Cipher cipher("aes-256-ctr", "sha256");
    //Se convierten los bytes a base 64
    std::string imgInBase64 = cipher.encode_base64(data, fileSize); 
    //Se cifra la imagen con la llave en base64
    std::string encryptedImg = cipher.encrypt(imgInBase64, aesKey);
    //Se guarda la imagen cifrada en el directorio especificado
    cipher.file_write(outputPath, encryptedImg, true);


}

std::string Satellite::getImgName(const std::string& outputPath){
    //Dado un path se otbiene el nombre del archivo
    std::stringstream ss(outputPath);
    std::string item;
    std::vector<std::string> splitString;
    while (std::getline(ss, item, std::filesystem::path::preferred_separator)) {
        splitString.push_back(item);
    }
    return splitString.back();

}

std::string Satellite::generateKey()
{
    //Se define el conjunto de caracterers que pueden componer una llave 
    std::string CHARACTERS= "zABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    //Se generan 2 generadores de números aleatorios. 
    std::random_device rd;
    std::mt19937 generator(rd());
    //Selecciona caracteres aleatorios del conjunto de caracteres
    std::uniform_int_distribution <> distribution(1, CHARACTERS.size() - 1);
    //Determina aleatoriamente el tamaño de la llave
    std::uniform_int_distribution <> keyLengthGenerator(1, CHARACTERS.size()*4 - 1);
    
    std::string random_string;
    int length = keyLengthGenerator(generator);
    //Construye la clave
    for (int i = 0; i < length; ++i) {
        random_string
            += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

std::string GroundStation::desencriptarRSA(std::vector<BIGNUM*> encrypted_msg, BIGNUM* d, BIGNUM* n) {
    BN_CTX* ctx = BN_CTX_new();
    std::string resultado;

    for (auto& enc : encrypted_msg) {
        BIGNUM* decrypted = BN_new();
        BN_mod_exp(decrypted, enc, d, n, ctx); 
        resultado += static_cast<char>(BN_get_word(decrypted));
        BN_free(decrypted);
    }

    BN_CTX_free(ctx);
    return resultado;
}

std::vector<BIGNUM*> encriptarRSA(std::string msg, BIGNUM* e, BIGNUM* n){
    BN_CTX* ctx = BN_CTX_new();
    std::vector<BIGNUM*> resultado;

    for (char c : msg) {
        BIGNUM* m = BN_new();
        BIGNUM* encrypted = BN_new();

        BN_set_word(m, c); // Convertir el carácter a BIGNUM
        BN_mod_exp(encrypted, m, e, n, ctx); // Realizar la encriptación: c^e mod n

        resultado.push_back(encrypted);
        BN_free(m);
    }

    BN_CTX_free(ctx);
    return resultado;
}