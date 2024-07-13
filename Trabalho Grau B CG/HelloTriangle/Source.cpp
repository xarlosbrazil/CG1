﻿/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
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
#include <windows.h>  
#include <fstream>
#include <chrono>
#include <random>


using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//Classe que gerencia os shaders
#include "Shader.h"

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB_IMAGE
#include <stb_image.h>

#include "Sprite.h"

#include "Timer.h"

#include <vector>
std::vector<Sprite> moedas;

enum directions {NONE = -1, LEFT, RIGHT, UP, DOWN};

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight);
bool CheckCollision(Sprite &one, Sprite &two);
int randomInt(int min, int max);
bool isCollidingWithLava(glm::vec2 playerPosition);
bool isCollidingWithGrass(glm::vec2 playerPosition);
std::pair<int, int> tileAtual(glm::vec2 playerPosition);
// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 600;

//Variáveis globais

int dir = NONE;

//Variáveis para armazenar as infos do tileset
GLuint tilesetTexID;
glm::vec2 offsetTex; //armazena o deslocamento necessário das coordenadas de textura no tileset
GLuint VAOTile;
int nTiles;
glm::vec2 tileSize;

//Variáveis para armazenar as infos do tilemap
glm::vec2 tilemapSize;
const int MAX_COLUNAS = 15;
const int MAX_LINHAS = 15;
int tilemap[MAX_LINHAS][MAX_COLUNAS]; //este é o mapa de índices para os tiles do tileset

//Função para fazer a leitura do tilemap do arquivo
void loadMap(string fileName);
GLuint setupTile();
void drawMap(Shader &shader);

//Variáveis para controle de um personagem no mapa
glm::vec2 iPos; //índice do personagem no mapa

