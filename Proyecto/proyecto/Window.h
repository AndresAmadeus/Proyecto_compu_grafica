#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();

	//animaciones
	bool get_btn_1() { return btn_1; }
	void res_btn_1() { btn_1 = false; }
	bool get_btn_2() { return btn_2; }
	void res_btn_2() { btn_2 = false; }
	bool get_btn_3() { return btn_3; }
	void res_btn_3() { btn_3 = false; }
	bool get_btn_4() { return btn_4; }
	void res_btn_4() { btn_4 = false; }
	bool get_btn_5() { return btn_5; }
	void res_btn_5() { btn_5 = false; }
	bool get_btn_6() { return btn_6; }
	void res_btn_6() { btn_6 = false; }
	bool get_btn_7() { return btn_7; }
	void res_btn_7() { btn_7 = false; }
	bool get_btn_8() { return btn_8; }
	void res_btn_8() { btn_8 = false; }
	bool get_btn_p() { return btn_p; }
	void res_btn_p() { btn_p = false; }
	bool get_btn_o() { return btn_o; }
	void res_btn_o() { btn_o = false; }
	bool get_btn_l() { return btn_l; }
	void res_btn_l() { btn_l = false; }
	bool get_btn_i() { return btn_i; }
	void res_btn_i() { btn_i = false; }

	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevez() { return muevez; }
	GLfloat getmuevey() { return muevey; }


	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;

	//animaciones
	bool btn_1 = false;
	bool btn_2 = false;
	bool btn_3 = false;
	bool btn_4 = false;
	bool btn_5 = false;
	bool btn_6 = false;
	bool btn_7 = false;
	bool btn_8 = false;
	bool btn_p = false;
	bool btn_o = false;
	bool btn_l = false;
	bool btn_i = false;

	GLfloat muevex;
	GLfloat muevez;
	GLfloat muevey;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

