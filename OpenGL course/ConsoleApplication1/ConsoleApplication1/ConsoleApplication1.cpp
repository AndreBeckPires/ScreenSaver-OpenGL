// ConsoleApplication1.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include <iostream>
#include<stdio.h>
#include <string>
#include <stdlib.h>


using namespace std;

//window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

bool direction = true; 
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;
float curentAngle = 0.0f;
//vertex shader
// Vertex Shader code
static const char* vShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 pos;											  \n\
                                                                              \n\
uniform mat4 model;                                                           \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = model * vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);		  \n\
}";

// Fragment Shader
static const char* fShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
out vec4 colour;                                                               \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    colour = vec4(1.0, 0.0, 0.0, 1.0);                                         \n\
}";


void CreateTriangle() {
	GLfloat vertices[] = {
		-1.0f, -1.0f,0.0f,
		1.0, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, 0);
		glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //unbind do VBO
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);
	
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLenght[1];
	codeLenght[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLenght);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		cout << "Erro compilando o shader" << shaderType << eLog << endl;
		return;
	}

	glAttachShader(theProgram, theShader);
}
void CompileShaders() {
	shader = glCreateProgram();

	if (!shader)
	{
		cout << "Erro criando o shader program" << endl;
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader); //"criar na placa
	//teste
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Erro linkando o programa" << eLog << endl;
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Erro validando o programa" << eLog << endl;
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");//assinala a variaval ao xmove
}
int main()
{
	//initialize GLFW (pra fazer window)
	if (!glfwInit())
	{
		cout << "GLFW initialisation failed!";
		glfwTerminate();
		return 1;
	}
     //setup glfw window properties
	//OpenGLversion
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//core profile = no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Teste window", NULL, NULL);
	if (!mainWindow)
	{
		cout << "GLFW Window creation failed";
		glfwTerminate();
		return 1;
	}

	//Get bufffer size information(parte da tela q vai lidar com os dados de opengl a serem mostrados)
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//set context for glew to use
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;


	if (glewInit() != GLEW_OK)
	{
		cout << "glew init failed";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;

	}


	//Setup viewport size
	glViewport(0,0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();


	//loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// get and handle user input events
		glfwPollEvents();
		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}
		if (abs(triOffset) >= triMaxoffset)
		{
			direction = !direction;
		}
		curentAngle += 0.1f;
		if (curentAngle >= 360)
		{
			curentAngle -= 360;
		}


		//clear window
		glClearColor(0.0f,0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);
		//glm::mat4 model;
		model = glm::rotate(model, curentAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(triOffset, triOffset, 0.0f));//x,y,z
		
		

		//glUniform1f(uniformModel, triOffset);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES,0,3);
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}


    std::cout << "Hello World!\n";
	return 0;
}

