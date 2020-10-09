#version 330 core
layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec3 inputColor;
layout (location = 2) in vec2 inputTextureCoords;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(inputPosition, 1.0);
	ourColor = inputColor;
	TexCoord = vec2(inputTextureCoords.x, inputTextureCoords.y);
}