#pragma once
#include <bgfx.h>
#include <vector>
#include "MaterialManager.h"
#include "RenderContext.h"

class MeshObject
{
public:
	void Render(const RenderContext& cntxt);

	template<typename Vertex, typename Index>
	inline void CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<Index>& indices);

	template<typename Vertex, int V_count, typename Index, int I_count>
	void CreateBuffers(const Vertex(&vertices)[V_count], const Index(&indices)[I_count]);
	
	void SetState(uint64_t state);

	void SetProgram(MaterialManager::Programs programm);

private:
	template<typename Index>
	int GetIndexType();

	bgfx::IndexBufferHandle			m_ibhLines;
	bgfx::VertexBufferHandle		m_vbh;
	uint64_t						m_state;
	MaterialManager::Programs		m_programm;
};


template<typename Vertex, typename Index>
inline void MeshObject::CreateBuffers(const std::vector<Vertex>& vertices, const std::vector<Index>& indices)
{
	m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(&vertices[0], sizeof(Vertex) * vertices.size()), Vertex::ms_decl);
	m_ibhLines = bgfx::createIndexBuffer(bgfx::makeRef(&indices[0], sizeof(Index) * indices.size()), GetIndexType<Index>());
}

template<typename Vertex, int V_count, typename Index, int I_count>
inline void MeshObject::CreateBuffers(const Vertex(&vertices)[V_count], const Index(&indices)[I_count])
{
	m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(Vertex) * V_count), Vertex::ms_decl);
	m_ibhLines = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(Index) * I_count), GetIndexType<Index>());
}

template<typename Index>
inline int MeshObject::GetIndexType()
{
	return 0;
}

template<>
inline int MeshObject::GetIndexType<uint32_t>()
{
	return BGFX_BUFFER_INDEX32;
}
