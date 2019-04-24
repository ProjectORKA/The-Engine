
#include "Mesh.hpp"

Mesh::Mesh() {
	loaded = false;
}
void Mesh::load() {
	std::vector<glm::vec3> vertecies;
	vertecies.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	vertecies.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	vertecies.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float), vertecies.data(), GL_STATIC_DRAW);
	loaded = true;
};
void Mesh::render() {
	printf("rendering\n");
	if (!loaded) load();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);

};
Mesh::~Mesh() {

};

MeshHandler::MeshHandler() {
	meshMap["triangle"] = Mesh();
};