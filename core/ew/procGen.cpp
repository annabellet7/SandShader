/*
	Author: Eric Winebrenner
*/


#include "procGen.h"
#include <stdlib.h>

namespace ew {
	void averageTangents(MeshData* mesh)
	{
		for (size_t i = 0; i < mesh->indices.size() - 1; i++)
		{
			if (mesh->vertices[mesh->indices[i]].avTangent == glm::vec3(0.0f))
			{
				mesh->vertices[mesh->indices[i]].avTangent = mesh->tangents[i];
			}
			for (size_t j = i + 1; j < mesh->indices.size(); j++)
			{
				if (mesh->vertices[mesh->indices[i]].pos == mesh->vertices[mesh->indices[j]].pos &&
					mesh->vertices[mesh->indices[i]].uv == mesh->vertices[mesh->indices[j]].uv &&
					mesh->vertices[mesh->indices[i]].normal == mesh->vertices[mesh->indices[j]].normal)
				{
					mesh->vertices[mesh->indices[i]].avTangent = (mesh->vertices[mesh->indices[i]].avTangent + mesh->tangents[j]) / 2.0f;
				}
			}
		}

		return;
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
			mesh->vertices.emplace_back(pos,normal,uv,glm::vec3(0));
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
		mesh->tangents.reserve(subDivisions * subDivisions * 6);

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
				mesh->vertices.emplace_back(pos,normal,uv, glm::vec3(0));
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

		for (int i = 0; i < mesh->vertices.size() - 1; i += 3)
		{
			//triagnle one
			//positions
			glm::vec3 posOne = mesh->vertices[i].pos;
			glm::vec3 posTwo = mesh->vertices[i + 1].pos;
			glm::vec3 posThree = mesh->vertices[i + 2].pos;

			//uvs
			glm::vec2 UVOne = mesh->vertices[i].uv;
			glm::vec2 UVTwo = mesh->vertices[i + 1].uv;
			glm::vec2 UVThree = mesh->vertices[i + 2].uv;

			//triangle edges
			glm::vec3 deltaPosOne = posTwo - posOne;
			glm::vec3 deltaPosTwo = posThree - posOne;

			//uv delta
			glm::vec2 deltaUVOne = UVTwo - UVOne;
			glm::vec2 deltaUVTwo = UVThree - UVOne;

			float f = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVOne.y - deltaUVTwo.x);
			glm::vec3 tangent = f * (deltaPosOne * deltaUVTwo.y - deltaPosTwo * deltaUVOne.y);

			/*mesh->tangents.emplace_back(tangent);
			mesh->tangents.emplace_back(tangent);
			mesh->tangents.emplace_back(tangent);*/

			//triangle two
			//positions
			posOne = mesh->vertices[i + 1].pos;
			posTwo = mesh->vertices[i + 2].pos;
			posThree = mesh->vertices[i + 3].pos;

			//uvs
			UVOne = mesh->vertices[i + 1].uv;
			UVTwo = mesh->vertices[i + 2].uv;
			UVThree = mesh->vertices[i + 3].uv;

			//triangle edges
			deltaPosOne = posTwo - posOne;
			deltaPosTwo = posThree - posOne;

			//uv delta
			deltaUVOne = UVTwo - UVOne;
			deltaUVTwo = UVThree - UVOne;

			f = 1.0f / (deltaUVOne.x * deltaUVTwo.y - deltaUVOne.y - deltaUVTwo.x);
			tangent = f * (deltaPosOne * deltaUVTwo.y - deltaPosTwo * deltaUVOne.y);

			/*mesh->tangents.emplace_back(tangent);
			mesh->tangents.emplace_back(tangent);
			mesh->tangents.emplace_back(tangent);*/
		}

		//averageTangents(mesh);

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
				mesh->vertices.emplace_back(pos, normal, uv, glm::vec3(0));
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