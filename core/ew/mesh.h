/*
	Author: Eric Winebrenner
*/

#pragma once
#include "ewMath/ewMath.h"
#include <glm/glm.hpp>

namespace ew {
	struct Vertex {
		glm::vec3 pos = glm::vec3(0);
		glm::vec3 normal = glm::vec3(0);
		glm::vec2 uv = glm::vec2(0);
		Vertex() {

		}
		Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& uv):
			pos(pos),normal(normal),uv(uv){
		}
	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		MeshData() {

		}
	};

	enum class DrawMode {
		TRIANGLES = 0,
		POINTS = 1
	};

	class Mesh {
	public:
		Mesh() {};
		Mesh(const MeshData& meshData);
		void load(const MeshData& meshData);
		void draw(DrawMode drawMode = DrawMode::TRIANGLES)const;
		void drawInstanced(DrawMode drawMode, unsigned int instanceCount)const;
		inline int getNumVertices()const { return m_numVertices; }
		inline int getNumIndices()const { return m_numIndices; }
		void bind() const;
	private:
		bool m_initialized = false;
		unsigned int m_vao = 0;
		unsigned int m_vbo = 0;
		unsigned int m_ebo = 0;
		int m_numVertices = 0;
		int m_numIndices = 0;
	};
}