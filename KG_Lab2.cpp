#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders.h"
#include "Camera.h"
#include <soil.h>

using glm::mat4;
using glm::vec3;
using glm::vec4;
using glm::radians;
using glm::rotate;
using glm::lookAt;
using glm::perspective;
using glm::translate;
using glm::scale;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const char brickTexPath[] = "D:\\Studies\\Graphics\\3DFigures\\bricktexture.jpg";

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 850;

const mat4 IDENTITY_MATRIX(1.0f);

const vec4 BACKGROUND_COLOR(0.24f, 0.28f, 0.29f, 1.0f);

const vec3 PYRAMID_POS(1.0f, -1.0f, -0.1f); 
const vec4 PYRAMID_COLOR(0.98f, 0.84f, 0.65f, 1.0f);

const vec3 PRISM_POS(-0.5f, 0.1f, 1.0f);
const vec4 PRISM_COLOR(0.28f, 0.09f, 0.47f, 1.0f);

const vec3 LIGHT_POS1(-0.1f, 1.0f, 0.3f);
const vec4 LIGHT_COLOR1(1.0f, 1.0f, 1.0f, 1.0f);

const vec3 LIGHT_POS2(1.0f, 1.2f, -0.1f);
const vec4 LIGHT_COLOR2(1.0f, 0.0f, 0.0f, 0.1);

#define U(x) (x) 
#define V(x) (1.0f - x)

// пирамида с квадратом в основании
const GLfloat pyramidVertices[] =  {
//   пространственные коор-ты	 текстурные коор-ты				нормали         
		-0.5f, 0.0f,  0.5f,			U(0.0f), V(0.0f),		 0.0f, -1.0f, 0.0f,    // 0 - Нижняя грань
		-0.5f, 0.0f, -0.5f,			U(0.0f), V(1.0f),		 0.0f, -1.0f, 0.0f,    // 1 - Нижняя грань
		 0.5f, 0.0f, -0.5f,			U(1.0f), V(1.0f),		 0.0f, -1.0f, 0.0f,    // 2 - Нижняя грань
		 0.5f, 0.0f,  0.5f,			U(1.0f), V(0.0f),		 0.0f, -1.0f, 0.0f,    // 3 - Нижняя грань
											
		-0.5f, 0.0f,  0.5f,			U(0.0f), V(0.0f),		-0.8f, 0.5f,  0.0f,    // 4 - Левая сторона
		-0.5f, 0.0f, -0.5f,			U(1.0f), V(0.0f),		-0.8f, 0.5f,  0.0f,    // 5 - Левая сторона
		 0.0f, 0.8f,  0.0f,			U(0.5f), V(1.0f),		-0.8f, 0.5f,  0.0f,    // 6 - Левая сторона
											
		-0.5f, 0.0f, -0.5f,			U(1.0f), V(0.0f),		 0.0f, 0.5f, -0.8f,    // 7 - Задняя сторона
		 0.5f, 0.0f, -0.5f,			U(0.0f), V(0.0f),		 0.0f, 0.5f, -0.8f,    // 8 - Задняя сторона
		 0.0f, 0.8f,  0.0f,			U(0.5f), V(1.0f),		 0.0f, 0.5f, -0.8f,    // 9 - Задняя сторона
															 
		 0.5f, 0.0f, -0.5f,			U(0.0f), V(0.0f),		 0.8f, 0.5f,  0.0f,    // 10 - Правая сторона
		 0.5f, 0.0f,  0.5f,			U(1.0f), V(0.0f),		 0.8f, 0.5f,  0.0f,    // 11 - Правая сторона
		 0.0f, 0.8f,  0.0f,			U(0.5f), V(1.0f),		 0.8f, 0.5f,  0.0f,    // 12 - Правая сторона
															 
		 0.5f, 0.0f,  0.5f,			U(1.0f), V(0.0f),		 0.0f, 0.5f,  0.8f,    // 13 - Лицевая сторона
		-0.5f, 0.0f,  0.5f,			U(0.0f), V(0.0f),		 0.0f, 0.5f,  0.8f,    // 14 - Лицевая сторона
		 0.0f, 0.8f,  0.0f,			U(0.5f), V(1.0f),		 0.0f, 0.5f,  0.8f     // 15 - Лицевая сторона
};

