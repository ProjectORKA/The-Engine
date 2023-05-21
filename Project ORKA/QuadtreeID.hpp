#pragma once

#include "Basics.hpp"
#include "ULLUtil.hpp"

struct QuadtreeId {
	ULLVec2 location = ULLVec2(0);
	UShort level = 0;

	Bool getX() const;
	Bool getY() const;

	void clean();
	QuadtreeId parent() const;

	Vector<QuadtreeId> parents() const {
		Vector<QuadtreeId> vp;
		vp.push_back(parent());

		Bool x = getX();
		Bool y = getY();
	};
};
