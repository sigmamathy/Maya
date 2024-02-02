#version 330 core

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iTexCoord;

out vec2 vTexCoord;

void main() {
	gl_Position = vec4(iPos, 0, 1);
	vTexCoord = iTexCoord;
}