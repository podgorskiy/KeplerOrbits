#include "Icon.h"
#include "RenderContext.h"
#include "VertexDeclarations.h"
#include "MaterialManager.h"

#include <cstring>

Icon::Icon()
{
	static const TrajectoryVertex vertices[4] =
	{
		{ { 0.0f, -3.0f, 0.0f } },
		{ { 3.0f, 0.0f, 0.0f } },
		{ { 0.0f, 3.0f, 0.0f } },
		{ { -3.0f, 0.0f, 0.0f } } 
	};

	static const uint16_t indices[6] =
	{
		0, 1, 2, 0, 2, 3
	};

	m_mesh.CreateBuffers(vertices, indices);
	m_mesh.SetState(BGFX_STATE_RGB_WRITE | BGFX_STATE_PT_TRISTRIP);
	m_mesh.SetProgram(MaterialManager::Icon);
}

void Icon::Render(const RenderContext& cntxt)
{
	cntxt.m_materialManager->SetCurrentRenderColor(m_color);
	m_mesh.Render(cntxt);
}

void Icon::SetColor(const float color[3])
{
	memcpy(m_color, color, sizeof(m_color));
}