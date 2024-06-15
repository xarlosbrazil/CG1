#pragma once

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Sprite
{
public:

    bool ativo;

    Sprite() {} //Método construtor padrão
    ~Sprite(); //Método destrutor

    // Adiciona um método para definir a linha de animação
    void setAnimationLine(int line) {
        if (line >= 0 && line < nAnimations) {
            iAnimation = line;
        }
    }

    void inicializar(GLuint texID, int nAnimations = 1, int nFrames = 1, glm::vec3 pos = glm::vec3(0), glm::vec3 escala = glm::vec3(1), float angulo = 0.0, glm::vec3 cor = glm::vec3(1.0, 1.0, 0.0));
    void atualizar();
    void desenhar();
    void finalizar();

    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void movimentoEnemyDir();
    void movimentoEnemyEsq();

    inline void desativar() { ativo = false; }
    inline void setShader(Shader* shader) { this->shader = shader; }
    inline void setAngulo(float angulo) { this->angulo = angulo; }

    inline glm::vec3 getPMin() { return this->pmin;}
    inline glm::vec3 getPMax() { return this->pmax;}
    
    void setColisao() { colidiu = true; }


protected:

    void getAABB();

    GLuint VAO; //Identificador do buffer de geometria VAO
    GLuint texID;

    //Transformações na geometria
    glm::vec3 pos, escala;
    float angulo;
    float vel;

    //Animação

    int nAnimations, nFrames, iAnimation, iFrame;
    glm::vec2 offsetTex; //Armazenar ds e dt

    Shader* shader;

    glm::vec3 pmin, pmax;
    bool colidiu;
};