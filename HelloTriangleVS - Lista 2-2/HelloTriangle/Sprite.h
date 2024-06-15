#pragma once

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Sprite
{
    public:
        Sprite();// Construtor padrão
        ~Sprite(); //Método destrutor

        void inicializar(glm::vec3 pos, glm::vec3 escala, float angulo, glm::vec3 cor);
        void atualizar();
        void desenhar();
        void finalizar();

        inline void setShader(Shader *shader) { this->shader = shader; }
        inline void setAngulo(float angulo) { this->angulo = angulo; }

    protected:

        GLuint VAO; //Identificador do buffer de geometria VAO
        GLuint texID; //Identificador da textura

        //Transformações na geometria
        glm::vec3 pos, escala;
        float angulo;

        //Animação por sprite
        int nAnimations, nFrames, iAnimation, iFrame;
        glm::vec2 offsetTex; //ds e dt 

        Shader *shader;
};