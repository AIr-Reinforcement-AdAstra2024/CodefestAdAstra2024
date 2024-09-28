#ifndef SATELLITE_HPP
#define SATELLITE_HPP

#include <string>
#include "GroundStation.hpp"

class GroundStation;  // Forward declaration de GroundStation

// Clase que representa un Satellite en el sistema de comunicación
class Satellite {
    public:

        Satellite();   // Constructor: inicializa el satellite
        ~Satellite();  // Destructor: limpia los recursos utilizados por el satellite

        // Método para cifrar una imagen
        void encrypt(const std::string& inputPath, const std::string& outputPath);

        // Establecer la GroundStation asociada al Satellite
        void setGroundStation(GroundStation & groundStation);

        // Generar información para el intercambio Diffie-Hellman
        BIGNUM* give_me_info(BIGNUM* Ps, BIGNUM* Gs, BN_CTX* ctx);

        // Recibir información de Diffie-Hellman desde la GroundStation
        void receive_info(BIGNUM* response_groundstation, BIGNUM* Ps, BN_CTX* ctx);

    private:
        GroundStation* groundStation;  // Puntero a la estación base asociada

        BIGNUM* dh_secret_key;   // Clave secreta privada para Diffie-Hellman
        BIGNUM* dh_shared_key;   // Clave compartida generada por Diffie-Hellman

        // Genera una clave aleatoria basada en la clave compartida Diffie-Hellman
        std::string generateKey();

        // Método para cifrar la imagen usando AES
        void encryptImg(const std::string& inputPath, const std::string& outputPath, const std::string& aesKey);

        // Obtener el nombre del archivo a partir de su ruta
        std::string getImgName(const std::string& outputPath);
        
        // Generar la clave privada Diffie-Hellman
        void generateDHKey();

        // Realizar la operación modular g^h mod Ps
        BIGNUM* mod_exp(BIGNUM* g, BIGNUM* h, BIGNUM* Ps, BN_CTX* ctx); 

        void insertLine(const std::string& filename, const std::string& lineToInsert);
};

#endif // SATELLITE_HPP
