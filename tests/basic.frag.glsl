#version 330 core

in vec2 vTexCoord;

out vec4 oFrag;

uniform sampler2D uTextureSlot0;

void main() {
	oFrag = texture(uTextureSlot0, vTexCoord);
}