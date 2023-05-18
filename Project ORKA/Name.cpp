#include "Name.hpp"

bool operator<(const Name& l, const Name& r) {
	const int x = memcmp(&l.data[0], &r.data[0], NAME_SIZE);
	return 0 > x;
}