// индексы вершин для отрисовки пирамиды треугольниками 
const GLuint pyramidIndices[] = {
	0, 1, 2,	// Нижняя сторона
	0, 2, 3,	// Нижняя сторона
	4, 6, 5,	// Левая сторона
	7, 9, 8,	// Задняя сторона
	10, 12, 11, // Правая сторона
	13, 15, 14	// Лицевая сторона
};

// призма с шестиугольником в основании
const GLfloat prismVertices[] = {
//			координаты вершин					нормали
//			нижнее основание					
		 0.35f,  0.0f,  0.0f,				 0.0f, -1.0f,  0.0f,	// 0
		 0.15f,  0.0f, -0.3f,				 0.0f, -1.0f,  0.0f,	// 1
		-0.15f,  0.0f, -0.3f,				 0.0f, -1.0f,  0.0f,	// 2
		-0.35f,  0.0f,  0.0f,				 0.0f, -1.0f,  0.0f,	// 3
		-0.15f,  0.0f,  0.3f,				 0.0f, -1.0f,  0.0f,	// 4
		 0.15f,  0.0f,  0.3f,				 0.0f, -1.0f,  0.0f,	// 5
											 			   
//			верхнее основание				 			   
		 0.35f, 0.8f,  0.0f,				 0.0f,  1.0f,  0.0f,	// 6
		 0.15f, 0.8f, -0.3f,				 0.0f,  1.0f,  0.0f,	// 7
		-0.15f, 0.8f, -0.3f,				 0.0f,  1.0f,  0.0f,	// 8
		-0.35f, 0.8f,  0.0f,				 0.0f,  1.0f,  0.0f,	// 9
		-0.15f, 0.8f,  0.3f,				 0.0f,  1.0f,  0.0f,	// 10
		 0.15f, 0.8f,  0.3f,				 0.0f,  1.0f,  0.0f,	// 11
											 	    	   
//			боковая грань 1					 	    	   
		-0.15f,  0.0f,  0.3f,				 0.0f,  0.0f,  1.0f,	// 12	(4)
		-0.15f,  0.8f,  0.3f,				 0.0f,  0.0f,  1.0f,	// 13	(10)
		 0.15f,  0.8f,  0.3f,				 0.0f,  0.0f,  1.0f,	// 14	(11)
		 0.15f,  0.0f,  0.3f,				 0.0f,  0.0f,  1.0f,	// 15	(5) 
											 	    	   
//			боковая грань 2					 	    	   
		 0.15f,  0.0f,  0.3f,				 0.3f,  0.0f,  0.2f,	// 16	(5)
		 0.15f,  0.8f,  0.3f,				 0.3f,  0.0f,  0.2f,	// 17	(11)
		 0.35f,  0.8f,  0.0f,				 0.3f,  0.0f,  0.2f,	// 18	(6)
		 0.35f,  0.0f,  0.0f,				 0.3f,  0.0f,  0.2f,	// 19	(0)
											 	    
//			боковая грань 3					 	    
		 0.35f,  0.0f,  0.0f,				 0.3f,  0.0f, -0.2f,	// 20	(0)
		 0.35f,  0.8f,  0.0f,				 0.3f,  0.0f, -0.2f,	// 21	(6)
		 0.15f,  0.8f, -0.3f,				 0.3f,  0.0f, -0.2f,	// 22	(7)
		 0.15f,  0.0f, -0.3f,				 0.3f,  0.0f, -0.2f,	// 23	(1)
											 	    
//			боковая грань 4					 	    
		 0.15f,  0.0f, -0.3f,				 0.0f,  0.0f, -1.0f,	// 24	(1)
		 0.15f,  0.8f, -0.3f,				 0.0f,  0.0f, -1.0f,	// 25	(7)
		-0.15f,  0.8f, -0.3f,				 0.0f,  0.0f, -1.0f,	// 26	(8)
		-0.15f,  0.0f, -0.3f,				 0.0f,  0.0f, -1.0f,	// 27	(2)
												    
//			боковая грань 5						    
		-0.15f,  0.0f, -0.3f,				-0.3f,  0.0f, -0.2f,	// 28	(2)
		-0.15f,  0.8f, -0.3f,				-0.3f,  0.0f, -0.2f,	// 29	(8)
		-0.35f,  0.8f,  0.0f,				-0.3f,  0.0f, -0.2f,	// 30	(9)
		-0.35f,  0.0f,  0.0f,				-0.3f,  0.0f, -0.2f,	// 31	(3)

//			боковая грань 6	
		-0.35f,  0.0f,  0.0f,				-0.3f,  0.0f,  0.2f,	// 32	(3)
		-0.35f,  0.8f,  0.0f,				-0.3f,  0.0f,  0.2f,	// 33	(9)
		-0.15f,  0.8f,  0.3f,				-0.3f,  0.0f,  0.2f,	// 34	(10)
		-0.15f,  0.0f,  0.3f,				-0.3f,  0.0f,  0.2f,	// 35	(4) 
};

