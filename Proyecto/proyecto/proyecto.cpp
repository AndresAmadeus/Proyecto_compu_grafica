/*
Semestre 2021-2
Práctica : Iluminación
Cambios en el shader, en lugar de enviar la textura en el shader de fragmentos, enviaremos el finalcolor
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h> 

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//sonido
#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<mmsystem.h>
#include <thread>
#include <string>
using namespace std;

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;

//modelos de cocina
Model mesa_centro;
Model mesa_centro_aceite;
Model mesa_centro_freidora1;
Model mesa_centro_freidora2;
Model mesa_centro_puerta1;
Model mesa_centro_puerta2;
Model mesa_centro_puerta3;
Model mesa_centro_puerta4;
Model estructura;
Model lavadero;
Model mesa1;
Model mesa2;
Model puerta_metal;
Model mesa_ext_1;
Model mesa_ext_2;
Model mesa_ext_3;
Model silla_ext_1;
Model silla_ext_2;
Model silla_ext_3;
Model silla_ext_4;
Model silla_ext_5;
Model silla_ext_6;
Model silla_ext_7;
Model silla_ext_8;
Model silla_ext_9;
Model silla_ext_10;
Model silla_ext_11;
Model silla_ext_12;
Model puerta_ext_1;
Model puerta_ext_2;
Model estante_1;
Model estante_2;
Model refri_1;
Model refri_2;
Model refri_3;
Model refri_puerta_1;
Model refri_puerta_2;
Model refri_puerta_3;
Model papas_1;
Model papas_2;
Model papas_3;
Model papas_bolsa_1;
Model papas_bolsa_2;
Model papas_bolsa_3;
Model papas_pinzas;
Model plato_1;
Model plato_2;
Model olla_1;
Model olla_2;
Model olla_3;
Model cucharon;
Model tabla;
Model pan_1;
Model pan_2;
Model carne;
Model lechuga;
Model jitomate;
Model plato_sucio_1;
Model plato_sucio_2;
Model trastes_sucios;
Model estropajo_1;
Model estropajo_2;
Model extractor_1;
Model extractor_2;
Model chef;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Sonidos
void play_music(int n) {
	switch (n) {
	case 1:
		PlaySound(TEXT("Sounds/07019076.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP);
		break; //fondo
	case 2:
		PlaySound(TEXT("Sounds/bbc_household _07002109.wav"), NULL, SND_ASYNC);
		break; //servir
	case 3:
		PlaySound(TEXT("Sounds/bbc_household _07067001.wav"), NULL, SND_ASYNC);
		break; //lavar
	case 4:
		PlaySound(TEXT("Sounds/bbc_kitchen ap_07011060.wav"), NULL, SND_ASYNC);
		break; //puerta pequeña
	case 5:
		PlaySound(TEXT("Sounds/bbc_steel door_07037627.wav"), NULL, SND_ASYNC );
		break; //metal abrir
	}
	
}




//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}




void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	mesa_centro = Model();
	mesa_centro.LoadModel("Models/mesa_centro.obj");
	mesa_centro_aceite = Model();
	mesa_centro_aceite.LoadModel("Models/mesa_centro_aceite.obj");
	mesa_centro_freidora1 = Model();
	mesa_centro_freidora1.LoadModel("Models/mesa_centro_freidora1.obj");
	mesa_centro_freidora2 = Model();
	mesa_centro_freidora2.LoadModel("Models/mesa_centro_freidora2.obj");
	mesa_centro_puerta1 = Model();
	mesa_centro_puerta1.LoadModel("Models/mesa_centro_puerta1.obj");
	mesa_centro_puerta2 = Model();
	mesa_centro_puerta2.LoadModel("Models/mesa_centro_puerta2.obj");
	mesa_centro_puerta3 = Model();
	mesa_centro_puerta3.LoadModel("Models/mesa_centro_puerta3.obj");
	mesa_centro_puerta4 = Model();
	mesa_centro_puerta4.LoadModel("Models/mesa_centro_puerta4.obj");
	estructura = Model();
	estructura.LoadModel("Models/estructura.obj");
	lavadero = Model();
	lavadero.LoadModel("Models/lavadero.obj");
	mesa1 = Model();
	mesa1.LoadModel("Models/mesa1.obj");
	mesa2 = Model();
	mesa2.LoadModel("Models/mesa2.obj");
	puerta_metal = Model();
	puerta_metal.LoadModel("Models/puerta_metal.obj");
	mesa_ext_1 = Model();
	mesa_ext_1.LoadModel("Models/mesa_ext_1.obj");
	mesa_ext_2 = Model();
	mesa_ext_2.LoadModel("Models/mesa_ext_2.obj");
	mesa_ext_3 = Model();
	mesa_ext_3.LoadModel("Models/mesa_ext_3.obj");
	silla_ext_1 = Model();
	silla_ext_1.LoadModel("Models/silla_ext_1.obj");
	silla_ext_2 = Model();
	silla_ext_2.LoadModel("Models/silla_ext_2.obj");
	silla_ext_3 = Model();
	silla_ext_3.LoadModel("Models/silla_ext_3.obj");
	silla_ext_4 = Model();
	silla_ext_4.LoadModel("Models/silla_ext_4.obj");
	silla_ext_5 = Model();
	silla_ext_5.LoadModel("Models/silla_ext_5.obj");
	silla_ext_6 = Model();
	silla_ext_6.LoadModel("Models/silla_ext_6.obj");
	silla_ext_7 = Model();
	silla_ext_7.LoadModel("Models/silla_ext_7.obj");
	silla_ext_8 = Model();
	silla_ext_8.LoadModel("Models/silla_ext_8.obj");
	silla_ext_9 = Model();
	silla_ext_9.LoadModel("Models/silla_ext_9.obj");
	silla_ext_10 = Model();
	silla_ext_10.LoadModel("Models/silla_ext_10.obj");
	silla_ext_11 = Model();
	silla_ext_11.LoadModel("Models/silla_ext_11.obj");
	silla_ext_12 = Model();
	silla_ext_12.LoadModel("Models/silla_ext_12.obj");
	puerta_ext_1 = Model();
	puerta_ext_1.LoadModel("Models/puerta_ext_1.obj");
	puerta_ext_2 = Model();
	puerta_ext_2.LoadModel("Models/puerta_ext_2.obj");
	estante_1 = Model();
	estante_1.LoadModel("Models/estante_1.obj");
	estante_2 = Model();
	estante_2.LoadModel("Models/estante_2.obj");
	refri_1 = Model();
	refri_1.LoadModel("Models/refri_1.obj");
	refri_2 = Model();
	refri_2.LoadModel("Models/refri_2.obj");
	refri_3 = Model();
	refri_3.LoadModel("Models/refri_3.obj");
	refri_puerta_1 = Model();
	refri_puerta_1.LoadModel("Models/refri_puerta_1.obj");
	refri_puerta_2 = Model();
	refri_puerta_2.LoadModel("Models/refri_puerta_2.obj");
	refri_puerta_3 = Model();
	refri_puerta_3.LoadModel("Models/refri_puerta_3.obj");
	papas_1 = Model();
	papas_1.LoadModel("Models/papas_1.obj");
	papas_2 = Model();
	papas_2.LoadModel("Models/papas_2.obj");
	papas_3 = Model();
	papas_3.LoadModel("Models/papas_3.obj");
	papas_bolsa_1 = Model();
	papas_bolsa_1.LoadModel("Models/papas_bolsa_1.obj");
	papas_bolsa_2 = Model();
	papas_bolsa_2.LoadModel("Models/papas_bolsa_2.obj");
	papas_bolsa_3 = Model();
	papas_bolsa_3.LoadModel("Models/papas_bolsa_3.obj");
	papas_pinzas = Model();
	papas_pinzas.LoadModel("Models/papas_pinzas.obj");
	plato_1 = Model();
	plato_1.LoadModel("Models/plato_1.obj");
	plato_2 = Model();
	plato_2.LoadModel("Models/plato_2.obj");
	olla_1 = Model();
	olla_1.LoadModel("Models/olla_1.obj");
	olla_2 = Model();
	olla_2.LoadModel("Models/olla_2.obj");
	olla_3 = Model();
	olla_3.LoadModel("Models/olla_3.obj");
	cucharon = Model();
	cucharon.LoadModel("Models/cucharon.obj");
	tabla = Model();
	tabla.LoadModel("Models/tabla.obj");
	pan_1 = Model();
	pan_1.LoadModel("Models/pan_1.obj");
	pan_2 = Model();
	pan_2.LoadModel("Models/pan_2.obj");
	carne = Model();
	carne.LoadModel("Models/carne.obj");
	lechuga = Model();
	lechuga.LoadModel("Models/lechuga.obj");
	jitomate = Model();
	jitomate.LoadModel("Models/jitomate.obj");
	plato_sucio_1 = Model();
	plato_sucio_1.LoadModel("Models/plato_sucio_1.obj");
	plato_sucio_2 = Model();
	plato_sucio_2.LoadModel("Models/plato_sucio_2.obj");
	trastes_sucios = Model();
	trastes_sucios.LoadModel("Models/trastes_sucios.obj");
	estropajo_1 = Model();
	estropajo_1.LoadModel("Models/estropajo_1.obj");
	estropajo_2 = Model();
	estropajo_2.LoadModel("Models/estropajo_2.obj");
	extractor_1 = Model();
	extractor_1.LoadModel("Models/extractor_1.obj");
	extractor_2 = Model();
	extractor_2.LoadModel("Models/extractor_2.obj");
	chef = Model();
	chef.LoadModel("Models/chef.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	 
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	/*
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		2.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	*/
	unsigned int spotLightCount = 0;
	//luces del interior
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, //color
		0.0f, 2.0f,				//aln dln
		0.0f, 0.0f, 0.0f, 		//pos
		0.0f, -1.0f, 0.0f,		//direc
		1.0f, 0.0f, 0.0f,		//poli
		15.0f);					//tam
	spotLightCount++;
	
	/*
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		-3.5f, 4.5f, -4.7f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		3.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;
	
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		-3.5f, 4.5f, 2.7f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		3.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;
	/**/
	
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		4.7f, 4.5f, -4.6f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		2.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;

	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		4.7f, 4.5f, -1.0f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		2.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;
	/*
	
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		-0.0f, 4.5f, -4.6f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		2.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;

	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		0.0f, 4.5f, -1.0f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		2.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;

	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		1.0f, 4.5f, 3.3f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		2.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;

	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f, //color blanco
		0.0f, 2.0f,				//aln dln
		3.5f, 4.5f, 3.3f, 	//pos
		0.0f, -1.0f, 0.0f,		//direc
		2.0f, 0.0f, 0.0f,		//poli
		80.0f);					//tam
	spotLightCount++;
	/**/
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	

	//Variables de animaciones
	GLfloat del_mod = 100.0f;

	//Sonido de animacion
	bool musica_fondo = true;


	//puerta_ext
	GLfloat ani_puerta_ext_tm = 0.0f;
	bool ani_puerta_ext = false;
	GLfloat giro = 0.0f;
	GLfloat ani_puerta_ext_tm01 = 2.0f;
	GLfloat ani_puerta_ext_tm01_std = true;

	//puerta_cnt01
	GLfloat ani_puerta_cnt01_tm = 0.0f;
	bool ani_puerta_cnt01 = false;
	GLfloat giro_puerta_cnt01 = 0.0f;
	GLfloat ani_puerta_cnt01_tm01 = 2.0f;
	GLfloat ani_puerta_cnt01_tm01_std = true;

	//puerta_cnt02
	GLfloat ani_puerta_cnt02_tm = 0.0f;
	bool ani_puerta_cnt02 = false;
	GLfloat giro_puerta_cnt02 = 0.0f;
	GLfloat ani_puerta_cnt02_tm01 = 2.0f;
	GLfloat ani_puerta_cnt02_tm01_std = true;

	//puerta_cnt03
	GLfloat ani_puerta_cnt03_tm = 0.0f;
	bool ani_puerta_cnt03 = false;
	GLfloat giro_puerta_cnt03 = 0.0f;
	GLfloat ani_puerta_cnt03_tm01 = 2.0f;
	GLfloat ani_puerta_cnt03_tm01_std = true;

	//puerta_ref01
	GLfloat ani_puerta_ref01_tm = 0.0f;
	bool ani_puerta_ref01 = false;
	GLfloat giro_puerta_ref01 = 0.0f;
	GLfloat ani_puerta_ref01_tm01 = 2.0f;
	GLfloat ani_puerta_ref01_tm01_std = true;

	//puerta_ref01
	GLfloat ani_puerta_ref02_tm = 0.0f;
	bool ani_puerta_ref02 = false;
	GLfloat giro_puerta_ref02 = 0.0f;
	GLfloat ani_puerta_ref02_tm01 = 2.0f;
	GLfloat ani_puerta_ref02_tm01_std = true;

	//puerta_ref01
	GLfloat ani_puerta_ref03_tm = 0.0f;
	bool ani_puerta_ref03 = false;
	GLfloat giro_puerta_ref03 = 0.0f;
	GLfloat ani_puerta_ref03_tm01 = 2.0f;
	GLfloat ani_puerta_ref03_tm01_std = true;

	//puerta metal
	GLfloat ani_puerta_mtl_tm = 0.0f;
	bool ani_puerta_mtl = false;
	GLfloat giro_puerta_mtl = 0.0f;
	GLfloat ani_puerta_mtl_tm01 = 2.0f;
	GLfloat ani_puerta_mtl_tm01_std = true;


	//papas fritas
	GLfloat ani_papas_tm = 0.0f;
	GLfloat ani_papas_posX = 0.0f, ani_papas_posY = 0.0f;
	GLfloat ani_papas_posX_P1 = 0.0f, ani_papas_posY_P1 = 0.0f;
	GLfloat ani_papas_posX_P2 = 0.0f, ani_papas_posY_P2 = 0.0f;
	GLfloat ani_papas_posX_P3 = 0.0f, ani_papas_posY_P3 = 0.0f;
	bool ani_papas = false;
	GLfloat ani_papas_tm01 = 2.0f;
	GLfloat ani_papas_tm02 = 2*ani_papas_tm01;
	GLfloat ani_papas_tm03 = 3*ani_papas_tm01;
	GLfloat ani_papas_tm04 = 4*ani_papas_tm01;
	GLfloat ani_papas_tm05 = 5*ani_papas_tm01;
	
	//Hamburguesa
	GLfloat ani_ham_tm = 0.0f;
	bool ani_ham = false;
	GLfloat ani_ham_tm01 = 2.0f;
	GLfloat ani_ham_tm02 = 2 * ani_ham_tm01;
	GLfloat ani_ham_tm03 = 3 * ani_ham_tm01;
	GLfloat ani_ham_tm04 = 4 * ani_ham_tm01;
	GLfloat ani_ham_tm05 = 5 * ani_ham_tm01;
	GLfloat ani_ham_posX_P1 = 0.0f, ani_ham_posY_P1 = 0.0f, ani_ham_posZ_P1 = 0.0f;
	GLfloat ani_ham_posX_P2 = 0.0f, ani_ham_posY_P2 = 0.0f, ani_ham_posZ_P2 = 0.0f;
	GLfloat ani_ham_posX_P3 = 0.0f, ani_ham_posY_P3 = 0.0f, ani_ham_posZ_P3 = 0.0f;
	GLfloat ani_ham_posX_P4 = 0.0f, ani_ham_posY_P4 = 0.0f, ani_ham_posZ_P4 = 0.0f;
	GLfloat ani_ham_posX_P5 = 0.0f, ani_ham_posY_P5 = 0.0f, ani_ham_posZ_P5 = 0.0f;


	//Trastes sucios
	GLfloat ani_tra_tm = 0.0f;
	bool ani_tra = false;
	GLfloat ani_tra_posZ = 0.0f, ani_tra_posY = 0.0f;
	GLfloat ani_tra_posY_P1 = 0.0f, ani_tra_posY_P2 = 0.0f;
	GLfloat ani_tra_tm01 = 2.0f;
	GLfloat ani_tra_tm02 = 2 * ani_tra_tm01;
	GLfloat ani_tra_tm03 = 3 * ani_tra_tm01;
	GLfloat ani_tra_tm04 = 4 * ani_tra_tm01;
	GLfloat ani_tra_tm05 = 5 * ani_tra_tm01;

	//Extractores
	GLfloat ani_extr = 0.0f;

	//sopa
	bool ani_sopa = false;
	GLfloat ani_sopa_tm = 0.0f;
	GLfloat ani_sopa_posX = 0.0, ani_sopa_posY = 0.0, ani_sopa_posZ = 0.0;
	GLfloat ani_sopa_giro = 0.0f;
	GLfloat ani_sopa_tm01 = 2.0f;
	GLfloat ani_sopa_tm02 = ani_sopa_tm01 + 2.0f;
	GLfloat ani_sopa_tm03 = ani_sopa_tm02 + 2.0f;
	GLfloat ani_sopa_tm04 = ani_sopa_tm03 + 2.5f;
	GLfloat ani_sopa_tm05 = ani_sopa_tm04 + 2.5f;
	GLfloat ani_sopa_tm06 = ani_sopa_tm05 + 2.0f;
	GLfloat ani_sopa_tm07 = ani_sopa_tm06 + 2.0f;
	GLfloat ani_sopa_tm08 = ani_sopa_tm07 + 2.0f;

	GLfloat x, z, y;

	thread th_01, th_02, th_03, th_04, th_05, th_06;


	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / (limitFPS);
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		estructura.RenderModel();

		//----------------------------------------------------------------------------------
		//---------------------------------ANIMACIONES--------------------------------------
		//----------------------------------------------------------------------------------


		x = mainWindow.getmuevex();
		z = mainWindow.getmuevez();
		y = mainWindow.getmuevey();

		printf(" %.2ff, %.2ff, %.2ff  \n", x, y, z);
		//model = glm::translate(model, glm::vec3(0.0f + x, 0.0f + y, 0.0f + z));

		//puerta_ext
		if (mainWindow.get_btn_1()) {
			mainWindow.res_btn_1();
			ani_puerta_ext = true;
			ani_puerta_ext_tm = 0.0f;
		}

		if (ani_puerta_ext) {
			ani_puerta_ext_tm += deltaTime / del_mod;
			if (ani_puerta_ext_tm < ani_puerta_ext_tm01) {
				if (ani_puerta_ext_tm01_std) {
					giro = 90.0f * (ani_puerta_ext_tm / ani_puerta_ext_tm01);
				}
				else {
					giro = 90.0f - 90.0f * (ani_puerta_ext_tm / ani_puerta_ext_tm01);
				}
			} else {
				ani_puerta_ext = false;
				ani_puerta_ext_tm01_std = !ani_puerta_ext_tm01_std;
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3( -1.49f, 1.13f, -0.06f));
		model = glm::rotate(model, giro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		puerta_ext_1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3( -1.49f, 1.13f, -2.54f));
		model = glm::rotate(model, - giro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		puerta_ext_2.RenderModel();

		//papas fritas
		if (mainWindow.get_btn_p()) {
			mainWindow.res_btn_p();
			ani_papas = true;
			ani_papas_tm = 0.0f;
			ani_papas_posX_P1 = 0;
			ani_papas_posX_P2 = 0;
			ani_papas_posX_P3 = 0;
			ani_papas_posY_P1 = 0;
			ani_papas_posY_P2 = 0;
			ani_papas_posY_P3 = 0;
		}

		if (ani_papas) {
			ani_papas_tm += deltaTime / del_mod;
			if (ani_papas_tm < ani_papas_tm01) {
				ani_papas_posX = 0.66 * ani_papas_tm / ani_papas_tm01;
				ani_papas_posY = -4.014*pow((ani_papas_posX - 0.386),2) + 0.6;

				ani_papas_posX_P1 = ani_papas_posX;
				ani_papas_posY_P1 = ani_papas_posY;
			}
			else if (ani_papas_tm < ani_papas_tm02){
				ani_papas_posX = 0.66 - 0.78 * (ani_papas_tm- ani_papas_tm01) / ani_papas_tm01;
				ani_papas_posY = -2.873 * pow((ani_papas_posX - 0.336), 2) + 0.6;
			}
			else if (ani_papas_tm < ani_papas_tm03) {
				ani_papas_posX = -0.12 + 0.93 *(ani_papas_tm - ani_papas_tm02) / ani_papas_tm01;
				ani_papas_posY = -2.021 * pow((ani_papas_posX -0.424), 2) + 0.6;

				ani_papas_posX_P2 = ani_papas_posX +0.12;
				ani_papas_posY_P2 = ani_papas_posY;
			}
			else if (ani_papas_tm < ani_papas_tm04) {
				ani_papas_posX = 0.81 - 1.06 *(ani_papas_tm - ani_papas_tm03) / ani_papas_tm01;
				ani_papas_posY = -1.556 * pow((ani_papas_posX - 0.370), 2) + 0.6;
			}
			else if (ani_papas_tm < ani_papas_tm05) {
				ani_papas_posX = -0.25 + 1.18 *(ani_papas_tm - ani_papas_tm04) / ani_papas_tm01;
				ani_papas_posY = -1.255 * pow((ani_papas_posX - 0.441), 2) + 0.6;

				ani_papas_posX_P3 = ani_papas_posX +0.25;
				ani_papas_posY_P3 = ani_papas_posY;
			}
			else {
				ani_papas = false;
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_papas_posX_P3, 0.0f + ani_papas_posY_P3, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_papas_posX_P2, 0.0f + ani_papas_posY_P2, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_2.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_papas_posX_P1, 0.0f + ani_papas_posY_P1, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_3.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_papas_posX, 0.0f + ani_papas_posY, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_pinzas.RenderModel();


		//Servir platos de sopa
		if (mainWindow.get_btn_i()) {
			mainWindow.res_btn_i();
			ani_sopa = true;
			ani_sopa_tm = 0.0f;

			thread th_02(play_music, 2);
			if (th_02.joinable()) {
				th_02.join();
			}

		}

		if (ani_sopa) {
			ani_sopa_tm += deltaTime / del_mod;
			if (ani_sopa_tm < ani_sopa_tm01) {
				ani_sopa_posX = 0.83 * (ani_sopa_tm / ani_sopa_tm01); 
				ani_sopa_posY = -4.528 * pow( (ani_sopa_posX - 0.42) ,2) +0.8;
				ani_sopa_posZ = -0.08 * (ani_sopa_tm / ani_sopa_tm01);
			}
			else if (ani_sopa_tm < ani_sopa_tm02) {
				ani_sopa_giro = 70 * ((ani_sopa_tm - ani_sopa_tm01) / ani_sopa_tm01);
			}
			else if (ani_sopa_tm < ani_sopa_tm03) {
				ani_sopa_giro = 70 - 70 * ((ani_sopa_tm - ani_sopa_tm02) / ani_sopa_tm01);
			}
			else if (ani_sopa_tm < ani_sopa_tm04) {
				ani_sopa_posX = 0.83 - 0.83 * ((ani_sopa_tm - ani_sopa_tm03) / 2.5);
				ani_sopa_posY = -4.528 * pow((ani_sopa_posX - 0.42), 2) + 0.8;
				ani_sopa_posZ = -0.08 + 0.08 * ((ani_sopa_tm - ani_sopa_tm03) / 2.5);
			}
			else if (ani_sopa_tm < ani_sopa_tm05) {
				ani_sopa_posX = 0.83 * ((ani_sopa_tm - ani_sopa_tm04) / 2.5);
				ani_sopa_posY = -4.528 * pow((ani_sopa_posX - 0.42), 2) + 0.8;
				ani_sopa_posZ = -0.32 * ((ani_sopa_tm - ani_sopa_tm04) / 2.5);
			}
			else if (ani_sopa_tm < ani_sopa_tm06) {
				ani_sopa_giro = 70 * ((ani_sopa_tm - ani_sopa_tm05) / ani_sopa_tm01);
			}
			else if (ani_sopa_tm < ani_sopa_tm07) {
				ani_sopa_giro = 70 - 70 * ((ani_sopa_tm - ani_sopa_tm06) / ani_sopa_tm01);
			}
			else if (ani_sopa_tm < ani_sopa_tm08) {
				ani_sopa_posX = 0.83 - 0.83 * ((ani_sopa_tm - ani_sopa_tm07) / ani_sopa_tm01);
				ani_sopa_posY = -4.528 * pow((ani_sopa_posX - 0.42), 2) + 0.8;
				ani_sopa_posZ = -0.32 + 0.32 * ((ani_sopa_tm - ani_sopa_tm07) / ani_sopa_tm01);
			}
			else {
				ani_sopa = false;
				PlaySound(NULL, NULL, 0);
				thread th_01(play_music, 1);
				if (th_01.joinable()) {
					th_01.join();
				}
			}
			
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.52f + ani_sopa_posX, 1.66f + ani_sopa_posY, -2.31f + ani_sopa_posZ));
		model = glm::rotate(model, ani_sopa_giro * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cucharon.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		plato_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		plato_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		olla_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		olla_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		olla_3.RenderModel();

		//Hamburguesa
		if (mainWindow.get_btn_o()) {
			mainWindow.res_btn_o();
			ani_ham = true;
			ani_ham_tm = 0.0f;
			ani_ham_posX_P1 = 0.0f; ani_ham_posY_P1 = 0.0f; ani_ham_posZ_P1 = 0.0f;
			ani_ham_posX_P2 = 0.0f; ani_ham_posY_P2 = 0.0f; ani_ham_posZ_P2 = 0.0f;
			ani_ham_posX_P3 = 0.0f; ani_ham_posY_P3 = 0.0f; ani_ham_posZ_P3 = 0.0f;
			ani_ham_posX_P4 = 0.0f; ani_ham_posY_P4 = 0.0f; ani_ham_posZ_P4 = 0.0f;
			ani_ham_posX_P5 = 0.0f; ani_ham_posY_P5 = 0.0f; ani_ham_posZ_P5 = 0.0f;
		}

		if (ani_ham) {
			ani_ham_tm += deltaTime / del_mod;
			if (ani_ham_tm < ani_ham_tm01) {
				ani_ham_posX_P1 = 0.3 * (ani_ham_tm / ani_ham_tm01);
				ani_ham_posY_P1 = -13.3 * pow((ani_ham_posX_P1 - 0.15), 2) + 0.3;
				ani_ham_posZ_P1 = 0.4 * (ani_ham_tm / ani_ham_tm01);
			}
			else if (ani_ham_tm < ani_ham_tm02) {
				ani_ham_posX_P2 = 0.3 * ( (ani_ham_tm - ani_ham_tm01) / ani_ham_tm01);
				ani_ham_posY_P2 = -12.885 * pow((ani_ham_posX_P2 - 0.152), 2) + 0.3;
				ani_ham_posZ_P2 = -0.2 * ((ani_ham_tm - ani_ham_tm01) / ani_ham_tm01);
			}
			else if (ani_ham_tm < ani_ham_tm03) {
				ani_ham_posX_P3 = 0.3 * ((ani_ham_tm - ani_ham_tm02) / ani_ham_tm01);
				ani_ham_posY_P3 = -12.428 * pow((ani_ham_posX_P3 - 0.155), 2) + 0.3;
				ani_ham_posZ_P3 = 0.0f;
			}
			else if (ani_ham_tm < ani_ham_tm04) {
				ani_ham_posX_P4 = 0.3 * ((ani_ham_tm - ani_ham_tm03) / ani_ham_tm01);
				ani_ham_posY_P4 = -11.96 * pow((ani_ham_posX_P4 - 0.158), 2) + 0.3;
				ani_ham_posZ_P4 = 0.2 * ((ani_ham_tm - ani_ham_tm03) / ani_ham_tm01);
			}
			else if (ani_ham_tm < ani_ham_tm05) {
				ani_ham_posX_P5 = 0.3 * ((ani_ham_tm - ani_ham_tm04) / ani_ham_tm01);
				ani_ham_posY_P5 = -11.486 * pow((ani_ham_posX_P5 - 0.161), 2) + 0.3;
				ani_ham_posZ_P5 = -0.4 * ((ani_ham_tm - ani_ham_tm04) / ani_ham_tm01);
			}
			else {
				ani_ham = false;
			}
		}

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		tabla.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_ham_posX_P5, 0.0f + ani_ham_posY_P5, 0.0f + ani_ham_posZ_P5));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pan_1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_ham_posX_P1, 0.0f + ani_ham_posY_P1, 0.0f + ani_ham_posZ_P1));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pan_2.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_ham_posX_P2, 0.0f + ani_ham_posY_P2, 0.0f + ani_ham_posZ_P2));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		carne.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_ham_posX_P3, 0.0f + ani_ham_posY_P3, 0.0f + ani_ham_posZ_P3));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lechuga.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + ani_ham_posX_P4, 0.0f + ani_ham_posY_P4, 0.0f + ani_ham_posZ_P4));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		jitomate.RenderModel();

		//trastes sucios
		if (mainWindow.get_btn_l()) {
			mainWindow.res_btn_l();
			ani_tra = true;
			ani_tra_tm = 0.0f;
			ani_tra_posY_P1 = 0.0f;
			ani_tra_posY_P2 = 0.0f;
			ani_tra_posY = 0.0;
			ani_tra_posZ = 0.0;

			
			thread th_03(play_music, 3);
			if (th_03.joinable()) {
				th_03.join();
			}
		}

		if (ani_tra) {
			ani_tra_tm += deltaTime / del_mod;
			if (ani_tra_tm < ani_tra_tm01) {
				ani_tra_posY =  0.50 * (ani_tra_tm / ani_tra_tm01);
				ani_tra_posY_P1 = ani_tra_posY;
			}
			else if (ani_tra_tm < ani_tra_tm02) {
				ani_tra_posY = 0.50 + 0.02 * sinf(((ani_tra_tm - ani_tra_tm01) / ani_tra_tm01) * 50);
				ani_tra_posZ = -0.02 * sinf(((ani_tra_tm - ani_tra_tm01) / ani_tra_tm01) * 50);
			}
			else if (ani_tra_tm < ani_tra_tm03) {
				ani_tra_posY = 0.50;
				ani_tra_posZ = 0.47 * ((ani_tra_tm - ani_tra_tm02) / ani_tra_tm01);
				ani_tra_posY_P1 = 0.50 - 0.50 * ((ani_tra_tm - ani_tra_tm02) / ani_tra_tm01);
				ani_tra_posY_P2 = 0.50 * ((ani_tra_tm - ani_tra_tm02) / ani_tra_tm01);
			}
			else if (ani_tra_tm < ani_tra_tm04) {
				ani_tra_posY = 0.50 + 0.02 * sinf(((ani_tra_tm - ani_tra_tm03) / ani_tra_tm01) * 50);
				ani_tra_posZ = 0.47 -0.02 * sinf(((ani_tra_tm - ani_tra_tm03) / ani_tra_tm01) * 50);

			}
			else if (ani_tra_tm < ani_tra_tm05) {
				ani_tra_posY = 0.50 - 0.50 * ((ani_tra_tm - ani_tra_tm04) / ani_tra_tm01);
				ani_tra_posY_P2 = ani_tra_posY;
			}
			else {
				ani_tra = false;
				PlaySound(NULL, NULL, 0);
				thread th_01(play_music, 1);
				if (th_01.joinable()) {
					th_01.join();
				}
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f + ani_tra_posY_P2, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		plato_sucio_1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f + ani_tra_posY_P1, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		plato_sucio_2.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f + ani_tra_posY, 0.0f + ani_tra_posZ + y));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		estropajo_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		trastes_sucios.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lavadero.RenderModel();

		//puertas pequeñas
		if (mainWindow.get_btn_2()) {
			mainWindow.res_btn_2();
			ani_puerta_cnt01 = true;
			ani_puerta_cnt01_tm = 0.0f;
			thread th_04(play_music, 4);
			if (th_04.joinable()) {
				th_04.join();
			}
		}

		if (ani_puerta_cnt01) {
			ani_puerta_cnt01_tm += deltaTime / del_mod;
			if (ani_puerta_cnt01_tm < ani_puerta_cnt01_tm01) {
				if (ani_puerta_cnt01_tm01_std) {
					giro_puerta_cnt01 = - 90.0f * (ani_puerta_cnt01_tm / ani_puerta_cnt01_tm01);
				}
				else {
					giro_puerta_cnt01 = - 90.0f + 90.0f * (ani_puerta_cnt01_tm / ani_puerta_cnt01_tm01);
				}
			}
			else {
				ani_puerta_cnt01 = false;
				ani_puerta_cnt01_tm01_std = !ani_puerta_cnt01_tm01_std;
				PlaySound(NULL, NULL, 0);
				thread th_01(play_music, 1);
				if (th_01.joinable()) {
					th_01.join();
				}
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.75f, 0.71f, -4.94f));
		model = glm::rotate(model, giro_puerta_cnt01 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_puerta1.RenderModel();

		if (mainWindow.get_btn_3()) {
			mainWindow.res_btn_3();
			ani_puerta_cnt02 = true;
			ani_puerta_cnt02_tm = 0.0f;

			thread th_04(play_music, 4);
			if (th_04.joinable()) {
				th_04.join();
			}
		}

		if (ani_puerta_cnt02) {
			ani_puerta_cnt02_tm += deltaTime / del_mod;
			if (ani_puerta_cnt02_tm < ani_puerta_cnt02_tm01) {
				if (ani_puerta_cnt02_tm01_std) {
					giro_puerta_cnt02 = - 90.0f * (ani_puerta_cnt02_tm / ani_puerta_cnt02_tm01);
				}
				else {
					giro_puerta_cnt02 = - 90.0f + 90.0f * (ani_puerta_cnt02_tm / ani_puerta_cnt02_tm01);
				}
			}
			else {
				ani_puerta_cnt02 = false;
				ani_puerta_cnt02_tm01_std = !ani_puerta_cnt02_tm01_std;
				PlaySound(NULL, NULL, 0);
				thread th_01(play_music, 1);
				if (th_01.joinable()) {
					th_01.join();
				}
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.77f, 0.71f, -4.20f));
		model = glm::rotate(model, giro_puerta_cnt02 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_puerta2.RenderModel();

		if (mainWindow.get_btn_4()) {
			mainWindow.res_btn_4();
			ani_puerta_cnt03 = true;
			ani_puerta_cnt03_tm = 0.0f;

			thread th_04(play_music, 4);
			if (th_04.joinable()) {
				th_04.join();
			}
		}

		if (ani_puerta_cnt03) {
			ani_puerta_cnt03_tm += deltaTime / del_mod;
			if (ani_puerta_cnt03_tm < ani_puerta_cnt03_tm01) {
				if (ani_puerta_cnt03_tm01_std) {
					giro_puerta_cnt03 = 90.0f * (ani_puerta_cnt03_tm / ani_puerta_cnt03_tm01);
				}
				else {
					giro_puerta_cnt03 = 90.0f - 90.0f * (ani_puerta_cnt03_tm / ani_puerta_cnt03_tm01);
				}
			}
			else {
				ani_puerta_cnt03 = false;
				ani_puerta_cnt03_tm01_std = !ani_puerta_cnt03_tm01_std;
				PlaySound(NULL, NULL, 0);
				thread th_01(play_music, 1);
				if (th_01.joinable()) {
					th_01.join();
				}
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3( 0.77f, 0.70f, -3.43f));
		model = glm::rotate(model, -giro_puerta_cnt03 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_puerta3.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.79f, 0.69f, -2.08f));
		model = glm::rotate(model, giro_puerta_cnt03 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_puerta4.RenderModel();

		//puertas refri
		if (mainWindow.get_btn_5()) {
			mainWindow.res_btn_5();
			ani_puerta_ref01 = true;
			ani_puerta_ref01_tm = 0.0f;
		}

		if (ani_puerta_ref01) {
			ani_puerta_ref01_tm += deltaTime / del_mod;
			if (ani_puerta_ref01_tm < ani_puerta_ref01_tm01) {
				if (ani_puerta_ref01_tm01_std) {
					giro_puerta_ref01 = - 90.0f * (ani_puerta_ref01_tm / ani_puerta_ref01_tm01);
				}
				else {
					giro_puerta_ref01 = - 90.0f + 90.0f * (ani_puerta_ref01_tm / ani_puerta_ref01_tm01);
				}
			}
			else {
				ani_puerta_ref01 = false;
				ani_puerta_ref01_tm01_std = !ani_puerta_ref01_tm01_std;
			}
		}
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.06f, 1.21f, 3.90f));
		model = glm::rotate(model, giro_puerta_ref01 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		refri_puerta_1.RenderModel();

		if (mainWindow.get_btn_6()) {
			mainWindow.res_btn_6();
			ani_puerta_ref02 = true;
			ani_puerta_ref02_tm = 0.0f;
		}

		if (ani_puerta_ref02) {
			ani_puerta_ref02_tm += deltaTime / del_mod;
			if (ani_puerta_ref02_tm < ani_puerta_ref02_tm01) {
				if (ani_puerta_ref02_tm01_std) {
					giro_puerta_ref02 = - 90.0f * (ani_puerta_ref02_tm / ani_puerta_ref02_tm01);
				}
				else {
					giro_puerta_ref02 = - 90.0f + 90.0f * (ani_puerta_ref02_tm / ani_puerta_ref02_tm01);
				}
			}
			else {
				ani_puerta_ref02 = false;
				ani_puerta_ref02_tm01_std = !ani_puerta_ref02_tm01_std;
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.91f, 1.26f, 3.91f));
		model = glm::rotate(model, giro_puerta_ref02 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		refri_puerta_2.RenderModel();

		if (mainWindow.get_btn_7()) {
			mainWindow.res_btn_7();
			ani_puerta_ref03 = true;
			ani_puerta_ref03_tm = 0.0f;
		}

		if (ani_puerta_ref03) {
			ani_puerta_ref03_tm += deltaTime / del_mod;
			if (ani_puerta_ref03_tm < ani_puerta_ref03_tm01) {
				if (ani_puerta_ref03_tm01_std) {
					giro_puerta_ref03 = - 90.0f * (ani_puerta_ref03_tm / ani_puerta_ref03_tm01);
				}
				else {
					giro_puerta_ref03 = - 90.0f + 90.0f * (ani_puerta_ref03_tm / ani_puerta_ref03_tm01);
				}
			}
			else {
				ani_puerta_ref03 = false;
				ani_puerta_ref03_tm01_std = !ani_puerta_ref03_tm01_std;
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.67f, 1.22f, 3.90f));
		model = glm::rotate(model, giro_puerta_ref03 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		refri_puerta_3.RenderModel();

		//PUERTA DE METAL
		if (mainWindow.get_btn_8()) {
			mainWindow.res_btn_8();
			ani_puerta_mtl = true;
			ani_puerta_mtl_tm = 0.0f;
			thread th_05(play_music, 5);
			if (th_05.joinable()) {
				th_05.join();
			}
		}

		if (ani_puerta_mtl) {
			ani_puerta_mtl_tm += deltaTime / del_mod;
			if (ani_puerta_mtl_tm < ani_puerta_ref03_tm01) {
				if (ani_puerta_mtl_tm01_std) {
					giro_puerta_mtl = -90.0f * (ani_puerta_mtl_tm / ani_puerta_mtl_tm01);
				}
				else {
					giro_puerta_mtl = -90.0f + 90.0f * (ani_puerta_mtl_tm / ani_puerta_mtl_tm01);
				}
			}
			else {
				ani_puerta_mtl = false;
				ani_puerta_mtl_tm01_std = !ani_puerta_mtl_tm01_std;
				PlaySound(NULL, NULL, 0);
				thread th_01(play_music, 1);
				if (th_01.joinable()) {
					th_01.join();
				}
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.87f, 1.37f, 0.95f));
		model = glm::rotate(model, giro_puerta_mtl * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		puerta_metal.RenderModel();

		//Extractores
		ani_extr += deltaTime * del_mod;

		if (ani_extr > 1000.0) {
			ani_extr = 0.0f;
		}


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.98f, 2.49f, -7.50f));
		model = glm::rotate(model, ani_extr * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		extractor_1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.00f, 2.49f, -7.50f));
		model = glm::rotate(model, ani_extr * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		extractor_2.RenderModel();

		//----------------------------------------------------------------------------------
		
		
		
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_bolsa_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_bolsa_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		papas_bolsa_3.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_aceite.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_freidora1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_centro_freidora2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_ext_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_ext_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesa_ext_3.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_3.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_4.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_5.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_6.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_7.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_8.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_9.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_10.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_11.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		silla_ext_12.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		estante_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		estante_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		refri_1.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		refri_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		refri_3.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		estropajo_2.RenderModel();

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		chef.RenderModel();

		/**/

		glUseProgram(0);
		mainWindow.swapBuffers();

		if (musica_fondo) {
			musica_fondo = false;
			thread th_01(play_music, 1);
			if (th_01.joinable()) {
				th_01.join();
			}
		}

	}

	return 0;
}
