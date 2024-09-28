#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm> 
#include "GroundStation.hpp"
#include "cipher.h"

#define MAX_NUMBER 100000000

// Se crea un generador de números aleatorios
std::random_device rd2;
static long int fixedSeed = 128127523114124;  // Definir la semilla fija
static std::mt19937 generator(rd2());  // Inicializar generador con dispositivo de aleatoriedad
static std::uniform_int_distribution<int> distribution(0, MAX_NUMBER);  // Rango [0 - MAX_NUMBER]

// Constructor de la clase GroundStation
GroundStation::GroundStation() {
    // Carga en un mapa los nombres de archivos cifrados y sus respectivas llaves
    loadImgKeys();

    // Inicializa las variables BIGNUM para Diffie-Hellman
    dh_secret_key = BN_new();  // Asignar memoria para dh_secret_key
    dh_shared_key = BN_new();  // Asignar memoria para dh_shared_key
}

// Destructor de la clase GroundStation
GroundStation::~GroundStation() {
    // Liberar la memoria asignada para dh_secret_key y dh_shared_key
    if (dh_secret_key != nullptr) {
        BN_free(dh_secret_key);  // Libera la memoria asignada a dh_secret_key
    }
    if (dh_shared_key != nullptr) {
        BN_free(dh_shared_key);  // Libera la memoria asignada a dh_shared_key
    }
}

// Generar la clave Diffie-Hellman para GroundStation
void GroundStation::generateDHKey() {
    int ground_personalkey = distribution(generator);  // Generar clave personal del GroundStation
    this->dh_secret_key = BN_new();  // Crear un nuevo BIGNUM para la clave secreta
    BN_set_word(this->dh_secret_key, ground_personalkey);  // Asignar la clave generada al BIGNUM
}

// Función de exponenciación modular: (g^h) % Ps
BIGNUM* GroundStation::mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx) {
    BIGNUM* result = BN_new();  // Crear un nuevo BIGNUM para almacenar el resultado
    BN_mod_exp(result, g, h, Ps, ctx);  // Realiza la exponenciación modular
    return result;
}

// Generar la información para Diffie-Hellman y retornar g^dh_secret_key % Ps
BIGNUM* GroundStation::give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx) {
    generateDHKey();  // Generar la clave Diffie-Hellman
    return mod_exp(Gs, this->dh_secret_key, Ps, ctx);  // Calcular y devolver g^dh_secret_key mod Ps
}

// Recibir información del Satellite y calcular la clave compartida
void GroundStation::receive_info(BIGNUM* response_satellite, BIGNUM* Ps, BN_CTX* ctx) {
    // Guardar la clave compartida calculada usando la respuesta del Satellite
    this->dh_shared_key = mod_exp(response_satellite, this->dh_secret_key, Ps, ctx);
}

// Función para descifrar una imagen
void GroundStation::decrypt(const std::string& inputPath, const std::string& outputPath) {
    // Se obtiene el nombre del archivo cifrado
    std::string fileName = getImgName(inputPath);
    // Se busca la llave asociada al archivo cifrado
    std::map<std::string, std::string>::iterator iterator = this->imgKeys.find(fileName);

    if (iterator != this->imgKeys.end()) {
        // Si la llave es encontrada, se obtiene la llave AES
        std::string aesKey = this->imgKeys[fileName];
        if (aesKey.empty()) {
            throw std::runtime_error("Key for file \"" + fileName + "\" not found !!!");
        }
        // Descifrar la imagen usando la llave
        decryptImg(inputPath, outputPath, aesKey);
    } else {
        throw std::runtime_error("File \"" + fileName + "\" not found !!!");
    }
}

// Obtener la llave pública del GroundStation
std::string GroundStation::getPublicKey() {
    return this->publicKey;
}

// Obtener el nombre del archivo a partir del path
std::string GroundStation::getImgName(const std::string& outputPath) {
    std::stringstream ss(outputPath);
    std::string item;
    std::vector<std::string> splitString;
    // Dividir el path por el separador de directorios
    while (std::getline(ss, item, std::filesystem::path::preferred_separator)) {
        splitString.push_back(item);
    }
    return splitString.back();  // Retornar el último componente del path (nombre del archivo)
}

