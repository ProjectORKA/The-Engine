#include "RelativeChunkOffset.hpp"

Vec3 relativeCameraPosition(const OctreeId& id, const ULLVec3& chunkLocation, const Vec3 location) {
	Vec3 chunkOffset;

	const ULL cx = chunkLocation.x;
	const ULL cy = chunkLocation.y;
	const ULL cz = chunkLocation.z;

	const LL lx = id.location.x - cx;
	const LL ly = id.location.y - cy;

	chunkOffset.x = lx;
	chunkOffset.y = ly;

	if (id.location.z > cz) {
		const ULL uz = id.location.z - cz;
		chunkOffset.z = uz;
	}
	else {
		const ULL uz = cz - id.location.z;
		chunkOffset.z = uz;
		chunkOffset.z = -chunkOffset.z;
	}

	chunkOffset -= location;

	if (chunkOffset.x + pow(2, 63 - id.level) > +pow(2, 63)) chunkOffset.x -= pow(2, 64);
	if (chunkOffset.x + pow(2, 63 - id.level) < -pow(2, 63)) chunkOffset.x += pow(2, 64);

	if (chunkOffset.y + pow(2, 63 - id.level) > +pow(2, 63)) chunkOffset.y -= pow(2, 64);
	if (chunkOffset.y + pow(2, 63 - id.level) < -pow(2, 63)) chunkOffset.y += pow(2, 64);

	chunkOffset /= Vec3(pow(2, 64 - id.level));

	return -chunkOffset;
};
