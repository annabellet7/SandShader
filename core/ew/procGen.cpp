/*
	Author: Eric Winebrenner
*/


#include "procGen.h"
#include <stdlib.h>

namespace ew {
	void averageOrthognals(MeshData* mesh)
	{
		for (size_t i = 0; i < mesh->indices.size() - 1; i++)
		{
			if (mesh->vertices[mesh->indices[i]].avTangent == glm::vec3(0.0f))
			{
				mesh->vertices[mesh->indices[i]].avTangent = mesh->tangents[i];
				mesh->vertices[mesh->indices[i]].avBitangent = mesh->bitangents[i];
			}
			for (size_t j = i + 1; j < mesh->indices.size(); j++)
			{
				if (mesh->vertices[mesh->indices[i]].pos == mesh->vertices[mesh->indices[j]].pos &&
					mesh->vertices[mesh->indices[i]].uv == mesh->vertices[mesh->indices[j]].uv &&
					mesh->vertices[mesh->indices[i]].normal == mesh->vertices[mesh->indices[j]].normal)
				{
					mesh->vertices[mesh->indices[i]].avTangent = (mesh->vertices[mesh->indices[i]].avTangent + mesh->tangents[j]) / 2.0f;
					mesh->vertices[mesh->indices[i]].avBitangent = (mesh->vertices[mesh->indices[i]].avBitangent + mesh->bitangents[j]) / 2.0f;
				}
			}
		}
	}


	/// <summary>
	/// Helper function for createCube. Note that this is not meant to be used standalone
	/// </summary>
	/// <param name="normal">Normal direction of the face</param>
	/// <param name="size">Width/height of the face</param>
	/// <param name="mesh">MeshData struct to fill</param>
	static void createCubeFace(const glm::vec3& normal, float size, MeshData* mesh) {
		unsigned int startVertex = mesh->vertices.size();
		glm::vec3 a = glm::vec3(normal.z, normal.x, normal.y); //U axis
		glm::vec3 b = glm::cross(normal, a); //V axis
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int row = i / 2;
			glm::vec3 pos = normal * size * 0.5f;
			pos -= (a + b) * size * 0.5f;
			pos += (a * (float)col + b * (float)row) * size;
			const glm::vec2 uv = glm::vec2(col, row);
			mesh->vertices.emplace_back(pos,normal,uv, glm::vec3(0), glm::vec3(0));
		}

