#version 430 core

out vec4 colour;
uniform vec3 color;

void main(void) {    	
	colour = vec4(color, 1.0);
}