#include "MaterialManager.h"

#include "../shaders/fs_colorLit.h"
#include "../shaders/vs_simple.h"
#include "../shaders/fs_icon.h"
#include "../shaders/vs_icon.h"
 
void MaterialManager::Init()
{
	u_viewport = bgfx::createUniform("u_viewport", bgfx::UniformType::Vec4);
	u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
	//u_deformScale = bgfx::createUniform("u_deformScale", bgfx::UniformType::Vec4);
	//u_rangesIntervals = bgfx::createUniform("u_rangesIntervals", bgfx::UniformType::Vec4);
}

void MaterialManager::CreateMaterial(const uint8_t* v, uint32_t sizev, const uint8_t* f, uint32_t sizef)
{
	const bgfx::Memory* vs = bgfx::makeRef(v, sizev);
	const bgfx::Memory* fs = bgfx::makeRef(f, sizef);
	bgfx::ShaderHandle vsh = bgfx::createShader(vs);
	bgfx::ShaderHandle fsh = bgfx::createShader(fs);
	m_programs.push_back(bgfx::createProgram(vsh, fsh));
}

void MaterialManager::CreateDefaultMaterials()
{
	//ColorLit
	CreateMaterial(vs_simple_h, sizeof(vs_simple_h), fs_colorLit_h, sizeof(fs_colorLit_h));
	CreateMaterial(vs_icon_h, sizeof(vs_icon_h), fs_icon_h, sizeof(fs_icon_h));
}

void MaterialManager::SetCurrentRenderColor(const float* color)
{
	bgfx::setUniform(u_color, color, 1);
}

void MaterialManager::SubmitMeshReder(int view, int programm)
{
	bgfx::submit(view, m_programs[programm]);
}

void MaterialManager::SetViewPortScale(int width, int height)
{
	float values[4] = { static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f };
	bgfx::setUniform(u_viewport, values, 1);
}