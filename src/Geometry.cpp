#include "Geometry.h"

Geometry::Geometry() {
	drawMode = GL_TRIANGLES;
	vao = 0;
	vertexBuffer = 0;
	modelMatrix = glm::mat4(1.f);
}

Geometry::Geometry(GLuint dm): drawMode(dm) {
	vao = 0;
	vertexBuffer = 0;
	modelMatrix = glm::mat4(1.f);
}