		//Indices
		mesh->indices.emplace_back(startVertex);
		mesh->indices.emplace_back(startVertex + 1);
		mesh->indices.emplace_back(startVertex + 3);
		mesh->indices.emplace_back(startVertex + 3);
		mesh->indices.emplace_back(startVertex + 2);
		mesh->indices.emplace_back(startVertex);
	}
	/// <summary>
	/// Creates a cube of uniform size
	/// </summary>
	/// <param name="size">Total width, height, depth</param>
	/// <param name="mesh">MeshData struct to fill. Will be cleared.</param>
	void createCube(float size, MeshData* mesh) {
		mesh->vertices.reserve(24); //6 x 4 vertices
		mesh->indices.reserve(36); //6 x 6 indices
		createCubeFace(glm::vec3{ +0.0f,+0.0f,+1.0f }, size, mesh); //Front
		createCubeFace(glm::vec3{ +1.0f,+0.0f,+0.0f }, size, mesh); //Right
		createCubeFace(glm::vec3{ +0.0f,+1.0f,+0.0f }, size, mesh); //Top
		createCubeFace(glm::vec3{ -1.0f,+0.0f,+0.0f }, size, mesh); //Left
		createCubeFace(glm::vec3{ +0.0f,-1.0f,+0.0f }, size, mesh); //Bottom
		createCubeFace(glm::vec3{ +0.0f,+0.0f,-1.0f }, size, mesh); //Back
		return;
	}

	/// <summary>
	/// Creates a subdivided plane along X/Y axes
	/// </summary>
	/// <param name="width">Total width</param>
	/// <param name="height">Total height</param>
	/// <param name="subDivisions">Number of subdivisions</param>
	/// <returns></returns>
	void createPlaneXY(float width, float height, int subDivisions, MeshData* mesh) {
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
				pos.y = uv.y * height;
				pos.z = 0;
				glm::vec3 normal = glm::vec3(0, 0, 1);
				mesh->vertices.emplace_back(pos,normal,uv, glm::vec3(0), glm::vec3(0));
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

				glm::vec3 edgeOne = mesh->vertices[br].pos - mesh->vertices[bl].pos;
				glm::vec3 edgeTwo = mesh->vertices[tr].pos - mesh->vertices[bl].pos;
				glm::vec2 deltaUVOne = mesh->vertices[br].uv - mesh->vertices[bl].uv;
				glm::vec2 deltaUVTwo = mesh->vertices[tr].uv - mesh->vertices[bl].uv;

				float f = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVTwo.x * deltaUVOne.y);

				glm::vec3 tangent;
				tangent.x = f * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
				tangent.y = f * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
				tangent.z = f * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);

				glm::vec3 bitangent;
				bitangent.x = f * (-deltaUVTwo.x * edgeOne.x + deltaUVOne.x * edgeTwo.x);
				bitangent.y = f * (-deltaUVTwo.x * edgeOne.y + deltaUVOne.x * edgeTwo.y);
				bitangent.z = f * (-deltaUVTwo.x * edgeOne.z + deltaUVOne.x * edgeTwo.z);

				mesh->tangents.emplace_back(tangent);
				mesh->tangents.emplace_back(tangent);
				mesh->tangents.emplace_back(tangent);

				mesh->bitangents.emplace_back(bitangent);
				mesh->bitangents.emplace_back(bitangent);
				mesh->bitangents.emplace_back(bitangent);

				//Triangle 2
				mesh->indices.emplace_back(tr);
				mesh->indices.emplace_back(tl);
				mesh->indices.emplace_back(bl);

				edgeOne = mesh->vertices[tr].pos - mesh->vertices[bl].pos;
				edgeTwo = mesh->vertices[tl].pos - mesh->vertices[bl].pos;
				deltaUVOne = mesh->vertices[tr].uv - mesh->vertices[bl].uv;
				deltaUVTwo = mesh->vertices[tl].uv - mesh->vertices[bl].uv;

				tangent.x = f * (deltaUVTwo.y * edgeOne.x - deltaUVOne.y * edgeTwo.x);
				tangent.y = f * (deltaUVTwo.y * edgeOne.y - deltaUVOne.y * edgeTwo.y);
				tangent.z = f * (deltaUVTwo.y * edgeOne.z - deltaUVOne.y * edgeTwo.z);

				bitangent.x = f * (-deltaUVTwo.x * edgeOne.x + deltaUVOne.x * edgeTwo.x);
				bitangent.y = f * (-deltaUVTwo.x * edgeOne.y + deltaUVOne.x * edgeTwo.y);
				bitangent.z = f * (-deltaUVTwo.x * edgeOne.z + deltaUVOne.x * edgeTwo.z);

				mesh->tangents.emplace_back(tangent);
				mesh->tangents.emplace_back(tangent);
				mesh->tangents.emplace_back(tangent);

				mesh->bitangents.emplace_back(bitangent);
				mesh->bitangents.emplace_back(bitangent);
				mesh->bitangents.emplace_back(bitangent);
			}
		}

		averageOrthognals(mesh);

		return;
	}

	/// <summary>
	/// Creates a Polar sphere
	/// </summary>
	/// <param name="radius">Radius</param>
	/// <param name="subDivisions">Number of subdivisions (resolution). Min 3</param>
	/// <param name="mesh"></param>
	void createSphere(float radius, int subDivisions, MeshData* mesh) {
		mesh->vertices.clear();
		mesh->indices.clear();
		mesh->vertices.reserve((subDivisions + 1) * (subDivisions + 1));
		mesh->indices.reserve(subDivisions * subDivisions * 6);

		float thetaStep = 2 * PI / subDivisions;
		float phiStep = PI / subDivisions;
		for (size_t row = 0; row <= subDivisions; row++)
		{
			float phi = row * phiStep;
			for (size_t col = 0; col <= subDivisions; col++)
			{
				float theta = col * thetaStep;
				glm::vec2 uv;
				uv.x = 1.0-((float)col / subDivisions);
				uv.y = 1.0-((float)row / subDivisions);
				glm::vec3 pos;
				pos.x = cosf(theta) * sinf(phi) * radius;
				pos.y = cosf(phi) * radius;
				pos.z = sinf(theta) * sinf(phi) * radius;
				glm::vec3 normal = glm::normalize(pos);
				mesh->vertices.emplace_back(pos, normal, uv, glm::vec3(0), glm::vec3(0));
			}
		}

		//Indices
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

}