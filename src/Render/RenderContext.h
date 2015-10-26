#pragma once
#include <stdint.h>

class Camera;
class MaterialManager;

struct RenderContext
{
	Camera * m_camera;
	int m_width;
	int m_height;
	uint64_t m_state;

	MaterialManager * m_materialManager;
};