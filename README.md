# rockpush
videogame

[![Rockpush](https://img.youtube.com/vi/fOq6kgN9nt8/0.jpg)](https://www.youtube.com/watch?v=fOq6kgN9nt8)

Instrucciones de compilación para GNU/Linux:

	- Instalar los paquetes de desarrollo correspondiente a:
	
		libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev

	- Sus dependencias binarias en caso de no usar gestor de paquetes.
	- Tener instaladas las herramientas de desarrollo GNU (cmake, gcc, etc.)
	- En el directorio del proyecto, crear el directorio build. Entrar en él y escribir:

		cmake ../
		make
		cd ..
		build/rockpush

Instrucciones de compilación para Windows:

	- Instalar CMake y MinGW. Configurar correctamente las variables de entorno del sistema.
	- Instalar las bibliotecas de desarrollo de SDL2, SDL2 Mixer, SDL2 TTF y SDL2 Image.
	- Renombrar el archivo CMakeLists.mingw32 a CMakeLists.txt (sobreescribir el que haya)
	- Adaptar las opciones de compilación dentro del archivo. Por ejemplo las rutas de la biblioteca SDL2.
	- Crear una carpeta dentro del proyecto con nombre build
	- Entrar en la carpeta build y, desde línea de comandos, escribir:

		cmake -G "MinGW Makefiles" ../
		mingw32-make
		
	- Se puede descargar una versión precompilada desde:
		https://mega.nz/#!lcwWSQAR
