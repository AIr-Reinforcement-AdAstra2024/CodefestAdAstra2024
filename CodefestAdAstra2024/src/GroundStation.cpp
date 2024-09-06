#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>
#include <random>

#include "GroundStation.hpp"
#include "cipher.h"

#define MAX_NUMBER 100000000

static long int fixedSeed = 128127523114124;  // Definir la semilla fija
static std::mt19937 generator(fixedSeed); 
static std::uniform_int_distribution<int> distribution(0, MAX_NUMBER);  // [0 - MAX_NUMBER]


GroundStation::GroundStation(){
    //Cada vez que se crea esta clase guarda en un mapa los nombres de los archivos cifrados y las llaves para descifrar cada uno.
    loadImgKeys();

    // Inicializa las variables BIGNUM para Diffie-Hellman
    dh_secret_key = BN_new();  // Asignar memoria para dh_secret_key
    dh_shared_key = BN_new();  // Asignar memoria para dh_shared_key

}

GroundStation::~GroundStation() {
    // Liberar la memoria asignada para dh_secret_key y dh_shared_key
    if (dh_secret_key != nullptr) {
        BN_free(dh_secret_key);  // Libera la memoria asignada a dh_secret_key
    }
    if (dh_shared_key != nullptr) {
        BN_free(dh_shared_key);  // Libera la memoria asignada a dh_shared_key
    }
}

void GroundStation::generateDHKey()
{
    int ground_personalkey = distribution(generator);
    this->dh_secret_key = BN_new();
    BN_set_word(this->dh_secret_key, ground_personalkey);
}

BIGNUM* GroundStation::mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx) {
    // return g^h mod Ps
    // mi llave privada es h
    BIGNUM* result = BN_new();
    BN_mod_exp(result, g, h, Ps, ctx); // Realiza la exponenciación modular
    return result;
}

BIGNUM* GroundStation::give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx) {
    generateDHKey();
    return mod_exp(Gs, this->dh_secret_key, Ps, ctx);
}

void GroundStation::receive_info(BIGNUM* response_satellite, BIGNUM* Ps, BN_CTX* ctx){
    // Guardar la llave pública del Satellite
    this->dh_shared_key = mod_exp(response_satellite, this->dh_secret_key, Ps, ctx);
    std::cout << "GroundStation: Llave compartida: " << BN_bn2dec(this->dh_shared_key) << std::endl;
}

void GroundStation::decrypt(const std::string& inputPath, const std::string& outputPath){
    //Se obtiene el nombre del archivo cifrado
    std::string fileName = getImgName(inputPath);
    //Se obtiene la llave del archivo cifrado (Se asume que dicha llave existe)
    std::map<std::string, std::string>::iterator iterator = this->imgKeys.find(fileName);

    if (iterator != this->imgKeys.end()) {
        std::string aesKey = this->imgKeys[fileName];
        if (aesKey.empty()){
            throw std::runtime_error("Key for file \""+ fileName + "\" not found !!!");
        }
        //En caso de tener la llave de la imagen, se descifra la imagen
        decryptImg(inputPath, outputPath,aesKey);
    }
    else{
        throw std::runtime_error("File \""+ fileName + "\" not found !!!");
    }

    
}


std::string GroundStation::getPublicKey(){
    //Se obtiene la llave pública del GroundStation
    return this->publicKey ;
}

std::string GroundStation::getImgName(const std::string& outputPath){
    //Dado un path se otbiene el nombre del archivo
    // Ejemplo: "C:/Users/Usuario/Documents/imagen.jpg" -> "imagen.jpg"
    std::stringstream ss(outputPath);
    std::string item;
    std::vector<std::string> splitString;
    while (std::getline(ss, item, std::filesystem::path::preferred_separator)) {
        splitString.push_back(item);
    }
    return splitString.back();
}

void GroundStation::storeImgKeyPair(const std::string& imgName, const std::string& encryptedKey){
    //Guarda la pareja de Nombre de imagen cifrada y llave, dentro de un mapa
    this->imgKeys.insert({imgName, encryptedKey});

    //En caso de ser necesario crea una carpeta para persistir el mapa de imagenes y llaves 
    std::string directoryPath = "groundStationSecrets";
    if (!std::filesystem::exists(directoryPath)){
        std::filesystem::create_directory(directoryPath);
    }

    //Guarda en el archvio imgKeys.txt las parejas Nombre imagen y llave 
    char path[] = "groundStationSecrets imgKeys.txt";
    path[20] = std::filesystem::path::preferred_separator;
    insertLine(path, imgName +","+ encryptedKey);
}

void GroundStation::loadImgKeys()
{   
    //Abre el archivo de mapas
    char path[] = "groundStationSecrets imgKeys.txt";
    path[20] = std::filesystem::path::preferred_separator;

    //Lee las líneas del archivo y las almacena dentro del mapa de nombres de imagenes y llaves
    std::ifstream inputFile(path);
    std::string linea; 
    while (std::getline(inputFile, linea)) {
        std::string item;
        std::vector<std::string> splitString;
        std::stringstream ss2(linea); 
        while (std::getline(ss2, item, ',')) 
        {
            splitString.push_back(item);    
        }
        this->imgKeys.insert({splitString.front(), splitString.back()});
    }   
}


void GroundStation::insertLine(const std::string& filename, const std::string& lineToInsert) {
    //abre un archivo y añande una string específico al final
    std::ofstream outputFile(filename, std::ios::app);
    outputFile << lineToInsert << std::endl;
}


void GroundStation::decryptImg(std::string inputPath, std::string outputPath, std::string aesKey){
    //Crea una clase para poder descifrar la imagen 
    Cipher cipher("aes-256-ctr", "sha256");
    //Lee la imagen cifrada y la almacena en un string
    std::string text = cipher.file_read(inputPath);
    //Decifra la imagen
    std::string decryptedImg = cipher.decrypt(text, aesKey);
    //Se convierte la imagen a bytes
    Cipher::kv1_t dataNew = cipher.decode_base64(decryptedImg);
    //Se extraen los bytes y el tamaño de ellos
    unsigned char* dataNewPointer = dataNew.first;
    unsigned int dataNewLength = dataNew.second; 
    //Se guardan los bytes en el directorio especificado
    std::ofstream outputFile(outputPath, std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(dataNewPointer), dataNewLength);
}

// std::string GroundStation::desencriptarRSA(std::vector<BIGNUM*> encrypted_msg, BIGNUM* d, BIGNUM* n) {
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

// std::vector<BIGNUM*> GroundStation::encriptarRSA(std::string msg, BIGNUM* e, BIGNUM* n){
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
