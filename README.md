# 🚀 Proyecto SQF - [Añade un subtítulo o descripción aquí]

Este repositorio contiene la estructura base y el código fuente del proyecto para la asignatura de [Nombre de la Asignatura / Máster]. Está configurado para compilar C++ utilizando **CMake** y el framework de testing de **Boost**.


## ⚠️ Requisitos Previos (¡Muy Importante!)

Para que el proyecto compile correctamente en **Windows**, es estrictamente necesario tener el siguiente entorno configurado:

1. **CMake** (Versión 3.10 o superior).
2. **MinGW-w64** (Recomendado GCC 15.2.0 o similar, instalado a través de MSYS2).
3. **Boost 1.90.0**: 
   * Debe estar extraído en `C:/boost_1_90_0`. SI NO, poner vuestra Dirección de boost, también en el archivo **CMakeBoostTest.cmake**
   * **Importante:** Las librerías de Boost deben estar compiladas estáticamente (`.a`). Los binarios deben existir en `C:/boost_1_90_0/stage/lib`. De nuevo, cambiar la ruta si no

> 🛑 **REGLA DE ORO SOBRE LAS RUTAS:**
> El compilador de MinGW falla si hay espacios o caracteres especiales (como **tildes** o eñes) ens