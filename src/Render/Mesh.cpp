#include "Mesh.h"
#include "VertexDeclarations.h"
#include "MaterialManager.h"

void MeshObject::Render(const RenderContext& cntxt)
{
	bgfx::setVertexBuffer(m_vbh);
	bgfx::setIndexBuffer(m_ibhLines);
	bgfx::setState(m_state | cntxt.m_state );
	cntxt.m_materialManager->SubmitMeshReder(0, m_programm);
}

void MeshObject::SetState(uint64_t state)
{
	m_state = state;
}

void MeshObject::SetProgram(MaterialManager::Programs programm)
{
	m_programm = programm;
}