#include "VertexDeclarations.h"

bgfx::VertexDecl TrajectoryVertex::ms_decl;

void TrajectoryVertex::init()
{
	ms_decl
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.end();
};
