# Proyecto_compu_grafica

#Manual de usuario
Movimiento
A - izquierda
W - arriba
S - abajo
D - derecha
raton - camara

Animaciones
1 - puertas exteriores
2,3,4 - puertas de mesa central
5,6,7 - puertas de refrigeradores
8 - puerta de metal
i - sopa
o - hamburguesa
p - papas fritas
l - trastes

#Manual Tecnico
Dentro de un proyecto en blanco c++ en visual estudio se copiaran los archivo que hay en el zip.
	-carpetas
		Sounds
		Models
		Textures
	-archivos
		proyecto.cpp
		window.cpp
		window.h
		CommonValues.h

Todos los demas archivos seran los mismo que se han usado en la clase, en caso de cualquier imprevisto se dejara una carpeta con los archivo llamada respaldo.
Dentro del proyecto configuraremos sus propiedades de la siguiente manera:
	Configuracion: Todas las configuraciones.
	Plataforma: Win32.
	C/C++ -> General -> Directorios de inclusion adicionales: include;glm;
	Vinculador -> General -> Directorio de bibliotecas adicionales: lib;
	Vinculador -> Entrada -> Dependencias Adicionales: winmm.lib;glfw3.lib;glew32.lib;opengl32.lib;assimp-vc140-mt.lib;

Si los modelos presentan una falla en texturas y muestra una textura de error, se debe a los .mtl de los objetos de cada modelo, se tiene que cambiar la direccion a la direccion donde se encuentren las texturas.
En caso de volver a fallar al momento de compilar el proyecto se puede obtener del repositorio de GitHub https://github.com/AndresAmadeus/Proyecto_compu_grafica.
