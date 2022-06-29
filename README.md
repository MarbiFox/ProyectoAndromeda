
Grupo Andromeda:

	Benjamin Diaz
	Benjamin Fernandez
	Thomas Molina
	Jorge Palacios


para compilar el programa nesesita instalr tdm gcc 64 

link directo:https://github.com/jmeubank/tdm-gcc/releases/download/v10.3.0-tdm64-2/tdm64-gcc-10.3.0-2.exe

link de la pagina para cualquier consulta del compilador: https://jmeubank.github.io/tdm-gcc/

(utilizamos este porque la libreria sdl requeria un compilador de 64 bits y el mignw32 no ofrese esta opcion)


copie el repositorio del github y abralo en Visual Studio Code y en la terminal powershell ejecute el siguente comando:

 	"gcc -I source/include -L source/lib menu.c list.c -o GalaticMadness.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_IMAGE" (sin las comillas)

luego valla a la carpeta del repositorio y ejecute el "GalaticMadness.exe" y el juego deberia ejecutarse


ERRORES:
	
	hay opciones que no funcionan como "ranking" y "opciones"
	
	ranking esta implementada en codigo pero por falta de tiempo no pudimos implimentarlos en la aplicacion
	todas sus funciones funcionan

	cuando cambia de nivel tiene que elegir la ventana para poder jugar denuevo (se dara cuenta cuando pase de nivel)

	


