#!/bin/bash

# Ir al directorio de compilación
cd CodefestAdAstra2024/build

# Generar los archivos Makefile con cmake
cmake ..

# Compilar el proyecto
make 

# Verifica si el ejecutable fue creado
if [ -f CodefestAdAstra2024 ]; then
  # Mover el ejecutable al directorio superior y cambiarle el nombre
  mv CodefestAdAstra2024 ../../CodefestAdAstra2024.sh
  cd ../..
  # Ejecutar el script renombrado (ejecutable) con los argumentos pasados al script
  ./CodefestAdAstra2024.sh "$@"
else
  echo "Error: El ejecutable no fue generado. Revisa los errores de compilación."
fi
