/*
	Author: William Bishop
*/
#include "terrain.h"
#include <stdlib.h>
#include <functional>
namespace ew {
	/// <summary>
		/// Creates a subdivided plane along X/Y axes
		/// </summary>
		/// <param name="width">Total width</param>
		/// <param name="height">Total height</param>
		/// <param name="subDivisions">Number of subdivisions</param>
		/// <returns></returns>
	void createTerrain(float width, float height, int subDivisions, MeshData* mesh, int type) {
		mesh->vertices.clear();
		mesh->indices.clear();
		mesh->vertices.reserve((subDivisions + 1) * (subDivisions + 1));
		mesh->indices.reserve(subDivisions * subDivisions * 6);

		for (size_t row = 0; row <= subDivisions; row++)
		{
			for (size_t col = 0; col <= subDivisions; col++)
			{
				glm::vec2 uv;
				uv.x = ((float)col / subDivisions);
				uv.y = ((float)row / subDivisions);
				glm::vec3 pos;
				pos.x = uv.x * width;
				pos.z = uv.y * height * -1;
				pos.y = edgeSmoother(col, row,subDivisions, type);



				glm::vec3 normal = getNormal(width, height, subDivisions, row, col, type);
				glm::vec3 tangent = getTangent(width, height, subDivisions, row, col, normal, type);
				mesh->vertices.emplace_back(pos, normal, uv, tangent);
			}
		}

		//Indices and tangents/bitangents
		for (size_t row = 0; row < subDivisions; row++)
		{
			for (size_t col = 0; col < subDivisions; col++)
			{
				unsigned int bl = row * (subDivisions + 1) + col;
				unsigned int br = bl + 1;
				unsigned int tl = bl + subDivisions + 1;
				unsigned int tr = tl + 1;

				//Triangle 1
				mesh->indices.emplace_back(bl);
				mesh->indices.emplace_back(br);
				mesh->indices.emplace_back(tr);

				//Triangle 2
				mesh->indices.emplace_back(tr);
				mesh->indices.emplace_back(tl);
				mesh->indices.emplace_back(bl);
			}
		}

		return;
	}
	glm::vec3 getNormal(float width, float height, int subDivisions, int row, int col, int type) {
		
		glm::vec3 posA;
		posA.x = ((float)col / subDivisions) * width;
		posA.y = edgeSmoother(col, row, subDivisions, type); 
		posA.z = ((float)row / subDivisions) * height * -1;
		glm::vec3 posB;
		posB.x = (((float)col + 1.0f) / subDivisions) * width;
		posB.y = edgeSmoother(col+1, row, subDivisions, type);
		posB.z = ((float)row / subDivisions) * height * -1;
		glm::vec3 posC;
		posC.x = ((float)col / subDivisions) * width;
		posC.y = edgeSmoother(col, row+1, subDivisions, type);
		posC.z = (((float)row + 1.0f)/ subDivisions) * height * -1;

		glm::vec3 vA = posB - posA;
		glm::vec3 vB = posC - posA;

		return glm::cross(vA,vB);
	}
	glm::vec3 getTangent(float width, float height, int subDivisions, int row, int col, glm::vec3 normal, int type) {
		
		glm::vec3 posA;
		posA.x = ((float)col / subDivisions) * width;
		posA.y = edgeSmoother(col, row, subDivisions, type);
		posA.z = ((float)row / subDivisions) * height * -1;
		glm::vec3 posB;
		posB.x = (((float)col + 1.0f) / subDivisions) * width;
		posB.y = edgeSmoother(col+1, row, subDivisions, type);
		posB.z = ((float)row / subDivisions) * height * -1;

		glm::vec3 vA = posB - posA;

		return glm::cross(vA,normal);
	}
	float makeDune(int col, int row, int type = 0) {
		if (type == 0) {
			return abs(sinf(sqrt(col))) + cosf(sqrt(row)) + cosf(sqrt(col)) * 3;
		}
		else if (type == 1) {
			return cosf(col/PI) + sinf(sqrt(col)/18) + floor(row/10)/8;

		}
		else if (type == 2) {
			return sin(col/(3*PI/2));

		}
		else if (type == 3) {
			return cos(sqrt(row)) + abs(sin(sqrt(row+col)));

		}
		else if (type == 4) {
			return cos(sqrt(col)) + abs(sin(sqrt(row+col)));

		}
		return sin(col) + sin(row);
		
		
	}
	float edgeSmoother(int col, int row,int subDivisions, int type = 0) {
		float H=0;
		float J = PI;
		float x = col, y = row;
		float lowerBound = (float)subDivisions / 5.0;
		float uppperBound = (float)subDivisions - ((float)subDivisions / 10.0);
		int G = 6;
		H = makeDune(col, row, type);
		//if (x < 7 || y < 7 || x > 65 || y>65) {
		//	if (sin((float)x/ ((float)subDivisions / J)) * G > sin((float)y / ((float)subDivisions / J)) * G) {
		//		
		//		H = sin((float)y / ((float)subDivisions/J)) * G - 6;
		//	}
		//	else {
		//		H = sin((float)x / ((float)subDivisions / J)) * G - 6;
		//	}
		//}
		if (x < lowerBound) {
			if (y > lowerBound) {
				float Joe = makeDune(lowerBound, y, type);
				H = cos(x / ((3.0*PI)/2.0)) * Joe - abs(Joe);
				
				//H = Joe;
			}
			else if (y < lowerBound){
				float Joe = makeDune(lowerBound, lowerBound, type);
				if (cos(y / ((3.0 * PI) / 2.0)) * Joe - abs(Joe) > cos(x / ((3.0 * PI) / 2.0)) * Joe - abs(Joe)) {

					H = cos(x / ((3.0 * PI) / 2.0)) * Joe - abs(Joe);
				}
				else {
					H = cos(y / ((3.0 * PI) / 2.0)) * Joe - abs(Joe);
				}
			}
		}
		else if (y < lowerBound) {
			float Joe = makeDune(x, lowerBound, type);
			H = sin(y / ((3.0*PI)/2.0)) * Joe;
		}
		else if (x > uppperBound) {
			if (y < uppperBound) {
				float Joe = makeDune(uppperBound, y, type);
				H = sin(x / ((3.0*PI)/2.0)) * Joe;
				
				//H = Joe;
			}
			else if (y > uppperBound){
				float Joe = makeDune(uppperBound, uppperBound, type);
				if (cos(y / ((3.0 * PI) / 2.0)) * Joe - abs(Joe) > cos(x / ((3.0 * PI) / 2.0)) * Joe - abs(Joe)) {

					H = cos(x / ((3.0 * PI) / 2.0)) * Joe - abs(Joe);
				}
				else {
					H = cos(x / ((3.0 * PI) / 2.0)) * Joe - abs(Joe);
				}
			}
		}
		else if (y > uppperBound) {
			float Joe = makeDune(x, uppperBound, type);
			H = sin(y / ((3.0*PI)/2.0)) * Joe;
		}


		return H;
	}
}