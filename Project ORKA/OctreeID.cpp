#include "OctreeID.hpp"

Double OctreeId::size() const {
	return pow(2, 64 - level);
}
