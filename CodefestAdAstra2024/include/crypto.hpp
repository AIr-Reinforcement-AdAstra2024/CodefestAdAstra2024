#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>

// Función para cifrar un archivo
void encrypt(const std::string& input_path, const std::string& output_path);

// Función para descifrar un archivo
void decrypt(const std::string& input_path, const std::string& output_path);

// Función para generar una llave simétrica aleatoria
std::string generateSymmetricKey();

#endif // CRYPTO_HPP
