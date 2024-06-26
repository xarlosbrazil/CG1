// C�digo fonte do Vertex Shader (em GLSL)
#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertexColor;
uniform mat4 projection; //matriz de proje��o
uniform mat4 model;

void main()
{
	//...pode ter mais linhas de c�digo aqui!
	gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
	vertexColor = color;
}