glm::vec2 posIni; //posição inicial de desenho do mapa

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

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

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);




	// Compilando e buildando o programa de shader

	Shader shader("HelloTriangle.vs","HelloTriangle.fs");
	Shader shaderDebug("HelloTriangle.vs","HelloTriangleDebug.fs");
	
	int imgWidth, imgHeight;
	GLuint texID = loadTexture("walk1.png", imgWidth, imgHeight);
	
	//Criação de um objeto Sprite
	Sprite player;
	player.inicializar(texID, 1, 6, glm::vec3(400.0,150.0,0.0), glm::vec3(imgWidth*0.8,imgHeight*0.8,1.0));
	player.setShader(&shader);
	player.setShaderDebug(&shaderDebug);


	//Leitura do tilemap
	loadMap("map2.txt");
	VAOTile = setupTile();

	if (tileSize.x == 0 || tileSize.y == 0) {
		std::cerr << "Erro: tileSize não foi inicializado corretamente." << std::endl;
		return -1;
	}

	//Habilita o shader que sera usado (glUseProgram)
	shader.Use();

	// Exercício 2 da Lista 2
	glm::mat4 projection = glm::ortho(0.0, 800.0, 600.0, 0.0, -1.0, 1.0);
	//Enviando para o shader via variável do tipo uniform (glUniform....)
	shader.setMat4("projection",glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	shader.setInt("texBuffer", 0);

	shaderDebug.Use();
	shaderDebug.setMat4("projection",glm::value_ptr(projection));

	posIni.x = tileSize.x/2;
	posIni.y = tileSize.y/2;

	iPos.x = 1;
	iPos.y = 1;

	int imgWidth2, imgHeight2;
	GLuint texID2 = loadTexture("icon32.png", imgWidth2, imgHeight2);

	int nMoedas = 5; // Número de moedas
	for (int i = 0; i < nMoedas; ++i) {

		float randomX = posIni.x + randomInt(1, MAX_COLUNAS - 2) * 38;
		float randomY = posIni.y + randomInt(1, MAX_LINHAS - 2) * 38;

		cout << "(X,Y):" << randomX << " " << randomY << endl;

		Sprite moeda;
		moeda.inicializar(texID2, 1, 1, glm::vec3(randomX, randomY, 0.0), glm::vec3(imgWidth2 * 1.2, imgHeight2 * 1.2, 1.0), 0.0, glm::vec3(1.0, 1.0, 0.0));
		moeda.setShader(&shader);
		moeda.setShaderDebug(&shaderDebug);
		moedas.push_back(moeda);

		cout << "Moeda adicionada ao vetor. Total de moedas: " << moedas.size() << endl;

	}
	
	int imgWidth3, imgHeight3;
	GLuint texID3 = loadTexture("Icon10.png", imgWidth2, imgHeight2);
	Sprite bau;
	bau.inicializar(texID, 1, 1, glm::vec3(400.0, 150.0, 0.0), glm::vec3(imgWidth * 2, imgHeight * 2, 1.0));
	bau.setShader(&shader);
	bau.setShaderDebug(&shaderDebug);

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{

		if (isCollidingWithLava(player.getPosicao()))
		{
			glfwSetWindowShouldClose(window, true);
		}

		if (isCollidingWithGrass(player.getPosicao()))
		{
			int i, j;
			std::tie(i, j) = tileAtual(player.getPosicao());
			tilemap[j][i] = 0;
			// cout << tilemap[i][j];
		}

		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawMap(shader);

		float x = posIni.x + iPos.x * tileSize.x;
		float y = posIni.y + iPos.y * tileSize.y;

		for (auto it = moedas.begin(); it != moedas.end(); ++it) {
			if (CheckCollision(player, *it)) {

				it->desativar();

				int i, j;
				std::tie(i, j) = tileAtual(player.getPosicao());
				tilemap[j][i] = 6;
				tilemap[j + 1][i] = 6;
				tilemap[j][i + 1] = 6;
				tilemap[j - 1][i] = 6;
				tilemap[j][i - 1] = 6;

			}

		}

		// cout << "TileSize: " << tileSize.x << tileSize.y << endl;
		player.setPosicao(glm::vec3(x, y, 0.0)); //passa a posição baseada no indice da matriz
		player.desenhar();

		for (auto& moeda : moedas)
		{
			if (moeda.ativo == true)
			{
				moeda.desenhar();
			}
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Funçãoo de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS && iPos.x != 1)
	{
		dir = LEFT;
		iPos.x -= 1;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS && iPos.x != 13)
	{
		dir = RIGHT;
		iPos.x += 1;
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS && iPos.y != 1)
	{
		dir = LEFT;
		iPos.y -= 1;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS && iPos.y != 13)
	{
		dir = RIGHT;
		iPos.y += 1;
	}

	if (action == GLFW_RELEASE)
	{
		dir = NONE;
	}


}

GLuint loadTexture(string filePath, int &imgWidth, int &imgHeight)
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
	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
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

		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);

		std::cout << "Loaded texture: " << filePath << " with ID: " << texID << std::endl;

	}
	else
	{
    	 std::cout << "Failed to load texture" << std::endl;
	}
	return texID;
}

//Função para fazer a leitura do tilemap do arquivo
void loadMap(string fileName)
{
	ifstream arqEntrada;
	arqEntrada.open(fileName); //abertura do arquivo
	if (arqEntrada)
	{
		///leitura dos dados
		string textureName;
		int width, height;
		//Leitura das informações sobre o tileset
		arqEntrada >> textureName >> nTiles >> tileSize.y >> tileSize.x;
		tilesetTexID = loadTexture(textureName, width, height);
		//Só pra debug, printar os dados
		cout << textureName << " " << nTiles << " " << tileSize.y << " " << tileSize.x << endl;
		//Leitura das informações sobre o mapa (tilemap)
		arqEntrada >> tilemapSize.y >> tilemapSize.x; //nro de linhas e de colunas do mapa
		cout << tilemapSize.y << " " << tilemapSize.x << endl;
		for (int i = 0; i < tilemapSize.y; i++) //percorrendo as linhas do mapa
		{
			for (int j = 0; j < tilemapSize.x; j++) //percorrendo as colunas do mapa
			{
				arqEntrada >> tilemap[i][j];
				cout << tilemap[i][j] << " ";
			}
			cout << endl;
		}
	
	}
	else
	{
		cout << "Houve um problema na leitura de " << fileName << endl;
	}
}

