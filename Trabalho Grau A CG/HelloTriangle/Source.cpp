/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 14/08/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//Classe que gerencia os shaders
#include "Shader.h"

#include "Sprite.h"
#include "Timer.h"

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB_IMAGE
#include "stb_image.h"

enum directions {LEFT, RIGHT, UP, DOWN, NONE};


// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight);
bool CheckCollision(Sprite& one, Sprite& two);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 600;

int dir = NONE;

// Fun��o MAIN
int main()
{

	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! -- Rossana", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// PROFUNDIDADE
	glClear(GL_COLOR_BUFFER_BIT);
	GL_DEPTH_BUFFER_BIT;

	// Compilando e buildando o programa de shader	

	Shader shader("HelloTriangle.vs", "HelloTriangle.fs");

	//Habilita o shader que sera usado (glUseProgram)
	shader.Use();

	int imgWidthMain, imgHeightMain;
	GLuint texID = loadTexture("../../Textures/1bit/Main_Characters/Char_Boy.png", imgWidthMain, imgHeightMain);

	// Criação do objeto dos sprites
	Sprite player, background, enemyEsq, enemyDir;
	player.inicializar(texID, 6, 8, glm::vec3(200.0, 200.0, 0.0), glm::vec3(-imgWidthMain*2.0, imgHeightMain*2.0, 1.0));
	player.setShader(&shader);

	GLuint texID2 = loadTexture("../../Textures/backgrounds/fantasy-set/PNG/Battleground2/Bright/Battleground2.png", imgWidthMain, imgHeightMain);

	background.inicializar(texID2, 1, 1, glm::vec3(400.0, 300.0, 0.0), glm::vec3(imgWidthMain/1.8, imgHeightMain/1.8, 1.0));
	background.setShader(&shader);

	GLuint texID3 = loadTexture("../../Textures/1bit/Enemies/alien6.png", imgWidthMain, imgHeightMain);

	enemyEsq.inicializar(texID3, 5, 8, glm::vec3(-100.0, 170.0, 0.0), glm::vec3(imgWidthMain * 2.0, imgHeightMain * 2.0, 1.0));
	enemyEsq.setShader(&shader);

	GLuint texID4 = loadTexture("../../Textures/1bit/Enemies/alien1.png", imgWidthMain, imgHeightMain);

	enemyDir.inicializar(texID4, 5, 8, glm::vec3(900.0, 210.0, 0.0), glm::vec3(-imgWidthMain * 2.0, imgHeightMain * 2.0, 1.0));
	enemyDir.setShader(&shader);

	glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	shader.setMat4("projection", glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	shader.setInt("texBuffer", 0);

	Timer timer;

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{

		timer.start();

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		if (dir == UP)
		{
			player.moveUp();
		}

		if (dir == DOWN)
		{
			player.moveDown();
		}

		if (dir == LEFT)
		{
			player.moveLeft();
		}

		if (dir == RIGHT)
		{
			player.moveRight();
		}

		if (CheckCollision(player, enemyEsq) == true)
		{
			player.ativo = false;
		}

		if (CheckCollision(player, enemyDir) == true)
		{
			player.ativo = false;
		}

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		background.desenhar();

		if (player.ativo)
		{
			player.setAnimationLine(2);
			player.desenhar();
		}

		enemyDir.setAnimationLine(2);
		enemyDir.movimentoEnemyDir();
		enemyDir.desenhar();
		

		enemyEsq.setAnimationLine(2);
		enemyEsq.movimentoEnemyEsq();
		enemyEsq.desenhar();
		


		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		dir = UP;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		dir = DOWN;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		dir = LEFT;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		dir = RIGHT;
	}

	if (action == GLFW_RELEASE)
	{
		dir = NONE;
	}



}

// Prototipo das funções
GLuint loadTexture(string filePath, int& imgWidth, int& imgHeight)
{
	GLuint texID;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height,&nrChannels, 0);

	if (data)
	{
		std::cout << "Texture loaded successfully: " << filePath << std::endl;
		std::cout << "Texture width: " << width << ", height: " << height << ", channels: " << nrChannels << std::endl;

		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		imgWidth = width;
		imgHeight = height;
	}

	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	return texID;
}

bool CheckCollision(Sprite& one, Sprite& two)
{
	// collision x-axis?
	bool collisionX = one.getPMax().x >= two.getPMin().x &&
		two.getPMax().x >= one.getPMin().x;
	// collision y-axis?
	bool collisionY = one.getPMax().y >= two.getPMin().y &&
		two.getPMax().y >= one.getPMin().y;
	// collision only if on both axes
	return collisionX && collisionY;
}