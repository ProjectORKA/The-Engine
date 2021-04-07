
#include "RelativeChunkOffset.hpp"

Vec3 cameraRelativeLocationOfChunk(OctreeID id, PlanetCamera& camera) {

	Vec3 chunkOffset;

	ULL cx = camera.chunkLocation.x;
	ULL cy = camera.chunkLocation.y;
	ULL cz = camera.chunkLocation.z;

	LL  lx = id.location.x - cx;
	LL  ly = id.location.y - cy;

	chunkOffset.x = lx;
	chunkOffset.y = ly;

	if (id.location.z > cz) {
		ULL uz = id.location.z - cz;
		chunkOffset.z = uz;
	}
	else {
		ULL uz = cz - id.location.z;
		chunkOffset.z = uz;
		chunkOffset.z = -chunkOffset.z;
	};

	chunkOffset -= camera.location;

	if (chunkOffset.x + pow(2, 63 - id.level) > +pow(2, 63)) chunkOffset.x -= pow(2, 64);
	if (chunkOffset.x + pow(2, 63 - id.level) < -pow(2, 63)) chunkOffset.x += pow(2, 64);

	if (chunkOffset.y + pow(2, 63 - id.level) > +pow(2, 63)) chunkOffset.y -= pow(2, 64);
	if (chunkOffset.y + pow(2, 63 - id.level) < -pow(2, 63)) chunkOffset.y += pow(2, 64);

	chunkOffset /= Vec3(pow(2, 64 - id.level));

	return chunkOffset;
};