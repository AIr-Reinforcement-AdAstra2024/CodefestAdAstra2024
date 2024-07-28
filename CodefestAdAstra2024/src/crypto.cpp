#include "crypto.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

void encrypt(const std::string& input_path, const std::string& output_path, const std::string& key) {
    // Leer el contenido del archivo de entrada
    std::ifstream input_file(input_path, std::ios::binary);
    std::vector<unsigned char> input_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();

    // Leer el IV desde el archivo de llave
    unsigned char iv[AES_BLOCK_SIZE];
    std::ifstream key_file("keys/satellite-symmetric.key", std::ios::binary);
    key_file.read(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE);
    key_file.close();

    // Configurar el contexto de cifrado
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv);

    std::vector<unsigned char> encrypted_data(input_data.size() + AES_BLOCK_SIZE);
    int len;
    EVP_EncryptUpdate(ctx, encrypted_data.data(), &len, input_data.data(), input_data.size());
    int ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, encrypted_data.data() + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    // Escribir el contenido cifrado en el archivo de salida
    std::ofstream output_file(output_path, std::ios::binary);
    output_file.write(reinterpret_cast<const char*>(encrypted_data.data()), ciphertext_len);
    output_file.close();
}

void decrypt(const std::string& input_path, const std::string& output_path, const std::string& key) {
    // Leer el contenido cifrado del archivo de entrada
    std::ifstream input_file(input_path, std::ios::binary);
    std::vector<unsigned char> input_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();

    // Leer el IV desde el archivo de llave
    unsigned char iv[AES_BLOCK_SIZE];
    std::ifstream key_file("keys/ground_station-symmetric.key", std::ios::binary);
    key_file.read(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE);
    key_file.close();

    // Configurar el contexto de descifrado
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), iv);

    std::vector<unsigned char> decrypted_data(input_data.size());
    int len;
    EVP_DecryptUpdate(ctx, decrypted_data.data(), &len, input_data.data(), input_data.size());
    int plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, decrypted_data.data() + len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    // Escribir el contenido descifrado en el archivo de salida
    std::ofstream output_file(output_path, std::ios::binary);
    output_file.write(reinterpret_cast<const char*>(decrypted_data.data()), plaintext_len);
    output_file.close();
}
