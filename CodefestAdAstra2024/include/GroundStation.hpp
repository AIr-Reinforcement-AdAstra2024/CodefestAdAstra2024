#ifndef GROUNDSTATION_HPP
#define GROUNDSTATION_HPP

#include <string>
#include <map>
#include <openssl/bn.h>  // Biblioteca para trabajar con números grandes (BIGNUM)
#include <vector>

// Clase que representa la GroundStation en el sistema de comunicación
class GroundStation {
    public:
        GroundStation();    // Constructor: inicializa la estación
        ~GroundStation();   // Destructor: limpia los recursos asignados

        // Método para descifrar una imagen
        void decrypt(const std::string& inputPath, const std::string& outputPath);

        // Método para almacenar la pareja de nombre de imagen cifrada y clave
        void storeImgKeyPair(const std::string& imgName);

        // Obtener la clave pública de la estación base
        std::string getPublicKey();

        // Método para encriptar un mensaje usando RSA
        std::vector<BIGNUM*> encriptarRSA(std::string msg, BIGNUM* e, BIGNUM* n);

        // Generar información de intercambio Diffie-Hellman
        BIGNUM* give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx);

        // Recibir información de Diffie-Hellman y calcular la clave compartida
        void receive_info(BIGNUM* response_satellite, BIGNUM* Ps, BN_CTX* ctx);
        
    private:

        BIGNUM* dh_secret_key;   // Clave secreta privada para Diffie-Hellman
        BIGNUM* dh_shared_key;   // Clave compartida generada por Diffie-Hellman

        std::string privateKey;  // Clave privada (RSA)
        std::string publicKey;   // Clave pública (RSA)
        std::map<std::string, std::string> imgKeys;  // Mapa que almacena las claves para cada imagen

        // Método para descifrar una imagen usando la clave AES
        void decryptImg(std::string inputPath, std::string outputPath, std::string aesKey);

        // Obtener el nombre de un archivo a partir de su ruta
        std::string getImgName(const std::string& outputPath);

        // Insertar una nueva línea en un archivo (para almacenar claves de imágenes)
        void insertLine(const std::string& filename, const std::string& lineToInsert);

        // Cargar el mapa de claves de imágenes desde un archivo
        void loadImgKeys();

        // Generar una clave de cifrado AES
        std::string generateKey();

        // Método para desencriptar un mensaje usando RSA
        std::string desencriptarRSA(std::vector<BIGNUM*> encrypted_msg, BIGNUM* d, BIGNUM* n);

        // Generar la clave secreta privada para Diffie-Hellman
        void generateDHKey();

        // Realizar la operación modular g^h mod Ps
        BIGNUM* mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx); 
};

#endif 
