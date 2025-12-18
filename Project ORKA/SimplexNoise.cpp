#include "SimplexNoise.hpp"

class SimplexNoise
{
public:
	SimplexNoise()
	{
		LL seed = 0;

		Short source[256];
		for (Short i = 0; i < 256; i++)
		{
			source[i] = i;
		}
		seed = seed * 6364136223846793005l + 1442695040888963407l;
		seed = seed * 6364136223846793005l + 1442695040888963407l;
		seed = seed * 6364136223846793005l + 1442695040888963407l;
		for (Int i = 255; i >= 0; i--)
		{
			seed  = seed * 6364136223846793005l + 1442695040888963407l;
			Int r = static_cast<Int>((seed + 31) % (i + 1));
			if (r < 0) r += (i + 1);
			perm[i]            = source[r];
			permGradIndex3d[i] = static_cast<Short>((perm[i] % (gradients3d.size() / 3)) * 3);
			source[r]          = source[i];
		}
	}

	const Double stretch2d = -0.211324865405187;
	const Double squish2d  = 0.366025403784439;
	const Double stretch3d = -1.0 / 6;
	const Double squish3d  = 1.0 / 3;
	const Double stretch4d = -0.138196601125011;
	const Double squish4d  = 0.309016994374947;

	const Double norm2d = 47;
	const Double norm3d = 103;
	const Double norm4d = 30;

	const Long defaultSeed = 0;

	Array<Short, 256> perm            = {};
	Array<Short, 256> permGradIndex3d = {};
	Array<Char, 16>   gradients2d     = {5, 2, 2, 5, -5, 2, -2, 5, 5, -2, 2, -5, -5, -2, -2, -5,};
	Array<Char, 72>   gradients3d     = {-11, 4, 4, -4, 11, 4, -4, 4, 11, 11, 4, 4, 4, 11, 4, 4, 4, 11, -11, -4, 4, -4, -11, 4, -4, -4, 11, 11, -4, 4, 4, -11, 4, 4, -4, 11, -11, 4, -4, -4, 11, -4, -4, 4, -11, 11, 4, -4, 4, 11, -4, 4, 4, -11, -11, -4, -4, -4, -11, -4, -4, -4, -11, 11, -4, -4, 4, -11, -4, 4, -4, -11,};
	Array<Char, 256>  gradients4d     = {3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, -3, 1, 1, 1, -1, 3, 1, 1, -1, 1, 3, 1, -1, 1, 1, 3, 3, -1, 1, 1, 1, -3, 1, 1, 1, -1, 3, 1, 1, -1, 1, 3, -3, -1, 1, 1, -1, -3, 1, 1, -1, -1, 3, 1, -1, -1, 1, 3, 3, 1, -1, 1, 1, 3, -1, 1, 1, 1, -3, 1, 1, 1, -1, 3, -3, 1, -1, 1, -1, 3, -1, 1, -1, 1, -3, 1, -1, 1, -1, 3, 3, -1, -1, 1, 1, -3, -1, 1, 1, -1, -3, 1, 1, -1, -1, 3, -3, -1, -1, 1, -1, -3, -1, 1, -1, -1, -3, 1, -1, -1, -1, 3, 3, 1, 1, -1, 1, 3, 1, -1, 1, 1, 3, -1, 1, 1, 1, -3, -3, 1, 1, -1, -1, 3, 1, -1, -1, 1, 3, -1, -1, 1, 1, -3, 3, -1, 1, -1, 1, -3, 1, -1, 1, -1, 3, -1, 1, -1, 1, -3, -3, -1, 1, -1, -1, -3, 1, -1, -1, -1, 3, -1, -1, -1, 1, -3, 3, 1, -1, -1, 1, 3, -1, -1, 1, 1, -3, -1, 1, 1, -1, -3, -3, 1, -1, -1, -1, 3, -1, -1, -1, 1, -3, -1, -1, 1, -1, -3, 3, -1, -1, -1, 1, -3, -1, -1, 1, -1, -3, -1, 1, -1, -1, -3, -3, -1, -1, -1, -1, -3, -1, -1, -1, -1, -3, -1, -1, -1, -1, -3,};

	[[nodiscard]] Double extrapolate(Int xsb, Int ysb, Double dx, Double dy) const;
	[[nodiscard]] Double extrapolate(Int xsb, Int ysb, Int zsb, Double dx, Double dy, Double dz) const;
	[[nodiscard]] Double extrapolate(Int xsb, Int ysb, Int zsb, Int wsb, Double dx, Double dy, Double dz, Double dw) const;
};

static SimplexNoise noise;

Double simplexNoise(const Double x, const Double y)
{
	//Place input coordinates onto grid.
	const Double stretchOffset = (x + y) * noise.stretch2d;
	const Double xs            = x + stretchOffset;
	const Double ys            = y + stretchOffset;

	//Floor to get grid coordinates of rhombus (stretched square) super-cell origin.
	Int xsb = static_cast<Int>(floor(xs));
	Int ysb = static_cast<Int>(floor(ys));

	//Skew out to get actual coordinates of rhombus origin. We'll need these later.
	const Double squishOffset = (xsb + ysb) * noise.squish2d;
	const Double xb           = xsb + squishOffset;
	const Double yb           = ysb + squishOffset;

	//Compute grid coordinates relative to rhombus origin.
	const Double xins = xs - xsb;
	const Double yins = ys - ysb;

	//Sum those together to get a value that determines which region we're in.
	const Double inSum = xins + yins;

	//Positions relative to origin point.
	Double dx0 = x - xb;
	Double dy0 = y - yb;

	//We'll be defining these inside the next block and using them afterwards.
	Double dx_ext,  dy_ext;
	Int    xsv_ext, ysv_ext;

	Double value = 0;

	//Contribution (1,0)
	const Double dx1 = dx0 - 1 - noise.squish2d;
	const Double dy1 = dy0 - 0 - noise.squish2d;
	if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1; attn1 > 0)
	{
		attn1 *= attn1;
		value += attn1 * attn1 * noise.extrapolate(xsb + 1, ysb + 0, dx1, dy1);
	}

	//Contribution (0,1)
	const Double dx2 = dx0 - 0 - noise.squish2d;
	const Double dy2 = dy0 - 1 - noise.squish2d;
	if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2; attn2 > 0)
	{
		attn2 *= attn2;
		value += attn2 * attn2 * noise.extrapolate(xsb + 0, ysb + 1, dx2, dy2);
	}

	if (inSum <= 1)
	{
		//We're inside the triangle (2-Simplex) at (0,0)
		if (const Double zins = 1 - inSum; zins > xins || zins > yins)
		{
			//(0,0) is one of the closest two triangular vertices
			if (xins > yins)
			{
				xsv_ext = xsb + 1;
				ysv_ext = ysb - 1;
				dx_ext  = dx0 - 1;
				dy_ext  = dy0 + 1;
			}
			else
			{
				xsv_ext = xsb - 1;
				ysv_ext = ysb + 1;
				dx_ext  = dx0 + 1;
				dy_ext  = dy0 - 1;
			}
		}
		else
		{
			//(1,0) and (0,1) are the closest two vertices.
			xsv_ext = xsb + 1;
			ysv_ext = ysb + 1;
			dx_ext  = dx0 - 1 - 2 * noise.squish2d;
			dy_ext  = dy0 - 1 - 2 * noise.squish2d;
		}
	}
	else
	{
		//We're inside the triangle (2-Simplex) at (1,1)
		if (const Double zins = 2 - inSum; zins < xins || zins < yins)
		{
			//(0,0) is one of the closest two triangular vertices
			if (xins > yins)
			{
				xsv_ext = xsb + 2;
				ysv_ext = ysb + 0;
				dx_ext  = dx0 - 2 - 2 * noise.squish2d;
				dy_ext  = dy0 + 0 - 2 * noise.squish2d;
			}
			else
			{
				xsv_ext = xsb + 0;
				ysv_ext = ysb + 2;
				dx_ext  = dx0 + 0 - 2 * noise.squish2d;
				dy_ext  = dy0 - 2 - 2 * noise.squish2d;
			}
		}
		else
		{
			//(1,0) and (0,1) are the closest two vertices.
			dx_ext  = dx0;
			dy_ext  = dy0;
			xsv_ext = xsb;
			ysv_ext = ysb;
		}
		xsb += 1;
		ysb += 1;
		dx0 = dx0 - 1 - 2 * noise.squish2d;
		dy0 = dy0 - 1 - 2 * noise.squish2d;
	}

	//Contribution (0,0) or (1,1)
	if (Double attn0 = 2 - dx0 * dx0 - dy0 * dy0; attn0 > 0)
	{
		attn0 *= attn0;
		value += attn0 * attn0 * noise.extrapolate(xsb, ysb, dx0, dy0);
	}

	//Extra Vertex
	if (Double attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext; attn_ext > 0)
	{
		attn_ext *= attn_ext;
		value += attn_ext * attn_ext * noise.extrapolate(xsv_ext, ysv_ext, dx_ext, dy_ext);
	}

	return value / noise.norm2d;
}

