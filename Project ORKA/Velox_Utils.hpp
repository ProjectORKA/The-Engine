#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"

namespace Utils {
	template <typename T>
	constexpr usize strlen(const T* str) {
		usize size = 0;
		while (*str) {
			++str;
			size++;
		}
		return size;
	}
}
