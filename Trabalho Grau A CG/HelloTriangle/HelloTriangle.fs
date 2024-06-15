#version 330 core

out vec4 color;

in vec2 texCoord;

uniform sampler2D texBuffer;

void main()
{
    color = texture(texBuffer, texCoord); //vec4(vertexColor, 1.0f);
}