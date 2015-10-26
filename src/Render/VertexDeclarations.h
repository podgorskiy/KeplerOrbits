#pragma once 

#include <bgfx.h>

struct TrajectoryVertex
{
	float m_pos[3];
	static void init();

	static bgfx::VertexDecl ms_decl;
};
