#version 330 core

layout (location = 0) in vec2 iPos;

void main() {
	gl_Position = vec4(iPos, 0, 1);
}