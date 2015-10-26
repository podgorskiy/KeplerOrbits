#pragma once

#include <entry/entry.h>
#include <string>
#include "ViewType.h"
#include "Render/Camera.h"
#include "Render/MaterialManager.h"
#include "Resources.h"
#include "Render/Mask.h"
#include "CelestialBodyPositionCalculator.h"

#ifndef __EMSCRIPTEN__
#define EM_ASM(X)
#endif

class RenderableTrajectory;

class Application: public  entry::AppI
{
public:
	virtual void init(int /*_argc*/, char** /*_argv*/);
	virtual bool update();
	virtual int shutdown();
	bool ProcessEvent();

private:
	std::vector<RenderableTrajectory*> m_trajectories;

	bool m_detailedReportEnabled;
	bool m_aboutEnabled;
	ViewType::ViewType m_viewType;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_reset;
	uint32_t m_debug;
	Camera m_camera;
	MaterialManager m_materialManager;
	entry::MouseState m_mouseState;
	
	Resources m_resources;
	CelestialBodyPositionCalculator m_celestialBodyPositionCalculator;

	Mask m_mask;
	int m_fontSize;
	int m_scale;
	
	std::vector<std::string> m_licenseText; 

public:
	static KeplerOrbits::GeoCoordinates m_geoCoordinates;
};