#pragma once

#include "Basics.hpp"
#include "ULLUtil.hpp"

struct QuadtreeID {
	ULLVec2 location = ULLVec2(0);
	UShort level = 0;

	Bool getX() const;
	Bool getY() const;

	void clean();
	QuadtreeID parent() const;

	Vector<QuadtreeID> parents() const {
		Vector<QuadtreeID> vp;
		vp.push_back(parent());

		Bool x = getX();
		Bool y = getY();
	};
};
