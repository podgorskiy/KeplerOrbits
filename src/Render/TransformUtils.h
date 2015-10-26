#pragma once
#include "Camera.h"

#include <bx/fpumath.h>
#include <bgfx_utils.h>

inline void TransformToWorldSpace(float xInp, float yInp, float zInp, int width, int height, const Camera& camera, float& xOut, float& yOut)
{
	float pos[4];
	float posTransformed[4];
	pos[0] = xInp;
	pos[1] = yInp;
	pos[2] = zInp;
	pos[3] = 1.0f;
	float viewProj[16];
	camera.GetViewProj(viewProj);
	bx::vec4MulMtx(posTransformed, pos, viewProj);

	xOut = posTransformed[0] / 2.0f * width + width / 2.0f;
	yOut = height / 2.0f - posTransformed[1] / 2.0f * height;
}