#pragma once
#include <vector>
#include <bgfx.h>

class MaterialManager
{
public:
	enum Programs
	{
		ColorLit,
		Icon,
	};

	void Init();

	void CreateDefaultMaterials();

	void CreateMaterial(const uint8_t* v, uint32_t sizev, const uint8_t* f, uint32_t sizef);

	void SetCurrentRenderColor(const float* color);

	void SubmitMeshReder(int view, int programm);

	void SetDeformsScale(float scale);

	void SetIntervals(float min, float max, float intervals);

	void SetViewPortScale(int width, int height);
private:
	std::vector<bgfx::ProgramHandle> m_programs;
	bgfx::UniformHandle u_percantage;
	bgfx::UniformHandle u_color;
	bgfx::UniformHandle u_viewport;
	bgfx::UniformHandle u_rangesIntervals;
};