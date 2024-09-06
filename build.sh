#!/bin/bash

# Ir al directorio de compilación
mkdir -p CodefestAdAstra2024/build
cd CodefestAdAstra2024/build

# Generar los archivos Makefile con cmake
cmake ..

# Compilar el proyecto
make 

# Verifica si el ejecutable fue creado
if [ -f CodefestAdAstra2024 ]; then
  # Mover el ejecutable al directorio superior y cambiarle el nombre
  mv CodefestAdAstra2024 ../../CodefestAdAstra2024.sh
else
  echo "Error: El ejecutable no fue generado. Revisa los errores de compilación."
fi
