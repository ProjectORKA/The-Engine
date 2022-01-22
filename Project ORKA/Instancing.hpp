
#pragma once

#include "VertexBufferObject.hpp"

//struct InstancingData {
//	VertexBufferObject transforms;
//
//	void create(Vector<Vec4> & data) {
//		transforms.create(4, glm::value_ptr(data[0]), data.size() * sizeof(Vec4), GL_STATIC_DRAW, 4);
//	};
//	void attach() {
//		glVertexAttribDivisor(2, 1);
//	};
//	void update();
//	void detach();
//	void destroy() {
//		transforms.unload();
//	};
//};