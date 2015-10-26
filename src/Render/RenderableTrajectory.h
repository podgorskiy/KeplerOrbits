#pragma once
#include "Mesh.h"
#include "KeplerOrbits/Body.h"
#include "VertexDeclarations.h"

#include <bgfx.h>
#include <vector>

class RenderableTrajectory
{
public:
	RenderableTrajectory(const KeplerOrbits::Trajectory& trajectory);

	/* Renders mesh. */
	void Render(const RenderContext& cntxt);

	void SetColor(const float color[3]);

private:
	MeshObject	m_mesh;
	std::vector<TrajectoryVertex>	m_vertices;
	std::vector<uint32_t>			m_indices;
	float							m_color[3];
};
