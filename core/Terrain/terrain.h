#ifndef  TERRAIN_H
#define TERRAIN_H
#pragma once
#include "..\ew\mesh.h"


namespace ew {
	void createTerrain(float width, float height, int subDivisions, MeshData* meshData, int type);
	glm::vec3 getNormal(float width, float height, int subDivisions, int row, int col, int type);
	glm::vec3 getTangent(float width, float height, int subDivisions, int row, int col, glm::vec3 normal, int type);
	float makeDune(int col, int row, int type);
	float edgeSmoother(int col, int row, int subDivisions, int type);
}




#endif // ! TERRAIN_H
