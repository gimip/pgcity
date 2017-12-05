#version 330 core
out vec4 FragColor;

in vec3 colour;
uniform sampler2D shadowMap;

uniform vec3 objectColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;

void main()
{           

    FragColor = vec4(colour, 1.0f);
}