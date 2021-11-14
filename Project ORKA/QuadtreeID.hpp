#pragma once

#include "Basics.hpp"
#include "ULLUtil.hpp"

struct QuadtreeID {
	ULLVec2 location = ULLVec2(0);
	UShort level = 0;

	Bool getX();
	Bool getY();

	void clean();
	QuadtreeID parent();
	Vector<QuadtreeID> parents() {
		Vector<QuadtreeID> vp;
		vp.push_back(parent());

		Bool x = getX();
		Bool y = getY();


	};
};