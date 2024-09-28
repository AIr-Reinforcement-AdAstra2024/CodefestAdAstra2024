# Codefest Ad Astra 2024 - Cifrado y Descifrado Satelital

Este proyecto implementa una solución criptográfica para el cifrado y descifrado de imágenes satelitales, cumpliendo con los requisitos del **Codefest Ad Astra 2024**. La solución está diseñada para sistemas embebidos y utiliza llaves dinámicas para garantizar la seguridad de las comunicaciones entre satélites y estaciones terrenas.

## **Contenido**

- [Descripción del Proyecto](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Requisitos](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Instalación](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Compilación](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Ejecución](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Estructura del Proyecto](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Detalles Técnicos](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
    - [Cifrado](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
    - [Descifrado](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
    - [Llaves Dinámicas](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Verificación y Validación](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)
- [Autores](https://www.notion.so/Codefest-Ad-Astra-2024-Cifrado-y-Descifrado-Satelital-10fbb7d45b8f80959d85db995b6706b9?pvs=21)

## **Descripción del Proyecto**

Este proyecto aborda el problema de **ciberseguridad en misiones satelitales**. El objetivo es diseñar e implementar algoritmos de cifrado y descifrado de imágenes satelitales usando llaves dinámicas y cumpliendo con los requerimientos de sistemas embebidos de baja capacidad.

La solución está diseñada para ejecutarse en un **Jetson Nano** y hace uso de técnicas avanzadas de criptografía.

## **Requisitos**

- **Sistemas embebidos**: El código está optimizado para ejecutarse en una Jetson Nano con un límite de **4 GB de RAM**.
- **Librerías**:
    - `libcrypto` para el cifrado/descifrado.
    - `OpenCV` para el manejo de imágenes.
- **Lenguaje**: C/C++
- **Herramientas**:
    - CMake para la compilación.
    - Jetson Nano con Ubuntu (ya configurado en la competencia).

## **Instalación**

1. Clonar este repositorio:
    
    ```bash
    git clone <https://github.com/tuusuario/CodefestAdAstra2024.git>
    cd CodefestAdAstra2024
    
    ```
    
2. Instalar las dependencias necesarias (en caso de que no estén instaladas en el entorno de la Jetson Nano):
    
    ```bash
    sudo apt-get install libcrypto++-dev libopencv-dev
    
    ```
    

## **Compilación**

### **En Linux/Unix**

Para compilar el código, ejecutar el siguiente comando:

```bash
./build.sh

```

Este comando generará el ejecutable `CodefestAdAstra2024.sh` dentro del directorio. Para ejecutar el programa:

```bash
./CodefestAdAstra2024.sh

```

### **En Windows**

Existen varias opciones para compilar el código en Windows. A continuación, se detallan algunos métodos:

- **MinGW**:
    1. Instalar `mingw32-make`.
    2. Navegar a la carpeta del proyecto: `CodefestAdAstra2024/build`.
    3. Ejecutar:
        
        ```bash
        mingw32-make
        
        ```
        
- **Visual Studio**:
    1. Abrir la carpeta del proyecto en Visual Studio.
    2. Ejecutar `nmake` desde la terminal.
- **CMake** (experimental):
Ejecutar:
    
    ```bash
    ./build.bat
    
    ```
    

## **Ejecución**

Para ejecutar el programa, después de la compilación, usar el siguiente comando:

```bash
./CodefestAdAstra2024.sh <ruta_imagen_entrada> <ruta_imagen_salida>

```

## **Estructura del Proyecto**

```
├── include/                 # Archivos de cabecera
│   ├── cipher.h             # Definiciones para cifrado y descifrado
│   ├── GroundStation.hpp    # Lógica de la estación terrena
│   └── Satellite.hpp        # Lógica del satélite
├── src/                     # Código fuente
│   ├── cipher.cc            # Implementación de las funciones de cifrado/descifrado
│   ├── GroundStation.cpp    # Implementación de la estación terrena
│   ├── Satellite.cpp        # Implementación del satélite
│   └── main.cpp             # Programa principal
├── images/                  # Imágenes de prueba
├── Diagramas/               # Diagramas UML del proyecto
├── README.md                # Archivo README
└── Documento_de_diseño___CodefestAdAstra2024.pdf  # Documento detallado del diseño del proyecto

```

## **Detalles Técnicos**

### **Cifrado**

El algoritmo de cifrado se encuentra en `cipher.cc` y utiliza un esquema de cifrado simétrico basado en **AES-256** para garantizar la integridad y seguridad de las imágenes satelitales. El algoritmo toma como entrada la imagen en formato binario y la cifra para que no sea legible ni manipulable.

```cpp
void encryptImage(const std::string &inputPath, const std::string &outputPath);

```

### **Descifrado**

El descifrado sigue un esquema simétrico, utilizando la misma llave que el cifrado para restaurar la imagen original.

```cpp
void decryptImage(const std::string &inputPath, const std::string &outputPath);

```

### **Llaves Dinámicas**

El manejo de llaves dinámicas es crucial en este proyecto. Las llaves no se almacenan localmente, sino que se generan dinámicamente de manera independiente en el satélite y en la estación terrena utilizando **OpenSSL** para asegurar que las llaves sean únicas y seguras. Esto garantiza que no se pueda interceptar ni manipular la llave durante la comunicación.

## **Verificación y Validación**

Para garantizar la calidad del código, se recomienda el uso de las herramientas de análisis estático **Facebook Infer** y **SonarQube**. Estas herramientas permiten identificar posibles errores o vulnerabilidades en el código.

1. **Facebook Infer**:
    
    ```bash
    infer run -- make
    
    ```
    
2. **SonarQube**:
Configurar el proyecto en SonarQube para realizar un análisis exhaustivo del código fuente.

## **Autores**

- **Sergio Oliveros**
- **Daniel Álvarez**
- **Sebastian Urrea**
- **Haider Fonseca**
- **Daniel Perea**

---

Este README proporciona una guía detallada para utilizar, compilar, y entender el proyecto con una estructura bien definida para cada parte del código.