Double simplexNoise(Double x, Double y, Double z)
{
	//Place input coordinates on simplectic honeycomb.
	Double stretchOffset = (x + y + z) * noise.stretch3d;
	Double xs            = x + stretchOffset;
	Double ys            = y + stretchOffset;
	Double zs            = z + stretchOffset;

	//static_cast<int>(floor to get simplectic honeycomb coordinates of rhombohedron (stretched cube) super-cell origin.
	Int xsb = static_cast<Int>(floor(xs));
	Int ysb = static_cast<Int>(floor(ys));
	Int zsb = static_cast<Int>(floor(zs));

	//Skew out to get actual coordinates of rhombohedron origin. We'll need these later.
	Double squishOffset = (xsb + ysb + zsb) * noise.squish3d;
	Double xb           = xsb + squishOffset;
	Double yb           = ysb + squishOffset;
	Double zb           = zsb + squishOffset;

	//Compute simplectic honeycomb coordinates relative to rhombohedral origin.
	Double xins = xs - xsb;
	Double yins = ys - ysb;
	Double zins = zs - zsb;

	//Sum those together to get a value that determines which region we're in.
	Double inSum = xins + yins + zins;

	//Positions relative to origin point.
	Double dx0 = x - xb;
	Double dy0 = y - yb;
	Double dz0 = z - zb;

	//We'll be defining these inside the next block and using them afterwards.
	Double dx_ext0,  dy_ext0,  dz_ext0;
	Double dx_ext1,  dy_ext1,  dz_ext1;
	Int    xsv_ext0, ysv_ext0, zsv_ext0;
	Int    xsv_ext1, ysv_ext1, zsv_ext1;

	Double value = 0;
	if (inSum <= 1)
	{
		//We're inside the tetrahedron (3-Simplex) at (0,0,0)

		//Determine which two of (0,0,1), (0,1,0), (1,0,0) are closest.
		Char   aPoint = 0x01;
		Double aScore = xins;
		Char   bPoint = 0x02;
		Double bScore = yins;
		if (aScore >= bScore && zins > bScore)
		{
			bScore = zins;
			bPoint = 0x04;
		}
		else if (aScore < bScore && zins > aScore)
		{
			aScore = zins;
			aPoint = 0x04;
		}

		//Now we determine the two lattice points not part of the tetrahedron that may contribute.
		//This depends on the closest two tetrahedral vertices, including (0,0,0)
		if (Double wins = 1 - inSum; wins > aScore || wins > bScore)
		{
			//(0,0,0) is one of the closest two tetrahedral vertices.
			Char c = (bScore > aScore ? bPoint : aPoint); //Our other closest vertex is the closest out of a and b.

			if ((c & 0x01) == 0)
			{
				xsv_ext0 = xsb - 1;
				xsv_ext1 = xsb;
				dx_ext0  = dx0 + 1;
				dx_ext1  = dx0;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsb + 1;
				dx_ext0  = dx_ext1  = dx0 - 1;
			}

			if ((c & 0x02) == 0)
			{
				ysv_ext0 = ysv_ext1 = ysb;
				dy_ext0  = dy_ext1  = dy0;
				if ((c & 0x01) == 0)
				{
					ysv_ext1 -= 1;
					dy_ext1 += 1;
				}
				else
				{
					ysv_ext0 -= 1;
					dy_ext0 += 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysb + 1;
				dy_ext0  = dy_ext1  = dy0 - 1;
			}

			if ((c & 0x04) == 0)
			{
				zsv_ext0 = zsb;
				zsv_ext1 = zsb - 1;
				dz_ext0  = dz0;
				dz_ext1  = dz0 + 1;
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsb + 1;
				dz_ext0  = dz_ext1  = dz0 - 1;
			}
		}
		else
		{
			//(0,0,0) is not one of the closest two tetrahedral vertices.
			Char c = static_cast<Char>(aPoint | bPoint); //Our two extra vertices are determined by the closest two.

			if ((c & 0x01) == 0)
			{
				xsv_ext0 = xsb;
				xsv_ext1 = xsb - 1;
				dx_ext0  = dx0 - 2 * noise.squish3d;
				dx_ext1  = dx0 + 1 - noise.squish3d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsb + 1;
				dx_ext0  = dx0 - 1 - 2 * noise.squish3d;
				dx_ext1  = dx0 - 1 - noise.squish3d;
			}

			if ((c & 0x02) == 0)
			{
				ysv_ext0 = ysb;
				ysv_ext1 = ysb - 1;
				dy_ext0  = dy0 - 2 * noise.squish3d;
				dy_ext1  = dy0 + 1 - noise.squish3d;
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysb + 1;
				dy_ext0  = dy0 - 1 - 2 * noise.squish3d;
				dy_ext1  = dy0 - 1 - noise.squish3d;
			}

			if ((c & 0x04) == 0)
			{
				zsv_ext0 = zsb;
				zsv_ext1 = zsb - 1;
				dz_ext0  = dz0 - 2 * noise.squish3d;
				dz_ext1  = dz0 + 1 - noise.squish3d;
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsb + 1;
				dz_ext0  = dz0 - 1 - 2 * noise.squish3d;
				dz_ext1  = dz0 - 1 - noise.squish3d;
			}
		}

		//Contribution (0,0,0)
		if (Double attn0 = 2 - dx0 * dx0 - dy0 * dy0 - dz0 * dz0; attn0 > 0)
		{
			attn0 *= attn0;
			value += attn0 * attn0 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 0, dx0, dy0, dz0);
		}

		//Contribution (1,0,0)
		Double dx1 = dx0 - 1 - noise.squish3d;
		Double dy1 = dy0 - 0 - noise.squish3d;
		Double dz1 = dz0 - 0 - noise.squish3d;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 0, dx1, dy1, dz1);
		}

		//Contribution (0,1,0)
		Double dx2 = dx0 - 0 - noise.squish3d;
		Double dy2 = dy0 - 1 - noise.squish3d;
		Double dz2 = dz1;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 0, dx2, dy2, dz2);
		}

		//Contribution (0,0,1)
		Double dx3 = dx2;
		Double dy3 = dy1;
		Double dz3 = dz0 - 1 - noise.squish3d;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 1, dx3, dy3, dz3);
		}
	}
	else if (inSum >= 2)
	{
		//We're inside the tetrahedron (3-Simplex) at (1,1,1)

		//Determine which two tetrahedral vertices are the closest, out of (1,1,0), (1,0,1), (0,1,1) but not (1,1,1).
		Char   aPoint = 0x06;
		Double aScore = xins;
		Char   bPoint = 0x05;
		Double bScore = yins;
		if (aScore <= bScore && zins < bScore)
		{
			bScore = zins;
			bPoint = 0x03;
		}
		else if (aScore > bScore && zins < aScore)
		{
			aScore = zins;
			aPoint = 0x03;
		}

		//Now we determine the two lattice points not part of the tetrahedron that may contribute.
		//This depends on the closest two tetrahedral vertices, including (1,1,1)
		if (Double wins = 3 - inSum; wins < aScore || wins < bScore)
		{
			//(1,1,1) is one of the closest two tetrahedral vertices.
			Char c = (bScore < aScore ? bPoint : aPoint); //Our other closest vertex is the closest out of a and b.

			if ((c & 0x01) != 0)
			{
				xsv_ext0 = xsb + 2;
				xsv_ext1 = xsb + 1;
				dx_ext0  = dx0 - 2 - 3 * noise.squish3d;
				dx_ext1  = dx0 - 1 - 3 * noise.squish3d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsb;
				dx_ext0  = dx_ext1  = dx0 - 3 * noise.squish3d;
			}

			if ((c & 0x02) != 0)
			{
				ysv_ext0 = ysv_ext1 = ysb + 1;
				dy_ext0  = dy_ext1  = dy0 - 1 - 3 * noise.squish3d;
				if ((c & 0x01) != 0)
				{
					ysv_ext1 += 1;
					dy_ext1 -= 1;
				}
				else
				{
					ysv_ext0 += 1;
					dy_ext0 -= 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysb;
				dy_ext0  = dy_ext1  = dy0 - 3 * noise.squish3d;
			}

			if ((c & 0x04) != 0)
			{
				zsv_ext0 = zsb + 1;
				zsv_ext1 = zsb + 2;
				dz_ext0  = dz0 - 1 - 3 * noise.squish3d;
				dz_ext1  = dz0 - 2 - 3 * noise.squish3d;
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsb;
				dz_ext0  = dz_ext1  = dz0 - 3 * noise.squish3d;
			}
		}
		else
		{
			//(1,1,1) is not one of the closest two tetrahedral vertices.
			Char c = static_cast<Char>(aPoint & bPoint); //Our two extra vertices are determined by the closest two.

			if ((c & 0x01) != 0)
			{
				xsv_ext0 = xsb + 1;
				xsv_ext1 = xsb + 2;
				dx_ext0  = dx0 - 1 - noise.squish3d;
				dx_ext1  = dx0 - 2 - 2 * noise.squish3d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsb;
				dx_ext0  = dx0 - noise.squish3d;
				dx_ext1  = dx0 - 2 * noise.squish3d;
			}

			if ((c & 0x02) != 0)
			{
				ysv_ext0 = ysb + 1;
				ysv_ext1 = ysb + 2;
				dy_ext0  = dy0 - 1 - noise.squish3d;
				dy_ext1  = dy0 - 2 - 2 * noise.squish3d;
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysb;
				dy_ext0  = dy0 - noise.squish3d;
				dy_ext1  = dy0 - 2 * noise.squish3d;
			}

			if ((c & 0x04) != 0)
			{
				zsv_ext0 = zsb + 1;
				zsv_ext1 = zsb + 2;
				dz_ext0  = dz0 - 1 - noise.squish3d;
				dz_ext1  = dz0 - 2 - 2 * noise.squish3d;
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsb;
				dz_ext0  = dz0 - noise.squish3d;
				dz_ext1  = dz0 - 2 * noise.squish3d;
			}
		}

		//Contribution (1,1,0)
		Double dx3 = dx0 - 1 - 2 * noise.squish3d;
		Double dy3 = dy0 - 1 - 2 * noise.squish3d;
		Double dz3 = dz0 - 0 - 2 * noise.squish3d;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 0, dx3, dy3, dz3);
		}

		//Contribution (1,0,1)
		Double dx2 = dx3;
		Double dy2 = dy0 - 0 - 2 * noise.squish3d;
		Double dz2 = dz0 - 1 - 2 * noise.squish3d;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 1, dx2, dy2, dz2);
		}

		//Contribution (0,1,1)
		Double dx1 = dx0 - 0 - 2 * noise.squish3d;
		Double dy1 = dy3;
		Double dz1 = dz2;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 1, dx1, dy1, dz1);
		}

		//Contribution (1,1,1)
		dx0 = dx0 - 1 - 3 * noise.squish3d;
		dy0 = dy0 - 1 - 3 * noise.squish3d;
		dz0 = dz0 - 1 - 3 * noise.squish3d;
		if (Double attn0 = 2 - dx0 * dx0 - dy0 * dy0 - dz0 * dz0; attn0 > 0)
		{
			attn0 *= attn0;
			value += attn0 * attn0 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 1, dx0, dy0, dz0);
		}
	}
	else
	{
		//We're inside the octahedron (Rectified 3-Simplex) in between.
		Double aScore;
		Char   aPoint;
		Bool   aIsFurtherSide;
		Double bScore;
		Char   bPoint;
		Bool   bIsFurtherSide;

		//Decide between point (0,0,1) and (1,1,0) as closest
		if (Double p1 = xins + yins; p1 > 1)
		{
			aScore         = p1 - 1;
			aPoint         = 0x03;
			aIsFurtherSide = true;
		}
		else
		{
			aScore         = 1 - p1;
			aPoint         = 0x04;
			aIsFurtherSide = false;
		}

		//Decide between point (0,1,0) and (1,0,1) as closest
		if (Double p2 = xins + zins; p2 > 1)
		{
			bScore         = p2 - 1;
			bPoint         = 0x05;
			bIsFurtherSide = true;
		}
		else
		{
			bScore         = 1 - p2;
			bPoint         = 0x02;
			bIsFurtherSide = false;
		}

		//The closest out of the two (1,0,0) and (0,1,1) will replace the furthest out of the two decided above, if closer.
		if (Double p3 = yins + zins; p3 > 1)
		{
			if (Double score = p3 - 1; aScore <= bScore && aScore < score)
			{
				aScore         = score;
				aPoint         = 0x06;
				aIsFurtherSide = true;
			}
			else if (aScore > bScore && bScore < score)
			{
				bScore         = score;
				bPoint         = 0x06;
				bIsFurtherSide = true;
			}
		}
		else
		{
			if (Double score = 1 - p3; aScore <= bScore && aScore < score)
			{
				aScore         = score;
				aPoint         = 0x01;
				aIsFurtherSide = false;
			}
			else if (aScore > bScore && bScore < score)
			{
				bScore         = score;
				bPoint         = 0x01;
				bIsFurtherSide = false;
			}
		}

		//Where each of the two closest points are determines how the extra two vertices are calculated.
		if (aIsFurtherSide == bIsFurtherSide)
		{
			if (aIsFurtherSide)
			{
				//Both closest points on (1,1,1) side

				//One of the two extra points is (1,1,1)
				dx_ext0  = dx0 - 1 - 3 * noise.squish3d;
				dy_ext0  = dy0 - 1 - 3 * noise.squish3d;
				dz_ext0  = dz0 - 1 - 3 * noise.squish3d;
				xsv_ext0 = xsb + 1;
				ysv_ext0 = ysb + 1;
				zsv_ext0 = zsb + 1;

				//Other extra point is based on the shared axis.
				if (Char c = static_cast<Char>(aPoint & bPoint); (c & 0x01) != 0)
				{
					dx_ext1  = dx0 - 2 - 2 * noise.squish3d;
					dy_ext1  = dy0 - 2 * noise.squish3d;
					dz_ext1  = dz0 - 2 * noise.squish3d;
					xsv_ext1 = xsb + 2;
					ysv_ext1 = ysb;
					zsv_ext1 = zsb;
				}
				else if ((c & 0x02) != 0)
				{
					dx_ext1  = dx0 - 2 * noise.squish3d;
					dy_ext1  = dy0 - 2 - 2 * noise.squish3d;
					dz_ext1  = dz0 - 2 * noise.squish3d;
					xsv_ext1 = xsb;
					ysv_ext1 = ysb + 2;
					zsv_ext1 = zsb;
				}
				else
				{
					dx_ext1  = dx0 - 2 * noise.squish3d;
					dy_ext1  = dy0 - 2 * noise.squish3d;
					dz_ext1  = dz0 - 2 - 2 * noise.squish3d;
					xsv_ext1 = xsb;
					ysv_ext1 = ysb;
					zsv_ext1 = zsb + 2;
				}
			}
			else
			{
				//Both closest points on (0,0,0) side

				//One of the two extra points is (0,0,0)
				dx_ext0  = dx0;
				dy_ext0  = dy0;
				dz_ext0  = dz0;
				xsv_ext0 = xsb;
				ysv_ext0 = ysb;
				zsv_ext0 = zsb;

				//Other extra point is based on the omitted axis.
				if (Char c = static_cast<Char>(aPoint | bPoint); (c & 0x01) == 0)
				{
					dx_ext1  = dx0 + 1 - noise.squish3d;
					dy_ext1  = dy0 - 1 - noise.squish3d;
					dz_ext1  = dz0 - 1 - noise.squish3d;
					xsv_ext1 = xsb - 1;
					ysv_ext1 = ysb + 1;
					zsv_ext1 = zsb + 1;
				}
				else if ((c & 0x02) == 0)
				{
					dx_ext1  = dx0 - 1 - noise.squish3d;
					dy_ext1  = dy0 + 1 - noise.squish3d;
					dz_ext1  = dz0 - 1 - noise.squish3d;
					xsv_ext1 = xsb + 1;
					ysv_ext1 = ysb - 1;
					zsv_ext1 = zsb + 1;
				}
				else
				{
					dx_ext1  = dx0 - 1 - noise.squish3d;
					dy_ext1  = dy0 - 1 - noise.squish3d;
					dz_ext1  = dz0 + 1 - noise.squish3d;
					xsv_ext1 = xsb + 1;
					ysv_ext1 = ysb + 1;
					zsv_ext1 = zsb - 1;
				}
			}
		}
		else
		{
			//One point on (0,0,0) side, one point on (1,1,1) side
			Char c1, c2;
			if (aIsFurtherSide)
			{
				c1 = aPoint;
				c2 = bPoint;
			}
			else
			{
				c1 = bPoint;
				c2 = aPoint;
			}

			//One contribution is a permutation of (1,1,-1)
			if ((c1 & 0x01) == 0)
			{
				dx_ext0  = dx0 + 1 - noise.squish3d;
				dy_ext0  = dy0 - 1 - noise.squish3d;
				dz_ext0  = dz0 - 1 - noise.squish3d;
				xsv_ext0 = xsb - 1;
				ysv_ext0 = ysb + 1;
				zsv_ext0 = zsb + 1;
			}
			else if ((c1 & 0x02) == 0)
			{
				dx_ext0  = dx0 - 1 - noise.squish3d;
				dy_ext0  = dy0 + 1 - noise.squish3d;
				dz_ext0  = dz0 - 1 - noise.squish3d;
				xsv_ext0 = xsb + 1;
				ysv_ext0 = ysb - 1;
				zsv_ext0 = zsb + 1;
			}
			else
			{
				dx_ext0  = dx0 - 1 - noise.squish3d;
				dy_ext0  = dy0 - 1 - noise.squish3d;
				dz_ext0  = dz0 + 1 - noise.squish3d;
				xsv_ext0 = xsb + 1;
				ysv_ext0 = ysb + 1;
				zsv_ext0 = zsb - 1;
			}

			//One contribution is a permutation of (0,0,2)
			dx_ext1  = dx0 - 2 * noise.squish3d;
			dy_ext1  = dy0 - 2 * noise.squish3d;
			dz_ext1  = dz0 - 2 * noise.squish3d;
			xsv_ext1 = xsb;
			ysv_ext1 = ysb;
			zsv_ext1 = zsb;
			if ((c2 & 0x01) != 0)
			{
				dx_ext1 -= 2;
				xsv_ext1 += 2;
			}
			else if ((c2 & 0x02) != 0)
			{
				dy_ext1 -= 2;
				ysv_ext1 += 2;
			}
			else
			{
				dz_ext1 -= 2;
				zsv_ext1 += 2;
			}
		}

		//Contribution (1,0,0)
		Double dx1 = dx0 - 1 - noise.squish3d;
		Double dy1 = dy0 - 0 - noise.squish3d;
		Double dz1 = dz0 - 0 - noise.squish3d;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 0, dx1, dy1, dz1);
		}

		//Contribution (0,1,0)
		Double dx2 = dx0 - 0 - noise.squish3d;
		Double dy2 = dy0 - 1 - noise.squish3d;
		Double dz2 = dz1;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 0, dx2, dy2, dz2);
		}

		//Contribution (0,0,1)
		Double dx3 = dx2;
		Double dy3 = dy1;
		Double dz3 = dz0 - 1 - noise.squish3d;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 1, dx3, dy3, dz3);
		}

		//Contribution (1,1,0)
		Double dx4 = dx0 - 1 - 2 * noise.squish3d;
		Double dy4 = dy0 - 1 - 2 * noise.squish3d;
		Double dz4 = dz0 - 0 - 2 * noise.squish3d;
		if (Double attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4; attn4 > 0)
		{
			attn4 *= attn4;
			value += attn4 * attn4 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 0, dx4, dy4, dz4);
		}

		//Contribution (1,0,1)
		Double dx5 = dx4;
		Double dy5 = dy0 - 0 - 2 * noise.squish3d;
		Double dz5 = dz0 - 1 - 2 * noise.squish3d;
		if (Double attn5 = 2 - dx5 * dx5 - dy5 * dy5 - dz5 * dz5; attn5 > 0)
		{
			attn5 *= attn5;
			value += attn5 * attn5 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 1, dx5, dy5, dz5);
		}

		//Contribution (0,1,1)
		Double dx6 = dx0 - 0 - 2 * noise.squish3d;
		Double dy6 = dy4;
		Double dz6 = dz5;
		if (Double attn6 = 2 - dx6 * dx6 - dy6 * dy6 - dz6 * dz6; attn6 > 0)
		{
			attn6 *= attn6;
			value += attn6 * attn6 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 1, dx6, dy6, dz6);
		}
	}

	//First extra vertex
	if (Double attn_ext0 = 2 - dx_ext0 * dx_ext0 - dy_ext0 * dy_ext0 - dz_ext0 * dz_ext0; attn_ext0 > 0)
	{
		attn_ext0 *= attn_ext0;
		value += attn_ext0 * attn_ext0 * noise.extrapolate(xsv_ext0, ysv_ext0, zsv_ext0, dx_ext0, dy_ext0, dz_ext0);
	}

	//Second extra vertex
	if (Double attn_ext1 = 2 - dx_ext1 * dx_ext1 - dy_ext1 * dy_ext1 - dz_ext1 * dz_ext1; attn_ext1 > 0)
	{
		attn_ext1 *= attn_ext1;
		value += attn_ext1 * attn_ext1 * noise.extrapolate(xsv_ext1, ysv_ext1, zsv_ext1, dx_ext1, dy_ext1, dz_ext1);
	}

	return value / noise.norm3d;
}