GLuint setupTile()
{
	GLuint VAO;
	
	offsetTex.s = 1.0/ (float) nTiles;
	offsetTex.t = 1.0;
	glm::vec3 cor; //temporario, dá pra tirar 
	cor.r = 1.0;
	cor.g = 0.0;
	cor.b = 1.0;
    //Especificação da geometria da sprite (quadrado, 2 triangulos)
    GLfloat vertices[] = {
		//x   y    z    r      g      b      s    t
		-0.5, 0.5, 0.0, cor.r, cor.g, cor.b, 0.0, offsetTex.t, //v0
        -0.5,-0.5, 0.0, cor.r, cor.g, cor.b, 0.0, 0.0, //v1
         0.5, 0.5, 0.0, cor.r, cor.g, cor.b, offsetTex.s, offsetTex.t, //v2
        -0.5,-0.5, 0.0, cor.r, cor.g, cor.b, 0.0, 0.0, //v1
         0.5,-0.5, 0.0, cor.r, cor.g, cor.b, offsetTex.s, 0.0, //v3
         0.5, 0.5, 0.0, cor.r, cor.g, cor.b, offsetTex.s, offsetTex.t  //v2
	};

	GLuint VBO;
	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo layout 0 - Posição - 3 valores dos 8 que descrevem o vértice
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo layout 1 - Cor - 3 valores dos 8 que descrevem o vértice
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo layout 2 - Coordenada de textura - 2 valores dos 8 que descrevem o vértice
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6* sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void drawMap(Shader &shader)
{
	shader.Use();

	glBindTexture(GL_TEXTURE_2D, tilesetTexID); //Conectando com a textura
   	glBindVertexArray(VAOTile); //Conectando ao buffer de geometria

	for (int i=0; i < tilemapSize.y; i++)
	{
		for (int j=0; j < tilemapSize.x; j++)
		{
			//Atualiza a matriz de transformação do tile i j
			glm::mat4 model = glm::mat4(1);
			model = glm::translate(model,glm::vec3(posIni.x + j * tileSize.x, posIni.y + i * tileSize.y,0.0));
			model = glm::scale(model, glm::vec3(tileSize.x,tileSize.y,1.0));
			shader.setMat4("model", glm::value_ptr(model));

			int indiceTile = tilemap[i][j];
			shader.setVec2("offsetTex",indiceTile * offsetTex.x, offsetTex.y);
			
			 
			//Chamada de desenho para o tile
			glDrawArrays(GL_TRIANGLES, 0, 6);
			
		}

	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
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

// Função para gerar número aleatório entre min e max
int randomInt(int min, int max)
{
	static std::mt19937 mt(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(mt);
}

bool isCollidingWithLava(glm::vec2 playerPosition)
{
	// Calcula os índices na matriz do tilemap
	int tileX = static_cast<int>(playerPosition.x / tileSize.x);
	int tileY = static_cast<int>(playerPosition.y / tileSize.y);

	// Verifica se o jogador está em um tile de lava
	return tilemap[tileY][tileX] == 3;
}

bool isCollidingWithGrass(glm::vec2 playerPosition)
{
	// Calcula os índices na matriz do tilemap
	int tileX = static_cast<int>(playerPosition.x / tileSize.x);
	int tileY = static_cast<int>(playerPosition.y / tileSize.y);

	// Verifica se o jogador está em um tile de lava
	return tilemap[tileY][tileX] == 1;
}

std::pair<int, int> tileAtual(glm::vec2 playerPosition)

{
	// Calcula os índices na matriz do tilemap
	int tileX = static_cast<int>(playerPosition.x / tileSize.x);
	int tileY = static_cast<int>(playerPosition.y / tileSize.y);

	return std::make_pair(tileX, tileY);
}

