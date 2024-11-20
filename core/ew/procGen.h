/*
	Author: Eric Winebrenner
*/


#pragma once
#include "mesh.h"

namespace ew {
	void averageOrthognals(MeshData* meshData);
	void createCube(float size, MeshData* meshData);
	void createPlaneXY(float width, float height, int subDivisions, MeshData* meshData);
	void createSphere(float radius, int subDivisions, MeshData* meshData);
}
