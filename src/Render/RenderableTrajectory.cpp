#include "RenderableTrajectory.h"
#include "RenderContext.h"
#include "VertexDeclarations.h"
#include "MaterialManager.h"
#include "KeplerOrbits/CoordinateSystems.h"

#include <cstring>

RenderableTrajectory::RenderableTrajectory(const KeplerOrbits::Trajectory& trajectory)
{
	int id = 0;
	for (std::vector<KeplerOrbits::CartesianCoordinates>::const_iterator it = trajectory.begin(); it != trajectory.end(); ++it)
	{
		TrajectoryVertex vertex;
		const KeplerOrbits::CartesianCoordinates& point = *it;
		vertex.m_pos[0] = static_cast<float>(point.x());
		vertex.m_pos[1] = static_cast<float>(point.y());
		vertex.m_pos[2] = static_cast<float>(point.z());
		m_vertices.push_back(vertex);
		m_indices.push_back(id++);
		m_indices.push_back(trajectory.end() - it == 1 ? 0 : id);
	}
	m_mesh.CreateBuffers(m_vertices, m_indices);
	m_mesh.SetState(BGFX_STATE_RGB_WRITE | BGFX_STATE_PT_LINES);
	m_mesh.SetProgram(MaterialManager::ColorLit);
}

void RenderableTrajectory::Render(const RenderContext& cntxt)
{
	cntxt.m_materialManager->SetCurrentRenderColor(m_color);
	m_mesh.Render(cntxt);
}

void RenderableTrajectory::SetColor(const float color[3])
{
	memcpy(m_color, color, sizeof(m_color));
}