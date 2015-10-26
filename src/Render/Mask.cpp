#define _USE_MATH_DEFINES

#include "Mask.h"
#include "RenderContext.h"
#include "VertexDeclarations.h"
#include "MaterialManager.h"

#include <bx/fpumath.h>

Mask::Mask()
{
}

void Mask::Render(const RenderContext& cntxt)
{
	float identity[16];
	bx::mtxIdentity(identity);
	bgfx::setTransform(identity);
	float ortho[16];
	bx::mtxOrtho(ortho, -1.0 * cntxt.m_width / cntxt.m_height, 1.0 * cntxt.m_width / cntxt.m_height, -1.0, 1.0, -2.0f, 2.0f);
	bgfx::setViewTransform(1, identity, ortho);
	bgfx::setViewRect(1, 0, 0, cntxt.m_width, cntxt.m_height);

	int count = 50;
	if (bgfx::checkAvailTransientVertexBuffer(4 * 3*count + 6, TrajectoryVertex::ms_decl))
	{
		bgfx::TransientVertexBuffer tvb;
		bgfx::allocTransientVertexBuffer(&tvb, 4 * 3 * count + 6, TrajectoryVertex::ms_decl);

		TrajectoryVertex* vertex = (TrajectoryVertex*)tvb.data;

		float signs[8] = { -1, -1, 1, -1, 1, 1, -1, 1 };
		for (int k = 0; k < 4; ++k)
		{
			for (int i = 0; i < count; ++i)
			{
				vertex->m_pos[0] = signs[2 * k + 0] * cos(i * 1.0 / count * M_PI / 2);
				vertex->m_pos[1] = signs[2 * k + 1] * sin(i * 1.0 / count * M_PI / 2);
				vertex->m_pos[2] = 1.0f;
				++vertex;
				vertex->m_pos[0] = signs[2 * k + 0] * 100.0f;
				vertex->m_pos[1] = signs[2 * k + 1] * 100.0f;
				vertex->m_pos[2] = 1.0f;
				++vertex;
				vertex->m_pos[0] = signs[2 * k + 0] * cos((i + 1) * 1.0 / count * M_PI / 2);
				vertex->m_pos[1] = signs[2 * k + 1] * sin((i + 1) * 1.0 / count * M_PI / 2);
				vertex->m_pos[2] = 1.0f;
				++vertex;
			}
		}
		vertex->m_pos[0] = -100.0f;
		vertex->m_pos[1] = -100.0f;
		vertex->m_pos[2] = 1.0f;
		++vertex;
		vertex->m_pos[0] = -1.0f;
		vertex->m_pos[1] = 0.0f;
		vertex->m_pos[2] = 1.0f;
		++vertex;
		vertex->m_pos[0] = -100.0f;
		vertex->m_pos[1] = 100.0f;
		vertex->m_pos[2] = 1.0f;
		++vertex;
		vertex->m_pos[0] = 100.0f;
		vertex->m_pos[1] = -100.0f;
		vertex->m_pos[2] = 1.0f;
		++vertex;
		vertex->m_pos[0] = 1.0f;
		vertex->m_pos[1] = 0.0f;
		vertex->m_pos[2] = 1.0f;
		++vertex;
		vertex->m_pos[0] = 100.0f;
		vertex->m_pos[1] = 100.0f;
		vertex->m_pos[2] = 1.0f;
		++vertex;

		bgfx::setVertexBuffer(&tvb);
		cntxt.m_materialManager->SetCurrentRenderColor(m_color);
		bgfx::setState(0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_ALPHA_WRITE
			| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
			);
		cntxt.m_materialManager->SubmitMeshReder(1, 0);
	}
}

void Mask::SetColor(const float color[4])
{
	memcpy(m_color, color, sizeof(m_color));
}