Double simplexNoise(Double x, Double y, Double z, Double w)
{
	//Place input coordinates on simplectic honeycomb.
	Double stretchOffset = (x + y + z + w) * noise.stretch4d;
	Double xs            = x + stretchOffset;
	Double ys            = y + stretchOffset;
	Double zs            = z + stretchOffset;
	Double ws            = w + stretchOffset;

	//static_cast<int>(floor to get simplectic honeycomb coordinates of rhombo-hypercube super-cell origin.
	Int xsb = static_cast<Int>(floor(xs));
	Int ysb = static_cast<Int>(floor(ys));
	Int zsb = static_cast<Int>(floor(zs));
	Int wsb = static_cast<Int>(floor(ws));

	//Skew out to get actual coordinates of stretched rhombo-hypercube origin. We'll need these later.
	Double squishOffset = (xsb + ysb + zsb + wsb) * noise.squish4d;
	Double xb           = xsb + squishOffset;
	Double yb           = ysb + squishOffset;
	Double zb           = zsb + squishOffset;
	Double wb           = wsb + squishOffset;

	//Compute simplectic honeycomb coordinates relative to rhombo-hypercube origin.
	Double xins = xs - xsb;
	Double yins = ys - ysb;
	Double zins = zs - zsb;
	Double wins = ws - wsb;

	//Sum those together to get a value that determines which region we're in.
	Double inSum = xins + yins + zins + wins;

	//Positions relative to origin point.
	Double dx0 = x - xb;
	Double dy0 = y - yb;
	Double dz0 = z - zb;
	Double dw0 = w - wb;

	//We'll be defining these inside the next block and using them afterwards.
	Double dx_ext0,  dy_ext0,  dz_ext0,  dw_ext0;
	Double dx_ext1,  dy_ext1,  dz_ext1,  dw_ext1;
	Double dx_ext2,  dy_ext2,  dz_ext2,  dw_ext2;
	Int    xsv_ext0, ysv_ext0, zsv_ext0, wsv_ext0;
	Int    xsv_ext1, ysv_ext1, zsv_ext1, wsv_ext1;
	Int    xsv_ext2, ysv_ext2, zsv_ext2, wsv_ext2;

	Double value = 0;
	if (inSum <= 1)
	{
		//We're inside the pentachoron (4-Simplex) at (0,0,0,0)

		//Determine which two of (0,0,0,1), (0,0,1,0), (0,1,0,0), (1,0,0,0) are closest.
		Char   aPoint = 0x01;
		Double aScore = xins;
		Char   bPoint = 0x02;
		Double bScore = yins;
		if (aScore >= bScore && zins > bScore)
		{
			bScore = zins;
			bPoint = 0x04;
		}
		else if (aScore < bScore && zins > aScore)
		{
			aScore = zins;
			aPoint = 0x04;
		}
		if (aScore >= bScore && wins > bScore)
		{
			bScore = wins;
			bPoint = 0x08;
		}
		else if (aScore < bScore && wins > aScore)
		{
			aScore = wins;
			aPoint = 0x08;
		}

		//Now we determine the three lattice points not part of the pentachoron that may contribute.
		//This depends on the closest two pentachoron vertices, including (0,0,0,0)
		if (Double uins = 1 - inSum; uins > aScore || uins > bScore)
		{
			//(0,0,0,0) is one of the closest two pentachoron vertices.
			Char c = (bScore > aScore ? bPoint : aPoint); //Our other closest vertex is the closest out of a and b.
			if ((c & 0x01) == 0)
			{
				xsv_ext0 = xsb - 1;
				xsv_ext1 = xsv_ext2 = xsb;
				dx_ext0  = dx0 + 1;
				dx_ext1  = dx_ext2 = dx0;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb + 1;
				dx_ext0  = dx_ext1  = dx_ext2  = dx0 - 1;
			}

			if ((c & 0x02) == 0)
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
				dy_ext0  = dy_ext1  = dy_ext2  = dy0;
				if ((c & 0x01) == 0x01)
				{
					ysv_ext0 -= 1;
					dy_ext0 += 1;
				}
				else
				{
					ysv_ext1 -= 1;
					dy_ext1 += 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
				dy_ext0  = dy_ext1  = dy_ext2  = dy0 - 1;
			}

			if ((c & 0x04) == 0)
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
				dz_ext0  = dz_ext1  = dz_ext2  = dz0;
				if ((c & 0x03) != 0)
				{
					if ((c & 0x03) == 0x03)
					{
						zsv_ext0 -= 1;
						dz_ext0 += 1;
					}
					else
					{
						zsv_ext1 -= 1;
						dz_ext1 += 1;
					}
				}
				else
				{
					zsv_ext2 -= 1;
					dz_ext2 += 1;
				}
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
				dz_ext0  = dz_ext1  = dz_ext2  = dz0 - 1;
			}

			if ((c & 0x08) == 0)
			{
				wsv_ext0 = wsv_ext1 = wsb;
				wsv_ext2 = wsb - 1;
				dw_ext0  = dw_ext1 = dw0;
				dw_ext2  = dw0 + 1;
			}
			else
			{
				wsv_ext0 = wsv_ext1 = wsv_ext2 = wsb + 1;
				dw_ext0  = dw_ext1  = dw_ext2  = dw0 - 1;
			}
		}
		else
		{
			//(0,0,0,0) is not one of the closest two pentachoron vertices.
			Char c = static_cast<Char>(aPoint | bPoint); //Our three extra vertices are determined by the closest two.

			if ((c & 0x01) == 0)
			{
				xsv_ext0 = xsv_ext2 = xsb;
				xsv_ext1 = xsb - 1;
				dx_ext0  = dx0 - 2 * noise.squish4d;
				dx_ext1  = dx0 + 1 - noise.squish4d;
				dx_ext2  = dx0 - noise.squish4d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb + 1;
				dx_ext0  = dx0 - 1 - 2 * noise.squish4d;
				dx_ext1  = dx_ext2 = dx0 - 1 - noise.squish4d;
			}

			if ((c & 0x02) == 0)
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
				dy_ext0  = dy0 - 2 * noise.squish4d;
				dy_ext1  = dy_ext2 = dy0 - noise.squish4d;
				if ((c & 0x01) == 0x01)
				{
					ysv_ext1 -= 1;
					dy_ext1 += 1;
				}
				else
				{
					ysv_ext2 -= 1;
					dy_ext2 += 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
				dy_ext0  = dy0 - 1 - 2 * noise.squish4d;
				dy_ext1  = dy_ext2 = dy0 - 1 - noise.squish4d;
			}

			if ((c & 0x04) == 0)
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
				dz_ext0  = dz0 - 2 * noise.squish4d;
				dz_ext1  = dz_ext2 = dz0 - noise.squish4d;
				if ((c & 0x03) == 0x03)
				{
					zsv_ext1 -= 1;
					dz_ext1 += 1;
				}
				else
				{
					zsv_ext2 -= 1;
					dz_ext2 += 1;
				}
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
				dz_ext0  = dz0 - 1 - 2 * noise.squish4d;
				dz_ext1  = dz_ext2 = dz0 - 1 - noise.squish4d;
			}

			if ((c & 0x08) == 0)
			{
				wsv_ext0 = wsv_ext1 = wsb;
				wsv_ext2 = wsb - 1;
				dw_ext0  = dw0 - 2 * noise.squish4d;
				dw_ext1  = dw0 - noise.squish4d;
				dw_ext2  = dw0 + 1 - noise.squish4d;
			}
			else
			{
				wsv_ext0 = wsv_ext1 = wsv_ext2 = wsb + 1;
				dw_ext0  = dw0 - 1 - 2 * noise.squish4d;
				dw_ext1  = dw_ext2 = dw0 - 1 - noise.squish4d;
			}
		}

		//Contribution (0,0,0,0)
		if (Double attn0 = 2 - dx0 * dx0 - dy0 * dy0 - dz0 * dz0 - dw0 * dw0; attn0 > 0)
		{
			attn0 *= attn0;
			value += attn0 * attn0 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 0, wsb + 0, dx0, dy0, dz0, dw0);
		}

		//Contribution (1,0,0,0)
		Double dx1 = dx0 - 1 - noise.squish4d;
		Double dy1 = dy0 - 0 - noise.squish4d;
		Double dz1 = dz0 - 0 - noise.squish4d;
		Double dw1 = dw0 - 0 - noise.squish4d;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 0, wsb + 0, dx1, dy1, dz1, dw1);
		}

		//Contribution (0,1,0,0)
		Double dx2 = dx0 - 0 - noise.squish4d;
		Double dy2 = dy0 - 1 - noise.squish4d;
		Double dz2 = dz1;
		Double dw2 = dw1;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 0, wsb + 0, dx2, dy2, dz2, dw2);
		}

		//Contribution (0,0,1,0)
		Double dx3 = dx2;
		Double dy3 = dy1;
		Double dz3 = dz0 - 1 - noise.squish4d;
		Double dw3 = dw1;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 1, wsb + 0, dx3, dy3, dz3, dw3);
		}

		//Contribution (0,0,0,1)
		Double dx4 = dx2;
		Double dy4 = dy1;
		Double dz4 = dz1;
		Double dw4 = dw0 - 1 - noise.squish4d;
		if (Double attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4; attn4 > 0)
		{
			attn4 *= attn4;
			value += attn4 * attn4 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 0, wsb + 1, dx4, dy4, dz4, dw4);
		}
	}
	else if (inSum >= 3)
	{
		//We're inside the pentachoron (4-Simplex) at (1,1,1,1)
		//Determine which two of (1,1,1,0), (1,1,0,1), (1,0,1,1), (0,1,1,1) are closest.
		Char   aPoint = 0x0E;
		Double aScore = xins;
		Char   bPoint = 0x0D;
		Double bScore = yins;
		if (aScore <= bScore && zins < bScore)
		{
			bScore = zins;
			bPoint = 0x0B;
		}
		else if (aScore > bScore && zins < aScore)
		{
			aScore = zins;
			aPoint = 0x0B;
		}
		if (aScore <= bScore && wins < bScore)
		{
			bScore = wins;
			bPoint = 0x07;
		}
		else if (aScore > bScore && wins < aScore)
		{
			aScore = wins;
			aPoint = 0x07;
		}

		//Now we determine the three lattice points not part of the pentachoron that may contribute.
		//This depends on the closest two pentachoron vertices, including (0,0,0,0)
		if (Double uins = 4 - inSum; uins < aScore || uins < bScore)
		{
			//(1,1,1,1) is one of the closest two pentachoron vertices.
			Char c = (bScore < aScore ? bPoint : aPoint); //Our other closest vertex is the closest out of a and b.

			if ((c & 0x01) != 0)
			{
				xsv_ext0 = xsb + 2;
				xsv_ext1 = xsv_ext2 = xsb + 1;
				dx_ext0  = dx0 - 2 - 4 * noise.squish4d;
				dx_ext1  = dx_ext2 = dx0 - 1 - 4 * noise.squish4d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb;
				dx_ext0  = dx_ext1  = dx_ext2  = dx0 - 4 * noise.squish4d;
			}

			if ((c & 0x02) != 0)
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
				dy_ext0  = dy_ext1  = dy_ext2  = dy0 - 1 - 4 * noise.squish4d;
				if ((c & 0x01) != 0)
				{
					ysv_ext1 += 1;
					dy_ext1 -= 1;
				}
				else
				{
					ysv_ext0 += 1;
					dy_ext0 -= 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
				dy_ext0  = dy_ext1  = dy_ext2  = dy0 - 4 * noise.squish4d;
			}

			if ((c & 0x04) != 0)
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
				dz_ext0  = dz_ext1  = dz_ext2  = dz0 - 1 - 4 * noise.squish4d;
				if ((c & 0x03) != 0x03)
				{
					if ((c & 0x03) == 0)
					{
						zsv_ext0 += 1;
						dz_ext0 -= 1;
					}
					else
					{
						zsv_ext1 += 1;
						dz_ext1 -= 1;
					}
				}
				else
				{
					zsv_ext2 += 1;
					dz_ext2 -= 1;
				}
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
				dz_ext0  = dz_ext1  = dz_ext2  = dz0 - 4 * noise.squish4d;
			}

			if ((c & 0x08) != 0)
			{
				wsv_ext0 = wsv_ext1 = wsb + 1;
				wsv_ext2 = wsb + 2;
				dw_ext0  = dw_ext1 = dw0 - 1 - 4 * noise.squish4d;
				dw_ext2  = dw0 - 2 - 4 * noise.squish4d;
			}
			else
			{
				wsv_ext0 = wsv_ext1 = wsv_ext2 = wsb;
				dw_ext0  = dw_ext1  = dw_ext2  = dw0 - 4 * noise.squish4d;
			}
		}
		else
		{
			//(1,1,1,1) is not one of the closest two pentachoron vertices.
			Char c = static_cast<Char>(aPoint & bPoint); //Our three extra vertices are determined by the closest two.

			if ((c & 0x01) != 0)
			{
				xsv_ext0 = xsv_ext2 = xsb + 1;
				xsv_ext1 = xsb + 2;
				dx_ext0  = dx0 - 1 - 2 * noise.squish4d;
				dx_ext1  = dx0 - 2 - 3 * noise.squish4d;
				dx_ext2  = dx0 - 1 - 3 * noise.squish4d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsv_ext2 = xsb;
				dx_ext0  = dx0 - 2 * noise.squish4d;
				dx_ext1  = dx_ext2 = dx0 - 3 * noise.squish4d;
			}

			if ((c & 0x02) != 0)
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb + 1;
				dy_ext0  = dy0 - 1 - 2 * noise.squish4d;
				dy_ext1  = dy_ext2 = dy0 - 1 - 3 * noise.squish4d;
				if ((c & 0x01) != 0)
				{
					ysv_ext2 += 1;
					dy_ext2 -= 1;
				}
				else
				{
					ysv_ext1 += 1;
					dy_ext1 -= 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysv_ext2 = ysb;
				dy_ext0  = dy0 - 2 * noise.squish4d;
				dy_ext1  = dy_ext2 = dy0 - 3 * noise.squish4d;
			}

			if ((c & 0x04) != 0)
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb + 1;
				dz_ext0  = dz0 - 1 - 2 * noise.squish4d;
				dz_ext1  = dz_ext2 = dz0 - 1 - 3 * noise.squish4d;
				if ((c & 0x03) != 0)
				{
					zsv_ext2 += 1;
					dz_ext2 -= 1;
				}
				else
				{
					zsv_ext1 += 1;
					dz_ext1 -= 1;
				}
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsv_ext2 = zsb;
				dz_ext0  = dz0 - 2 * noise.squish4d;
				dz_ext1  = dz_ext2 = dz0 - 3 * noise.squish4d;
			}

			if ((c & 0x08) != 0)
			{
				wsv_ext0 = wsv_ext1 = wsb + 1;
				wsv_ext2 = wsb + 2;
				dw_ext0  = dw0 - 1 - 2 * noise.squish4d;
				dw_ext1  = dw0 - 1 - 3 * noise.squish4d;
				dw_ext2  = dw0 - 2 - 3 * noise.squish4d;
			}
			else
			{
				wsv_ext0 = wsv_ext1 = wsv_ext2 = wsb;
				dw_ext0  = dw0 - 2 * noise.squish4d;
				dw_ext1  = dw_ext2 = dw0 - 3 * noise.squish4d;
			}
		}

		//Contribution (1,1,1,0)
		Double dx4 = dx0 - 1 - 3 * noise.squish4d;
		Double dy4 = dy0 - 1 - 3 * noise.squish4d;
		Double dz4 = dz0 - 1 - 3 * noise.squish4d;
		Double dw4 = dw0 - 3 * noise.squish4d;
		if (Double attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4; attn4 > 0)
		{
			attn4 *= attn4;
			value += attn4 * attn4 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 1, wsb + 0, dx4, dy4, dz4, dw4);
		}

		//Contribution (1,1,0,1)
		Double dx3 = dx4;
		Double dy3 = dy4;
		Double dz3 = dz0 - 3 * noise.squish4d;
		Double dw3 = dw0 - 1 - 3 * noise.squish4d;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 0, wsb + 1, dx3, dy3, dz3, dw3);
		}

		//Contribution (1,0,1,1)
		Double dx2 = dx4;
		Double dy2 = dy0 - 3 * noise.squish4d;
		Double dz2 = dz4;
		Double dw2 = dw3;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 1, wsb + 1, dx2, dy2, dz2, dw2);
		}

		//Contribution (0,1,1,1)
		Double dx1 = dx0 - 3 * noise.squish4d;
		Double dz1 = dz4;
		Double dy1 = dy4;
		Double dw1 = dw3;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 1, wsb + 1, dx1, dy1, dz1, dw1);
		}

		//Contribution (1,1,1,1)
		dx0 = dx0 - 1 - 4 * noise.squish4d;
		dy0 = dy0 - 1 - 4 * noise.squish4d;
		dz0 = dz0 - 1 - 4 * noise.squish4d;
		dw0 = dw0 - 1 - 4 * noise.squish4d;
		if (Double attn0 = 2 - dx0 * dx0 - dy0 * dy0 - dz0 * dz0 - dw0 * dw0; attn0 > 0)
		{
			attn0 *= attn0;
			value += attn0 * attn0 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 1, wsb + 1, dx0, dy0, dz0, dw0);
		}
	}
	else if (inSum <= 2)
	{
		//We're inside the first dispentachoron (Rectified 4-Simplex)
		Double aScore;
		Char   aPoint;
		Bool   aIsBiggerSide = true;
		Double bScore;
		Char   bPoint;
		Bool   bIsBiggerSide = true;

		//Decide between (1,1,0,0) and (0,0,1,1)
		if (xins + yins > zins + wins)
		{
			aScore = xins + yins;
			aPoint = 0x03;
		}
		else
		{
			aScore = zins + wins;
			aPoint = 0x0C;
		}

		//Decide between (1,0,1,0) and (0,1,0,1)
		if (xins + zins > yins + wins)
		{
			bScore = xins + zins;
			bPoint = 0x05;
		}
		else
		{
			bScore = yins + wins;
			bPoint = 0x0A;
		}

		//Closer between (1,0,0,1) and (0,1,1,0) will replace the further of a and b, if closer.
		if (xins + wins > yins + zins)
		{
			if (Double score = xins + wins; aScore >= bScore && score > bScore)
			{
				bScore = score;
				bPoint = 0x09;
			}
			else if (aScore < bScore && score > aScore)
			{
				aScore = score;
				aPoint = 0x09;
			}
		}
		else
		{
			if (Double score = yins + zins; aScore >= bScore && score > bScore)
			{
				bScore = score;
				bPoint = 0x06;
			}
			else if (aScore < bScore && score > aScore)
			{
				aScore = score;
				aPoint = 0x06;
			}
		}

		//Decide if (1,0,0,0) is closer.
		if (Double p1 = 2 - inSum + xins; aScore >= bScore && p1 > bScore)
		{
			bScore        = p1;
			bPoint        = 0x01;
			bIsBiggerSide = false;
		}
		else if (aScore < bScore && p1 > aScore)
		{
			aScore        = p1;
			aPoint        = 0x01;
			aIsBiggerSide = false;
		}

		//Decide if (0,1,0,0) is closer.
		if (Double p2 = 2 - inSum + yins; aScore >= bScore && p2 > bScore)
		{
			bScore        = p2;
			bPoint        = 0x02;
			bIsBiggerSide = false;
		}
		else if (aScore < bScore && p2 > aScore)
		{
			aScore        = p2;
			aPoint        = 0x02;
			aIsBiggerSide = false;
		}

		//Decide if (0,0,1,0) is closer.
		if (Double p3 = 2 - inSum + zins; aScore >= bScore && p3 > bScore)
		{
			bScore        = p3;
			bPoint        = 0x04;
			bIsBiggerSide = false;
		}
		else if (aScore < bScore && p3 > aScore)
		{
			aScore        = p3;
			aPoint        = 0x04;
			aIsBiggerSide = false;
		}

		//Decide if (0,0,0,1) is closer.
		if (Double p4 = 2 - inSum + wins; aScore >= bScore && p4 > bScore)
		{
			bScore        = p4;
			bPoint        = 0x08;
			bIsBiggerSide = false;
		}
		else if (aScore < bScore && p4 > aScore)
		{
			aScore        = p4;
			aPoint        = 0x08;
			aIsBiggerSide = false;
		}

		//Where each of the two closest points are determines how the extra three vertices are calculated.
		if (aIsBiggerSide == bIsBiggerSide)
		{
			if (aIsBiggerSide)
			{
				//Both closest points on the bigger side
				Char c1 = static_cast<Char>(aPoint | bPoint);
				Char c2 = static_cast<Char>(aPoint & bPoint);
				if ((c1 & 0x01) == 0)
				{
					xsv_ext0 = xsb;
					xsv_ext1 = xsb - 1;
					dx_ext0  = dx0 - 3 * noise.squish4d;
					dx_ext1  = dx0 + 1 - 2 * noise.squish4d;
				}
				else
				{
					xsv_ext0 = xsv_ext1 = xsb + 1;
					dx_ext0  = dx0 - 1 - 3 * noise.squish4d;
					dx_ext1  = dx0 - 1 - 2 * noise.squish4d;
				}

				if ((c1 & 0x02) == 0)
				{
					ysv_ext0 = ysb;
					ysv_ext1 = ysb - 1;
					dy_ext0  = dy0 - 3 * noise.squish4d;
					dy_ext1  = dy0 + 1 - 2 * noise.squish4d;
				}
				else
				{
					ysv_ext0 = ysv_ext1 = ysb + 1;
					dy_ext0  = dy0 - 1 - 3 * noise.squish4d;
					dy_ext1  = dy0 - 1 - 2 * noise.squish4d;
				}

				if ((c1 & 0x04) == 0)
				{
					zsv_ext0 = zsb;
					zsv_ext1 = zsb - 1;
					dz_ext0  = dz0 - 3 * noise.squish4d;
					dz_ext1  = dz0 + 1 - 2 * noise.squish4d;
				}
				else
				{
					zsv_ext0 = zsv_ext1 = zsb + 1;
					dz_ext0  = dz0 - 1 - 3 * noise.squish4d;
					dz_ext1  = dz0 - 1 - 2 * noise.squish4d;
				}

				if ((c1 & 0x08) == 0)
				{
					wsv_ext0 = wsb;
					wsv_ext1 = wsb - 1;
					dw_ext0  = dw0 - 3 * noise.squish4d;
					dw_ext1  = dw0 + 1 - 2 * noise.squish4d;
				}
				else
				{
					wsv_ext0 = wsv_ext1 = wsb + 1;
					dw_ext0  = dw0 - 1 - 3 * noise.squish4d;
					dw_ext1  = dw0 - 1 - 2 * noise.squish4d;
				}

				//One combination is a permutation of (0,0,0,2) based on c2
				xsv_ext2 = xsb;
				ysv_ext2 = ysb;
				zsv_ext2 = zsb;
				wsv_ext2 = wsb;
				dx_ext2  = dx0 - 2 * noise.squish4d;
				dy_ext2  = dy0 - 2 * noise.squish4d;
				dz_ext2  = dz0 - 2 * noise.squish4d;
				dw_ext2  = dw0 - 2 * noise.squish4d;
				if ((c2 & 0x01) != 0)
				{
					xsv_ext2 += 2;
					dx_ext2 -= 2;
				}
				else if ((c2 & 0x02) != 0)
				{
					ysv_ext2 += 2;
					dy_ext2 -= 2;
				}
				else if ((c2 & 0x04) != 0)
				{
					zsv_ext2 += 2;
					dz_ext2 -= 2;
				}
				else
				{
					wsv_ext2 += 2;
					dw_ext2 -= 2;
				}
			}
			else
			{
				//Both closest points on the smaller side
				//One of the two extra points is (0,0,0,0)
				xsv_ext2 = xsb;
				ysv_ext2 = ysb;
				zsv_ext2 = zsb;
				wsv_ext2 = wsb;
				dx_ext2  = dx0;
				dy_ext2  = dy0;
				dz_ext2  = dz0;
				dw_ext2  = dw0;

				//Other two points are based on the omitted axes.
				Char c = static_cast<Char>(aPoint | bPoint);

				if ((c & 0x01) == 0)
				{
					xsv_ext0 = xsb - 1;
					xsv_ext1 = xsb;
					dx_ext0  = dx0 + 1 - noise.squish4d;
					dx_ext1  = dx0 - noise.squish4d;
				}
				else
				{
					xsv_ext0 = xsv_ext1 = xsb + 1;
					dx_ext0  = dx_ext1  = dx0 - 1 - noise.squish4d;
				}

				if ((c & 0x02) == 0)
				{
					ysv_ext0 = ysv_ext1 = ysb;
					dy_ext0  = dy_ext1  = dy0 - noise.squish4d;
					if ((c & 0x01) == 0x01)
					{
						ysv_ext0 -= 1;
						dy_ext0 += 1;
					}
					else
					{
						ysv_ext1 -= 1;
						dy_ext1 += 1;
					}
				}
				else
				{
					ysv_ext0 = ysv_ext1 = ysb + 1;
					dy_ext0  = dy_ext1  = dy0 - 1 - noise.squish4d;
				}

				if ((c & 0x04) == 0)
				{
					zsv_ext0 = zsv_ext1 = zsb;
					dz_ext0  = dz_ext1  = dz0 - noise.squish4d;
					if ((c & 0x03) == 0x03)
					{
						zsv_ext0 -= 1;
						dz_ext0 += 1;
					}
					else
					{
						zsv_ext1 -= 1;
						dz_ext1 += 1;
					}
				}
				else
				{
					zsv_ext0 = zsv_ext1 = zsb + 1;
					dz_ext0  = dz_ext1  = dz0 - 1 - noise.squish4d;
				}

				if ((c & 0x08) == 0)
				{
					wsv_ext0 = wsb;
					wsv_ext1 = wsb - 1;
					dw_ext0  = dw0 - noise.squish4d;
					dw_ext1  = dw0 + 1 - noise.squish4d;
				}
				else
				{
					wsv_ext0 = wsv_ext1 = wsb + 1;
					dw_ext0  = dw_ext1  = dw0 - 1 - noise.squish4d;
				}
			}
		}
		else
		{
			//One point on each "side"
			Char c1, c2;
			if (aIsBiggerSide)
			{
				c1 = aPoint;
				c2 = bPoint;
			}
			else
			{
				c1 = bPoint;
				c2 = aPoint;
			}

			//Two contributions are the bigger-sided point with each 0 replaced with -1.
			if ((c1 & 0x01) == 0)
			{
				xsv_ext0 = xsb - 1;
				xsv_ext1 = xsb;
				dx_ext0  = dx0 + 1 - noise.squish4d;
				dx_ext1  = dx0 - noise.squish4d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsb + 1;
				dx_ext0  = dx_ext1  = dx0 - 1 - noise.squish4d;
			}

			if ((c1 & 0x02) == 0)
			{
				ysv_ext0 = ysv_ext1 = ysb;
				dy_ext0  = dy_ext1  = dy0 - noise.squish4d;
				if ((c1 & 0x01) == 0x01)
				{
					ysv_ext0 -= 1;
					dy_ext0 += 1;
				}
				else
				{
					ysv_ext1 -= 1;
					dy_ext1 += 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysb + 1;
				dy_ext0  = dy_ext1  = dy0 - 1 - noise.squish4d;
			}

			if ((c1 & 0x04) == 0)
			{
				zsv_ext0 = zsv_ext1 = zsb;
				dz_ext0  = dz_ext1  = dz0 - noise.squish4d;
				if ((c1 & 0x03) == 0x03)
				{
					zsv_ext0 -= 1;
					dz_ext0 += 1;
				}
				else
				{
					zsv_ext1 -= 1;
					dz_ext1 += 1;
				}
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsb + 1;
				dz_ext0  = dz_ext1  = dz0 - 1 - noise.squish4d;
			}

			if ((c1 & 0x08) == 0)
			{
				wsv_ext0 = wsb;
				wsv_ext1 = wsb - 1;
				dw_ext0  = dw0 - noise.squish4d;
				dw_ext1  = dw0 + 1 - noise.squish4d;
			}
			else
			{
				wsv_ext0 = wsv_ext1 = wsb + 1;
				dw_ext0  = dw_ext1  = dw0 - 1 - noise.squish4d;
			}

			//One contribution is a permutation of (0,0,0,2) based on the smaller-sided point
			xsv_ext2 = xsb;
			ysv_ext2 = ysb;
			zsv_ext2 = zsb;
			wsv_ext2 = wsb;
			dx_ext2  = dx0 - 2 * noise.squish4d;
			dy_ext2  = dy0 - 2 * noise.squish4d;
			dz_ext2  = dz0 - 2 * noise.squish4d;
			dw_ext2  = dw0 - 2 * noise.squish4d;
			if ((c2 & 0x01) != 0)
			{
				xsv_ext2 += 2;
				dx_ext2 -= 2;
			}
			else if ((c2 & 0x02) != 0)
			{
				ysv_ext2 += 2;
				dy_ext2 -= 2;
			}
			else if ((c2 & 0x04) != 0)
			{
				zsv_ext2 += 2;
				dz_ext2 -= 2;
			}
			else
			{
				wsv_ext2 += 2;
				dw_ext2 -= 2;
			}
		}

		//Contribution (1,0,0,0)
		Double dx1 = dx0 - 1 - noise.squish4d;
		Double dy1 = dy0 - 0 - noise.squish4d;
		Double dz1 = dz0 - 0 - noise.squish4d;
		Double dw1 = dw0 - 0 - noise.squish4d;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 0, wsb + 0, dx1, dy1, dz1, dw1);
		}

		//Contribution (0,1,0,0)
		Double dx2 = dx0 - 0 - noise.squish4d;
		Double dy2 = dy0 - 1 - noise.squish4d;
		Double dz2 = dz1;
		Double dw2 = dw1;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 0, wsb + 0, dx2, dy2, dz2, dw2);
		}

		//Contribution (0,0,1,0)
		Double dx3 = dx2;
		Double dy3 = dy1;
		Double dz3 = dz0 - 1 - noise.squish4d;
		Double dw3 = dw1;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 1, wsb + 0, dx3, dy3, dz3, dw3);
		}

		//Contribution (0,0,0,1)
		Double dx4 = dx2;
		Double dy4 = dy1;
		Double dz4 = dz1;
		Double dw4 = dw0 - 1 - noise.squish4d;
		if (Double attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4; attn4 > 0)
		{
			attn4 *= attn4;
			value += attn4 * attn4 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 0, wsb + 1, dx4, dy4, dz4, dw4);
		}

		//Contribution (1,1,0,0)
		Double dx5 = dx0 - 1 - 2 * noise.squish4d;
		Double dy5 = dy0 - 1 - 2 * noise.squish4d;
		Double dz5 = dz0 - 0 - 2 * noise.squish4d;
		Double dw5 = dw0 - 0 - 2 * noise.squish4d;
		if (Double attn5 = 2 - dx5 * dx5 - dy5 * dy5 - dz5 * dz5 - dw5 * dw5; attn5 > 0)
		{
			attn5 *= attn5;
			value += attn5 * attn5 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 0, wsb + 0, dx5, dy5, dz5, dw5);
		}

		//Contribution (1,0,1,0)
		Double dx6 = dx0 - 1 - 2 * noise.squish4d;
		Double dy6 = dy0 - 0 - 2 * noise.squish4d;
		Double dz6 = dz0 - 1 - 2 * noise.squish4d;
		Double dw6 = dw0 - 0 - 2 * noise.squish4d;
		if (Double attn6 = 2 - dx6 * dx6 - dy6 * dy6 - dz6 * dz6 - dw6 * dw6; attn6 > 0)
		{
			attn6 *= attn6;
			value += attn6 * attn6 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 1, wsb + 0, dx6, dy6, dz6, dw6);
		}

		//Contribution (1,0,0,1)
		Double dx7 = dx0 - 1 - 2 * noise.squish4d;
		Double dy7 = dy0 - 0 - 2 * noise.squish4d;
		Double dz7 = dz0 - 0 - 2 * noise.squish4d;
		Double dw7 = dw0 - 1 - 2 * noise.squish4d;
		if (Double attn7 = 2 - dx7 * dx7 - dy7 * dy7 - dz7 * dz7 - dw7 * dw7; attn7 > 0)
		{
			attn7 *= attn7;
			value += attn7 * attn7 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 0, wsb + 1, dx7, dy7, dz7, dw7);
		}

		//Contribution (0,1,1,0)
		Double dx8 = dx0 - 0 - 2 * noise.squish4d;
		Double dy8 = dy0 - 1 - 2 * noise.squish4d;
		Double dz8 = dz0 - 1 - 2 * noise.squish4d;
		Double dw8 = dw0 - 0 - 2 * noise.squish4d;
		if (Double attn8 = 2 - dx8 * dx8 - dy8 * dy8 - dz8 * dz8 - dw8 * dw8; attn8 > 0)
		{
			attn8 *= attn8;
			value += attn8 * attn8 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 1, wsb + 0, dx8, dy8, dz8, dw8);
		}

		//Contribution (0,1,0,1)
		Double dx9 = dx0 - 0 - 2 * noise.squish4d;
		Double dy9 = dy0 - 1 - 2 * noise.squish4d;
		Double dz9 = dz0 - 0 - 2 * noise.squish4d;
		Double dw9 = dw0 - 1 - 2 * noise.squish4d;
		if (Double attn9 = 2 - dx9 * dx9 - dy9 * dy9 - dz9 * dz9 - dw9 * dw9; attn9 > 0)
		{
			attn9 *= attn9;
			value += attn9 * attn9 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 0, wsb + 1, dx9, dy9, dz9, dw9);
		}

		//Contribution (0,0,1,1)
		Double dx10 = dx0 - 0 - 2 * noise.squish4d;
		Double dy10 = dy0 - 0 - 2 * noise.squish4d;
		Double dz10 = dz0 - 1 - 2 * noise.squish4d;
		Double dw10 = dw0 - 1 - 2 * noise.squish4d;
		if (Double attn10 = 2 - dx10 * dx10 - dy10 * dy10 - dz10 * dz10 - dw10 * dw10; attn10 > 0)
		{
			attn10 *= attn10;
			value += attn10 * attn10 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 1, wsb + 1, dx10, dy10, dz10, dw10);
		}
	}
	else
	{
		//We're inside the second dispentachoron (Rectified 4-Simplex)
		Double aScore;
		Char   aPoint;
		Bool   aIsBiggerSide = true;
		Double bScore;
		Char   bPoint;
		Bool   bIsBiggerSide = true;

		//Decide between (0,0,1,1) and (1,1,0,0)
		if (xins + yins < zins + wins)
		{
			aScore = xins + yins;
			aPoint = 0x0C;
		}
		else
		{
			aScore = zins + wins;
			aPoint = 0x03;
		}

		//Decide between (0,1,0,1) and (1,0,1,0)
		if (xins + zins < yins + wins)
		{
			bScore = xins + zins;
			bPoint = 0x0A;
		}
		else
		{
			bScore = yins + wins;
			bPoint = 0x05;
		}

		//Closer between (0,1,1,0) and (1,0,0,1) will replace the further of a and b, if closer.
		if (xins + wins < yins + zins)
		{
			if (Double score = xins + wins; aScore <= bScore && score < bScore)
			{
				bScore = score;
				bPoint = 0x06;
			}
			else if (aScore > bScore && score < aScore)
			{
				aScore = score;
				aPoint = 0x06;
			}
		}
		else
		{
			if (Double score = yins + zins; aScore <= bScore && score < bScore)
			{
				bScore = score;
				bPoint = 0x09;
			}
			else if (aScore > bScore && score < aScore)
			{
				aScore = score;
				aPoint = 0x09;
			}
		}

		//Decide if (0,1,1,1) is closer.
		if (Double p1 = 3 - inSum + xins; aScore <= bScore && p1 < bScore)
		{
			bScore        = p1;
			bPoint        = 0x0E;
			bIsBiggerSide = false;
		}
		else if (aScore > bScore && p1 < aScore)
		{
			aScore        = p1;
			aPoint        = 0x0E;
			aIsBiggerSide = false;
		}

		//Decide if (1,0,1,1) is closer.
		if (Double p2 = 3 - inSum + yins; aScore <= bScore && p2 < bScore)
		{
			bScore        = p2;
			bPoint        = 0x0D;
			bIsBiggerSide = false;
		}
		else if (aScore > bScore && p2 < aScore)
		{
			aScore        = p2;
			aPoint        = 0x0D;
			aIsBiggerSide = false;
		}

		//Decide if (1,1,0,1) is closer.
		if (Double p3 = 3 - inSum + zins; aScore <= bScore && p3 < bScore)
		{
			bScore        = p3;
			bPoint        = 0x0B;
			bIsBiggerSide = false;
		}
		else if (aScore > bScore && p3 < aScore)
		{
			aScore        = p3;
			aPoint        = 0x0B;
			aIsBiggerSide = false;
		}

		//Decide if (1,1,1,0) is closer.
		if (Double p4 = 3 - inSum + wins; aScore <= bScore && p4 < bScore)
		{
			bScore        = p4;
			bPoint        = 0x07;
			bIsBiggerSide = false;
		}
		else if (aScore > bScore && p4 < aScore)
		{
			aScore        = p4;
			aPoint        = 0x07;
			aIsBiggerSide = false;
		}

		//Where each of the two closest points are determines how the extra three vertices are calculated.
		if (aIsBiggerSide == bIsBiggerSide)
		{
			if (aIsBiggerSide)
			{
				//Both closest points on the bigger side
				Char c1 = static_cast<Char>(aPoint & bPoint);
				Char c2 = static_cast<Char>(aPoint | bPoint);

				//Two contributions are permutations of (0,0,0,1) and (0,0,0,2) based on c1
				xsv_ext0 = xsv_ext1 = xsb;
				ysv_ext0 = ysv_ext1 = ysb;
				zsv_ext0 = zsv_ext1 = zsb;
				wsv_ext0 = wsv_ext1 = wsb;
				dx_ext0  = dx0 - noise.squish4d;
				dy_ext0  = dy0 - noise.squish4d;
				dz_ext0  = dz0 - noise.squish4d;
				dw_ext0  = dw0 - noise.squish4d;
				dx_ext1  = dx0 - 2 * noise.squish4d;
				dy_ext1  = dy0 - 2 * noise.squish4d;
				dz_ext1  = dz0 - 2 * noise.squish4d;
				dw_ext1  = dw0 - 2 * noise.squish4d;
				if ((c1 & 0x01) != 0)
				{
					xsv_ext0 += 1;
					dx_ext0 -= 1;
					xsv_ext1 += 2;
					dx_ext1 -= 2;
				}
				else if ((c1 & 0x02) != 0)
				{
					ysv_ext0 += 1;
					dy_ext0 -= 1;
					ysv_ext1 += 2;
					dy_ext1 -= 2;
				}
				else if ((c1 & 0x04) != 0)
				{
					zsv_ext0 += 1;
					dz_ext0 -= 1;
					zsv_ext1 += 2;
					dz_ext1 -= 2;
				}
				else
				{
					wsv_ext0 += 1;
					dw_ext0 -= 1;
					wsv_ext1 += 2;
					dw_ext1 -= 2;
				}

				//One contribution is a permutation of (1,1,1,-1) based on c2
				xsv_ext2 = xsb + 1;
				ysv_ext2 = ysb + 1;
				zsv_ext2 = zsb + 1;
				wsv_ext2 = wsb + 1;
				dx_ext2  = dx0 - 1 - 2 * noise.squish4d;
				dy_ext2  = dy0 - 1 - 2 * noise.squish4d;
				dz_ext2  = dz0 - 1 - 2 * noise.squish4d;
				dw_ext2  = dw0 - 1 - 2 * noise.squish4d;
				if ((c2 & 0x01) == 0)
				{
					xsv_ext2 -= 2;
					dx_ext2 += 2;
				}
				else if ((c2 & 0x02) == 0)
				{
					ysv_ext2 -= 2;
					dy_ext2 += 2;
				}
				else if ((c2 & 0x04) == 0)
				{
					zsv_ext2 -= 2;
					dz_ext2 += 2;
				}
				else
				{
					wsv_ext2 -= 2;
					dw_ext2 += 2;
				}
			}
			else
			{
				//Both closest points on the smaller side
				//One of the two extra points is (1,1,1,1)
				xsv_ext2 = xsb + 1;
				ysv_ext2 = ysb + 1;
				zsv_ext2 = zsb + 1;
				wsv_ext2 = wsb + 1;
				dx_ext2  = dx0 - 1 - 4 * noise.squish4d;
				dy_ext2  = dy0 - 1 - 4 * noise.squish4d;
				dz_ext2  = dz0 - 1 - 4 * noise.squish4d;
				dw_ext2  = dw0 - 1 - 4 * noise.squish4d;

				//Other two points are based on the shared axes.
				Char c = static_cast<Char>(aPoint & bPoint);

				if ((c & 0x01) != 0)
				{
					xsv_ext0 = xsb + 2;
					xsv_ext1 = xsb + 1;
					dx_ext0  = dx0 - 2 - 3 * noise.squish4d;
					dx_ext1  = dx0 - 1 - 3 * noise.squish4d;
				}
				else
				{
					xsv_ext0 = xsv_ext1 = xsb;
					dx_ext0  = dx_ext1  = dx0 - 3 * noise.squish4d;
				}

				if ((c & 0x02) != 0)
				{
					ysv_ext0 = ysv_ext1 = ysb + 1;
					dy_ext0  = dy_ext1  = dy0 - 1 - 3 * noise.squish4d;
					if ((c & 0x01) == 0)
					{
						ysv_ext0 += 1;
						dy_ext0 -= 1;
					}
					else
					{
						ysv_ext1 += 1;
						dy_ext1 -= 1;
					}
				}
				else
				{
					ysv_ext0 = ysv_ext1 = ysb;
					dy_ext0  = dy_ext1  = dy0 - 3 * noise.squish4d;
				}

				if ((c & 0x04) != 0)
				{
					zsv_ext0 = zsv_ext1 = zsb + 1;
					dz_ext0  = dz_ext1  = dz0 - 1 - 3 * noise.squish4d;
					if ((c & 0x03) == 0)
					{
						zsv_ext0 += 1;
						dz_ext0 -= 1;
					}
					else
					{
						zsv_ext1 += 1;
						dz_ext1 -= 1;
					}
				}
				else
				{
					zsv_ext0 = zsv_ext1 = zsb;
					dz_ext0  = dz_ext1  = dz0 - 3 * noise.squish4d;
				}

				if ((c & 0x08) != 0)
				{
					wsv_ext0 = wsb + 1;
					wsv_ext1 = wsb + 2;
					dw_ext0  = dw0 - 1 - 3 * noise.squish4d;
					dw_ext1  = dw0 - 2 - 3 * noise.squish4d;
				}
				else
				{
					wsv_ext0 = wsv_ext1 = wsb;
					dw_ext0  = dw_ext1  = dw0 - 3 * noise.squish4d;
				}
			}
		}
		else
		{
			//One point on each "side"
			Char c1, c2;
			if (aIsBiggerSide)
			{
				c1 = aPoint;
				c2 = bPoint;
			}
			else
			{
				c1 = bPoint;
				c2 = aPoint;
			}

			//Two contributions are the bigger-sided point with each 1 replaced with 2.
			if ((c1 & 0x01) != 0)
			{
				xsv_ext0 = xsb + 2;
				xsv_ext1 = xsb + 1;
				dx_ext0  = dx0 - 2 - 3 * noise.squish4d;
				dx_ext1  = dx0 - 1 - 3 * noise.squish4d;
			}
			else
			{
				xsv_ext0 = xsv_ext1 = xsb;
				dx_ext0  = dx_ext1  = dx0 - 3 * noise.squish4d;
			}

			if ((c1 & 0x02) != 0)
			{
				ysv_ext0 = ysv_ext1 = ysb + 1;
				dy_ext0  = dy_ext1  = dy0 - 1 - 3 * noise.squish4d;
				if ((c1 & 0x01) == 0)
				{
					ysv_ext0 += 1;
					dy_ext0 -= 1;
				}
				else
				{
					ysv_ext1 += 1;
					dy_ext1 -= 1;
				}
			}
			else
			{
				ysv_ext0 = ysv_ext1 = ysb;
				dy_ext0  = dy_ext1  = dy0 - 3 * noise.squish4d;
			}

			if ((c1 & 0x04) != 0)
			{
				zsv_ext0 = zsv_ext1 = zsb + 1;
				dz_ext0  = dz_ext1  = dz0 - 1 - 3 * noise.squish4d;
				if ((c1 & 0x03) == 0)
				{
					zsv_ext0 += 1;
					dz_ext0 -= 1;
				}
				else
				{
					zsv_ext1 += 1;
					dz_ext1 -= 1;
				}
			}
			else
			{
				zsv_ext0 = zsv_ext1 = zsb;
				dz_ext0  = dz_ext1  = dz0 - 3 * noise.squish4d;
			}

			if ((c1 & 0x08) != 0)
			{
				wsv_ext0 = wsb + 1;
				wsv_ext1 = wsb + 2;
				dw_ext0  = dw0 - 1 - 3 * noise.squish4d;
				dw_ext1  = dw0 - 2 - 3 * noise.squish4d;
			}
			else
			{
				wsv_ext0 = wsv_ext1 = wsb;
				dw_ext0  = dw_ext1  = dw0 - 3 * noise.squish4d;
			}

			//One contribution is a permutation of (1,1,1,-1) based on the smaller-sided point
			xsv_ext2 = xsb + 1;
			ysv_ext2 = ysb + 1;
			zsv_ext2 = zsb + 1;
			wsv_ext2 = wsb + 1;
			dx_ext2  = dx0 - 1 - 2 * noise.squish4d;
			dy_ext2  = dy0 - 1 - 2 * noise.squish4d;
			dz_ext2  = dz0 - 1 - 2 * noise.squish4d;
			dw_ext2  = dw0 - 1 - 2 * noise.squish4d;
			if ((c2 & 0x01) == 0)
			{
				xsv_ext2 -= 2;
				dx_ext2 += 2;
			}
			else if ((c2 & 0x02) == 0)
			{
				ysv_ext2 -= 2;
				dy_ext2 += 2;
			}
			else if ((c2 & 0x04) == 0)
			{
				zsv_ext2 -= 2;
				dz_ext2 += 2;
			}
			else
			{
				wsv_ext2 -= 2;
				dw_ext2 += 2;
			}
		}

		//Contribution (1,1,1,0)
		Double dx4 = dx0 - 1 - 3 * noise.squish4d;
		Double dy4 = dy0 - 1 - 3 * noise.squish4d;
		Double dz4 = dz0 - 1 - 3 * noise.squish4d;
		Double dw4 = dw0 - 3 * noise.squish4d;
		if (Double attn4 = 2 - dx4 * dx4 - dy4 * dy4 - dz4 * dz4 - dw4 * dw4; attn4 > 0)
		{
			attn4 *= attn4;
			value += attn4 * attn4 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 1, wsb + 0, dx4, dy4, dz4, dw4);
		}

		//Contribution (1,1,0,1)
		Double dx3 = dx4;
		Double dy3 = dy4;
		Double dz3 = dz0 - 3 * noise.squish4d;
		Double dw3 = dw0 - 1 - 3 * noise.squish4d;
		if (Double attn3 = 2 - dx3 * dx3 - dy3 * dy3 - dz3 * dz3 - dw3 * dw3; attn3 > 0)
		{
			attn3 *= attn3;
			value += attn3 * attn3 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 0, wsb + 1, dx3, dy3, dz3, dw3);
		}

		//Contribution (1,0,1,1)
		Double dx2 = dx4;
		Double dy2 = dy0 - 3 * noise.squish4d;
		Double dz2 = dz4;
		Double dw2 = dw3;
		if (Double attn2 = 2 - dx2 * dx2 - dy2 * dy2 - dz2 * dz2 - dw2 * dw2; attn2 > 0)
		{
			attn2 *= attn2;
			value += attn2 * attn2 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 1, wsb + 1, dx2, dy2, dz2, dw2);
		}

		//Contribution (0,1,1,1)
		Double dx1 = dx0 - 3 * noise.squish4d;
		Double dz1 = dz4;
		Double dy1 = dy4;
		Double dw1 = dw3;
		if (Double attn1 = 2 - dx1 * dx1 - dy1 * dy1 - dz1 * dz1 - dw1 * dw1; attn1 > 0)
		{
			attn1 *= attn1;
			value += attn1 * attn1 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 1, wsb + 1, dx1, dy1, dz1, dw1);
		}

		//Contribution (1,1,0,0)
		Double dx5 = dx0 - 1 - 2 * noise.squish4d;
		Double dy5 = dy0 - 1 - 2 * noise.squish4d;
		Double dz5 = dz0 - 0 - 2 * noise.squish4d;
		Double dw5 = dw0 - 0 - 2 * noise.squish4d;
		if (Double attn5 = 2 - dx5 * dx5 - dy5 * dy5 - dz5 * dz5 - dw5 * dw5; attn5 > 0)
		{
			attn5 *= attn5;
			value += attn5 * attn5 * noise.extrapolate(xsb + 1, ysb + 1, zsb + 0, wsb + 0, dx5, dy5, dz5, dw5);
		}

		//Contribution (1,0,1,0)
		Double dx6 = dx0 - 1 - 2 * noise.squish4d;
		Double dy6 = dy0 - 0 - 2 * noise.squish4d;
		Double dz6 = dz0 - 1 - 2 * noise.squish4d;
		Double dw6 = dw0 - 0 - 2 * noise.squish4d;
		if (Double attn6 = 2 - dx6 * dx6 - dy6 * dy6 - dz6 * dz6 - dw6 * dw6; attn6 > 0)
		{
			attn6 *= attn6;
			value += attn6 * attn6 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 1, wsb + 0, dx6, dy6, dz6, dw6);
		}

		//Contribution (1,0,0,1)
		Double dx7 = dx0 - 1 - 2 * noise.squish4d;
		Double dy7 = dy0 - 0 - 2 * noise.squish4d;
		Double dz7 = dz0 - 0 - 2 * noise.squish4d;
		Double dw7 = dw0 - 1 - 2 * noise.squish4d;
		if (Double attn7 = 2 - dx7 * dx7 - dy7 * dy7 - dz7 * dz7 - dw7 * dw7; attn7 > 0)
		{
			attn7 *= attn7;
			value += attn7 * attn7 * noise.extrapolate(xsb + 1, ysb + 0, zsb + 0, wsb + 1, dx7, dy7, dz7, dw7);
		}

		//Contribution (0,1,1,0)
		Double dx8 = dx0 - 0 - 2 * noise.squish4d;
		Double dy8 = dy0 - 1 - 2 * noise.squish4d;
		Double dz8 = dz0 - 1 - 2 * noise.squish4d;
		Double dw8 = dw0 - 0 - 2 * noise.squish4d;
		if (Double attn8 = 2 - dx8 * dx8 - dy8 * dy8 - dz8 * dz8 - dw8 * dw8; attn8 > 0)
		{
			attn8 *= attn8;
			value += attn8 * attn8 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 1, wsb + 0, dx8, dy8, dz8, dw8);
		}

		//Contribution (0,1,0,1)
		Double dx9 = dx0 - 0 - 2 * noise.squish4d;
		Double dy9 = dy0 - 1 - 2 * noise.squish4d;
		Double dz9 = dz0 - 0 - 2 * noise.squish4d;
		Double dw9 = dw0 - 1 - 2 * noise.squish4d;
		if (Double attn9 = 2 - dx9 * dx9 - dy9 * dy9 - dz9 * dz9 - dw9 * dw9; attn9 > 0)
		{
			attn9 *= attn9;
			value += attn9 * attn9 * noise.extrapolate(xsb + 0, ysb + 1, zsb + 0, wsb + 1, dx9, dy9, dz9, dw9);
		}

		//Contribution (0,0,1,1)
		Double dx10 = dx0 - 0 - 2 * noise.squish4d;
		Double dy10 = dy0 - 0 - 2 * noise.squish4d;
		Double dz10 = dz0 - 1 - 2 * noise.squish4d;
		Double dw10 = dw0 - 1 - 2 * noise.squish4d;
		if (Double attn10 = 2 - dx10 * dx10 - dy10 * dy10 - dz10 * dz10 - dw10 * dw10; attn10 > 0)
		{
			attn10 *= attn10;
			value += attn10 * attn10 * noise.extrapolate(xsb + 0, ysb + 0, zsb + 1, wsb + 1, dx10, dy10, dz10, dw10);
		}
	}

	//First extra vertex
	if (Double attn_ext0 = 2 - dx_ext0 * dx_ext0 - dy_ext0 * dy_ext0 - dz_ext0 * dz_ext0 - dw_ext0 * dw_ext0; attn_ext0 > 0)
	{
		attn_ext0 *= attn_ext0;
		value += attn_ext0 * attn_ext0 * noise.extrapolate(xsv_ext0, ysv_ext0, zsv_ext0, wsv_ext0, dx_ext0, dy_ext0, dz_ext0, dw_ext0);
	}

	//Second extra vertex
	if (Double attn_ext1 = 2 - dx_ext1 * dx_ext1 - dy_ext1 * dy_ext1 - dz_ext1 * dz_ext1 - dw_ext1 * dw_ext1; attn_ext1 > 0)
	{
		attn_ext1 *= attn_ext1;
		value += attn_ext1 * attn_ext1 * noise.extrapolate(xsv_ext1, ysv_ext1, zsv_ext1, wsv_ext1, dx_ext1, dy_ext1, dz_ext1, dw_ext1);
	}

	//Third extra vertex
	if (Double attn_ext2 = 2 - dx_ext2 * dx_ext2 - dy_ext2 * dy_ext2 - dz_ext2 * dz_ext2 - dw_ext2 * dw_ext2; attn_ext2 > 0)
	{
		attn_ext2 *= attn_ext2;
		value += attn_ext2 * attn_ext2 * noise.extrapolate(xsv_ext2, ysv_ext2, zsv_ext2, wsv_ext2, dx_ext2, dy_ext2, dz_ext2, dw_ext2);
	}

	return value / noise.norm4d;
}