// индексы вершин для отрисовки пирамиды треугольниками
const GLuint prismIndices[] = {
//	нижняя грань
	0, 3, 1,
	1, 3, 2,
	3, 0, 4,
	4, 0, 5,

//	верхняя грань
	6, 7, 9,
	9, 7, 8, 
	9, 10, 6,
	6, 10, 11,

// боковая грань 1
	12, 15, 14,		// 4, 5, 11,
	12, 14, 13,		// 4, 11, 10,

// боковая грань 2
	16, 19, 18,		// 5, 0, 6,
	16, 18, 17,		// 5, 6, 11,

// боковая грань 3
	20, 23, 22,		// 0, 1, 7, 
	20, 22, 21,		// 0, 7, 6,

// боковая грань 4
	24, 27, 26,		// 1, 2, 8,
	24, 26, 25,		// 1, 8, 7,

// боковая грань 5
	28, 31, 30,		// 2, 3, 9,
	28, 30, 29,		// 2, 9, 8,

// боковая грань 6
	32, 35, 34,		// 3, 4, 10,
	32, 34, 33,		// 3, 10, 9,
};

// куб со стороной 0.2 и центром в (0,0,0) 
const GLfloat lightVertices[] =	{
//		x		y		z
	 -0.1f,	 -0.1f,	  -0.1f,	// 0
	 -0.1f,	 -0.1f,	   0.1f,	// 1

	  0.1f,	 -0.1f,	   0.1f,	// 2
	  0.1f,	 -0.1f,	  -0.1f,	// 3

	 -0.1f,	  0.1f,	  -0.1f,	// 4
	 -0.1f,	  0.1f,	   0.1f,	// 5

	  0.1f,	  0.1f,	   0.1f,	// 6
	  0.1f,	  0.1f,	  -0.1f		// 7
};

// индексы вершин для отрисовки куба треугольниками 
const GLuint lightIndices[] =	{

	1, 0, 2,	// нижняя
	2, 0, 3,	// грань

	1, 2, 5,	// передняя
	5, 2, 6,	// грань

	2, 3, 6,	// правая
	6, 3, 7,	// грань

	3, 0, 7,	// задняя
	7, 0, 4,	// грань

	5, 6, 4,	// верхняя
	4, 6, 7,	// грань

	0, 1, 5,	// левая
	5, 0, 4,	// грань
};

