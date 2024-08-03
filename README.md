# AIr Reinforcement - Codefest Ad Astra 2024

## ¿Cómo compliar el código? 

### Sistemas Linux o basados en Unix
Basta con ejecutar el archivo build_and_run.sh. La forma de cómo realizar esto se presenta a continuación: 

```
./build_and_run.sh
```
Esto generará el archivo ```CodefestAdAstra2024.sh``` que es el ejecutable el programa. 

### Sistemas Windows
Para esto es necesario tener instalado algún progrma capaz de ejecutar archivos make. 
Hay diferentes opciones como: 

#### _MinGW (Minimalist GNU for Windows)_
1. Con MinGW instalado, instalar el paqueute ```mingw32-make```
2. Ir a la carpeta   _CodefestAdAstra2024/build_
3. Ejecutar ```make```
4. El resultado será el ejecutable _CodefestAdAstra2024_

#### _Cygwin_
1. Seleccionar el paquete _make_ en el instalador de _Cygwin_ 
2. Ir a la carpeta   _CodefestAdAstra2024/build_
3. Ejecutar ```make```
4. El resultado será el ejecutable _CodefestAdAstra2024_

#### _Visual Studio_
En caso de tener Visual Studio:
1. Ir a la carpeta   _CodefestAdAstra2024/build_
2. Ejecutar ```nmake```
3. 4. El resultado será el ejecutable _CodefestAdAstra2024_

#### _Experimental_
Nosotros no logramos hacer funcionar esta opción, pero en caso de tener Cmake en windows, la siguiente opción puede serivr para crear unejecutable del proyecto. 
1. Ejecutar el comando:
```
./build_and_run.bat
```

#### Otro 
Se pueden encontrar otras formas de compilar el código si se busca ¿cómo ejecutar un _makefile_ en Windows?





##### Realizado Por:
1. Sergio Oliveros
2. Daniel Álavarez
3. Sebastian Urrea
4. Andrés Duarte