Double simplexOctaveNoise(Double x, Double y, Double z, Double w, Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;

	for (Int i = 0; i < octaves; ++i)
	{
		result += simplexNoise(x, y, z, w) * amp;
		x *= 2.0;
		y *= 2.0;
		z *= 2.0;
		w *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double simplexOctaveNoise(Double x, Double y, Double z, Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;

	for (Int i = 0; i < octaves; ++i)
	{
		result += simplexNoise(x, y, z) * amp;
		x *= 2.0;
		y *= 2.0;
		z *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double simplexOctaveNoise(Double x, Double y, Int octaves)
{
	Double result = 0.0;
	Double amp    = 1.0;

	for (Int i = 0; i < octaves; ++i)
	{
		result += simplexNoise(x, y) * amp;
		x *= 2.0;
		y *= 2.0;
		amp *= 0.5;
	}

	return result;
}

Double SimplexNoise::extrapolate(const Int xsb, const Int ysb, const Double dx, const Double dy) const
{
	const Int index = perm[(perm[xsb & 0xFF] + ysb) & 0xFF] & 0x0E;
	return gradients2d[index] * dx + gradients2d[index + 1] * dy;
}

Double SimplexNoise::extrapolate(const Int xsb, const Int ysb, const Int zsb, const Double dx, const Double dy, const Double dz) const
{
	const Int index = permGradIndex3d[(perm[(perm[xsb & 0xFF] + ysb) & 0xFF] + zsb) & 0xFF];
	return gradients3d[index] * dx + gradients3d[index + 1] * dy + gradients3d[index + 2] * dz;
}

Double SimplexNoise::extrapolate(const Int xsb, const Int ysb, const Int zsb, const Int wsb, const Double dx, const Double dy, const Double dz, const Double dw) const
{
	const Int index = perm[(perm[(perm[(perm[xsb & 0xFF] + ysb) & 0xFF] + zsb) & 0xFF] + wsb) & 0xFF] & 0xFC;
	return gradients4d[index] * dx + gradients4d[index + 1] * dy + gradients4d[index + 2] * dz + gradients4d[index + 3] * dw;
}