// Guardar la pareja de Nombre de imagen cifrada y llave en el mapa
void GroundStation::storeImgKeyPair(const std::string& imgName) {
    std::string aesKey = generateKey();  // Generar una llave AES
    this->imgKeys.insert({imgName, aesKey});  // Insertar la pareja en el mapa

    // Si es necesario, crear una carpeta para almacenar las llaves
    std::string directoryPath = "groundStationSecrets";
    if (!std::filesystem::exists(directoryPath)) {
        std::filesystem::create_directory(directoryPath);
    }

    // Guardar en el archivo imgKeys.txt las parejas de Nombre de imagen y llave
    char path[] = "groundStationSecrets imgKeys.txt";
    path[20] = std::filesystem::path::preferred_separator;
    insertLine(path, imgName + "," + aesKey);  // Insertar línea en el archivo
}

// Cargar las llaves desde el archivo imgKeys.txt
void GroundStation::loadImgKeys() {   
    char path[] = "groundStationSecrets imgKeys.txt";
    path[20] = std::filesystem::path::preferred_separator;

    // Leer las líneas del archivo y almacenarlas en el mapa
    std::ifstream inputFile(path);
    std::string linea;
    while (std::getline(inputFile, linea)) {
        std::string item;
        std::vector<std::string> splitString;
        std::stringstream ss2(linea); 
        while (std::getline(ss2, item, ',')) {
            splitString.push_back(item);
        }
        this->imgKeys.insert({splitString.front(), splitString.back()});
    }
}

// Generar una clave aleatoria usando la clave Diffie-Hellman compartida
std::string GroundStation::generateKey() {
    char* dh_shared_key_str = BN_bn2dec(dh_shared_key);  // Convertir dh_shared_key a cadena decimal
    std::string dh_shared_key_string(dh_shared_key_str);

    // Tomar los primeros 19 dígitos de la clave compartida
    std::string f_19_digits = dh_shared_key_string.substr(0, 19);

    // Convertir los primeros 19 dígitos a un entero de tipo unsigned long long
    unsigned long long val = std::stoull(f_19_digits);

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

// Insertar una línea en un archivo de texto
void GroundStation::insertLine(const std::string& filename, const std::string& lineToInsert) {
    std::ofstream outputFile(filename, std::ios::app);  // Abrir archivo en modo append
    outputFile << lineToInsert << std::endl;  // Escribir la línea en el archivo
}

// Función para descifrar una imagen
void GroundStation::decryptImg(std::string inputPath, std::string outputPath, std::string aesKey) {
    // Crear un objeto Cipher para descifrar la imagen
    Cipher cipher("aes-256-ctr", "sha256");
    
    // Leer la imagen cifrada del archivo y almacenarla en un string
    

    std::vector<std::string> filePaths;
    for (const auto& entry : std::filesystem::directory_iterator(inputPath)) {
            if (entry.is_regular_file()) {  // Check if it's a regular file
                filePaths.push_back(entry.path().string());  // Store the path as a string
            }
        }
    std::ofstream outputFile(outputPath, std::ios::binary);
    std::sort(filePaths.begin(), filePaths.end());  // Sort filePaths alphabetically
    for (const auto& filePath : filePaths) {
        std::string text = cipher.file_read(filePath);
        std::string decryptedImg = cipher.decrypt(text, aesKey);
        Cipher::kv1_t dataNew = cipher.decode_base64(decryptedImg);

        // Extraer los bytes y su tamaño
        unsigned char* dataNewPointer = dataNew.first;
        unsigned int dataNewLength = dataNew.second; 

        // Guardar los bytes en el archivo de salida
        
        outputFile.write(reinterpret_cast<const char*>(dataNewPointer), dataNewLength);
    }

    outputFile.close();
    
}

// Código comentado para desencriptar con RSA (no utilizado actualmente)

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

// Código comentado para encriptar con RSA (no utilizado actualmente)

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
