#pragma once
#include "Mesh.h"
#include "KeplerOrbits/Body.h"
#include "VertexDeclarations.h"

#include <bgfx.h>
#include <vector>

class Icon
{
public:
	Icon();

	/* Renders mesh. */
	void Render(const RenderContext& cntxt);

	void SetColor(const float color[3]);

private:
	MeshObject	m_mesh;
	float		m_color[3];
};
