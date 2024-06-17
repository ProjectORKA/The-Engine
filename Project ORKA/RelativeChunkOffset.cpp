#include "RelativeChunkOffset.hpp"

Vec3 relativeCameraPosition(const OctreeId& id, const UllVec3& chunkPosition, const Vec3 position)
{
	Vec3 chunkOffset;

	const ULL cx = chunkPosition.x;
	const ULL cy = chunkPosition.y;
	const ULL cz = chunkPosition.z;

	const LL lx = id.position.x - cx;
	const LL ly = id.position.y - cy;

	chunkOffset.x = lx;
	chunkOffset.y = ly;

	if(id.position.z > cz)
	{
		const ULL uz  = id.position.z - cz;
		chunkOffset.z = uz;
	}
	else
	{
		const ULL uz  = cz - id.position.z;
		chunkOffset.z = uz;
		chunkOffset.z = -chunkOffset.z;
	}

	chunkOffset -= position;

	if(chunkOffset.x + pow(2, 63 - id.level) > +pow(2, 63)) chunkOffset.x -= pow(2, 64);
	if(chunkOffset.x + pow(2, 63 - id.level) < -pow(2, 63)) chunkOffset.x += pow(2, 64);

	if(chunkOffset.y + pow(2, 63 - id.level) > +pow(2, 63)) chunkOffset.y -= pow(2, 64);
	if(chunkOffset.y + pow(2, 63 - id.level) < -pow(2, 63)) chunkOffset.y += pow(2, 64);

	chunkOffset /= Vec3(pow(2, 64 - id.level));

	return -chunkOffset;
}
