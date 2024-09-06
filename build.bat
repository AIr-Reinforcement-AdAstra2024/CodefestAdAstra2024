@echo off

:: Cambiar al directorio de compilación
cd CodefestAdAstra2024\build

:: Generar los archivos de construcción con cmake
cmake ..

:: Compilar el proyecto
make

:: Verificar si el ejecutable fue creado
if exist CodefestAdAstra2024.exe (
    :: Mover el ejecutable al directorio superior
    move CodefestAdAstra2024.exe ..\..\CodefestAdAstra2024.exe
) else (
    echo Error: El ejecutable no fue generado. Revisa los errores de compilación.
)