Camera camera(vec3(0.0f, 0.0f, 1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
	// Инициализируем GLFW и, если при этом возникла ошибка, формируем сообщение
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// Сообщаем GLFW, какую версию OpenGL мы используем
	// В данном случае используется OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Сообщаем GLFW, что мы хотим использовать CORE profile (с новыми функциями)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Проверяем, не возникло ли ошибок в процессе создания окна
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Scene", nullptr, 0);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	// Прежде чем инициализировать библиотеку GLew, необходимо присвоить переменной true
	glewExperimental = GL_TRUE;
	glewInit();

	// Назначаем функции обратного вызова
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // GLFW_CURSOR_NORMAL

	/***************************************************************************
	* Генерируем Vertex Array Object, Vertex Buffer Object и Element Buffer Object для пирамиды
	*/
	GLuint pyramidVBO, pyramidVAO, pyramidEBO;
	glGenVertexArrays(1, &pyramidVAO);
	glGenBuffers(1, &pyramidVBO);
	glGenBuffers(1, &pyramidEBO);

	// Привязываем VAO для подковы 
	glBindVertexArray(pyramidVAO);

	// Делаем VBO_hs текущим ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
	// Соотносим буфер и массив, хранящий информацию о вершинах
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

	// Делаем EBO_hs текущим ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
	// Соотносим буфер и массив, задающий порядок обхода вершин
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

	// Конфигурируем Vertex Attributes, чтобы OpenGL понимала, как читать буфер
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // аттрибут для пространственных координат
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // аттрибут для текстурных координат
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); // аттрибут для нормалей
	// Активируем Vertex Attributes, чтобы OpenGL могла их использовать
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Отвязываем настроенные VAO и VBO, чтобы случайно не изменить их 
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Отвязываем EBO (! Делаем это после отвязки VAO, чтобы они оставались связанными)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*****************************************************************************
	* Подключаем и настраиваем текстуру
	*/
	unsigned int brickTexture;

	glGenTextures(1, &brickTexture);
	glBindTexture(GL_TEXTURE_2D, brickTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = SOIL_load_image(brickTexPath, &width, &height, 0, SOIL_LOAD_RGB);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);


	/* ***********************************************************************
	* Генерируем Vertex Array Object, Vertex Buffer Object и Element Buffer Object для призмы
	*/

	GLuint prismVAO, prismVBO, prismEBO;
	glGenVertexArrays(1, &prismVAO);
	glGenBuffers(1, &prismVBO);
	glGenBuffers(1, &prismEBO);

	glBindVertexArray(prismVAO);
	glBindBuffer(GL_ARRAY_BUFFER, prismVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(prismVertices), prismVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prismEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(prismIndices), prismIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0); 
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/******************************************************************************
	* Генерируем Vertex Array Object, Vertex Buffer Object и Element Buffer Object для светильников
	*/

	GLuint lightVAO, lightVBO, lightEBO;
	// Создаем Vertex Array Object и привязываем его к состоянию
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// Создаем Vertex Buffer Object и связываем его с массивом вершин 
	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

	// Создаем Element Buffer Object и связываем его с массивом индексов
	glGenBuffers(1, &lightEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lightIndices), lightIndices, GL_STATIC_DRAW);

	// Настраиваем и активируем аттрибут для чтения координат
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Отвязываем VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Отвязываем VAO
	glBindVertexArray(0);
	// Отвязываем EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/*
	*****************************************************************************
	*/

	glEnable(GL_DEPTH_TEST);

	Shader pyramidShader("VertShader_pyramid.txt", "FragShader_pyramid.txt");
	Shader prismShader("VertShader_prism.txt", "FragShader_prism.txt");
	Shader lampShader("VertShader_lamp.txt", "FragShader_lamp.txt");

	// Основной цикл
	while (!glfwWindowShouldClose(window)) {
		
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// Выставляем цвет фона
		glClearColor(BACKGROUND_COLOR[0],
			BACKGROUND_COLOR[1],
			BACKGROUND_COLOR[2],
			BACKGROUND_COLOR[3]);
		// Очищаем буфер цвета и буфер глубины
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/******************************************************************************
		* Настройка MVP-матриц и рендеринг светильника 1
		*/
		mat4 lampModel1 = IDENTITY_MATRIX;
		lampModel1 = translate(lampModel1, LIGHT_POS1);
		lampModel1 = scale(lampModel1, vec3(0.5f));

		mat4 view = camera.GetViewMatrix();
		mat4 projection = perspective(radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);

		lampShader.use();
		lampShader.setMat4("model", lampModel1);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lampShader.setVec4("lightColor", LIGHT_COLOR1);

		glBindVertexArray(lightVAO);
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		/******************************************************************************
		* Настройка MVP-матриц и рендеринг светильника 2
		*/
		mat4 lampModel2 = IDENTITY_MATRIX;

		lampModel2 = translate(lampModel2, LIGHT_POS2);
		lampModel2 = scale(lampModel2, vec3(0.5f));

		lampShader.setMat4("model", lampModel2);
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		lampShader.setVec4("lightColor", LIGHT_COLOR2);

		glBindVertexArray(lightVAO);
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		/******************************************************************************
		* Настройка MVP-матриц и рендеринг пирамиды
		*/

		mat4 pyramidModel = IDENTITY_MATRIX;
		pyramidModel = translate(pyramidModel, PYRAMID_POS);
		pyramidModel = scale(pyramidModel, vec3(2.0));
		
		pyramidShader.use(); 
		pyramidShader.setMat4("model", pyramidModel);
		pyramidShader.setMat4("view", view);
		pyramidShader.setMat4("projection", projection);
		pyramidShader.setVec4("lightColor1", LIGHT_COLOR1);
		pyramidShader.setVec4("lightColor2", LIGHT_COLOR2);
		pyramidShader.setVec3("lightPos1", LIGHT_POS1);
		pyramidShader.setVec3("lightPos2", LIGHT_POS2);
		pyramidShader.setVec3("viewPos", camera.Position);

		glBindTexture(GL_TEXTURE_2D, brickTexture);
		glBindVertexArray(pyramidVAO);
		glDrawElements(GL_TRIANGLES, sizeof(pyramidIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		/******************************************************************************
		* Настройка MVP-матриц и рендеринг призмы
		*/

		prismShader.use();
		
		mat4 prismModel= IDENTITY_MATRIX;
		mat4 normalCorrection = IDENTITY_MATRIX;
		prismModel = translate(prismModel, PRISM_POS);
		prismModel = rotate(prismModel, -10.0f, vec3(0.0f, 0.0f, 1.0f));
		prismModel = rotate(prismModel, 5.0f, vec3(0.0f, 1.0f, 0.0f));
		normalCorrection = rotate(normalCorrection, -10.0f, vec3(0.0f, 0.0f, 1.0f));
		normalCorrection = rotate(normalCorrection, 5.0f, vec3(0.0f, 1.0f, 0.0f));
		pyramidModel = scale(pyramidModel, vec3(2.0));

		prismShader.setMat4("model", prismModel);
		prismShader.setMat4("view", view);
		prismShader.setMat4("projection", projection);
		prismShader.setMat4("normalCorrection", normalCorrection);
		prismShader.setVec4("objectColor", PRISM_COLOR);
		prismShader.setVec4("lightColor1", LIGHT_COLOR1);
		prismShader.setVec4("lightColor2", LIGHT_COLOR2);
		prismShader.setVec3("lightPos1", LIGHT_POS1);
		prismShader.setVec3("lightPos2", LIGHT_POS2);
		prismShader.setVec3("viewPos", camera.Position);

		glBindVertexArray(prismVAO);
		glDrawElements(GL_TRIANGLES, sizeof(prismIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &pyramidVAO);
	glDeleteBuffers(1, &pyramidVBO);
	glDeleteBuffers(1, &pyramidEBO);
	
	glDeleteVertexArrays(1, &prismVBO);
	glDeleteBuffers(1, &prismVAO);
	glDeleteBuffers(1, &prismEBO);

	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &lightVBO);
	glDeleteBuffers(1, &lightEBO);

	glDeleteTextures(1, &brickTexture);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
