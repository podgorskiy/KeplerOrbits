#define _USE_MATH_DEFINES

#include "Render/Mesh.h"
#include "Application.h"
#include "KeplerOrbits/CoordinateSystems.h"
#include "Render/RenderContext.h"
#include "Render/RenderableTrajectory.h"
#include "Render/Icon.h"
#include "Render/VertexDeclarations.h"
#include "Render/TransformUtils.h"

#include <bx/timer.h>
#include <bx/fpumath.h>

#include <entry/entry.h>
#include <bgfx_utils.h>

#include <bgfx.h>
#include <bx/timer.h>
#include <bx/fpumath.h>
#include <imgui/imgui.h>

#include <font/font_manager.h>
#include <font/text_metrics.h>
#include <font/text_buffer_manager.h>

#include <ocornut-imgui/imgui.h>
#include <ctime>
#include <sstream>

#include "../data/licence_text.h"

KeplerOrbits::GeoCoordinates Application::m_geoCoordinates;

extern  "C"
{
	void setGeoLocation(float latitude, float longitude)
	{
		Application::m_geoCoordinates.latitude() = latitude / 180 * M_PI;
		Application::m_geoCoordinates.longitude() = longitude / 180 * M_PI;
	}
}

double Greg2jd(const tm& x)
{
	int mon = x.tm_mon + 1;
	int year = x.tm_year + 1900;
	int a = (14 - mon) / 12;
	int y = year + 4800 - a;
	int m = mon + 12 * a - 3;
	double jd = x.tm_mday + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
	jd += (x.tm_hour - 12) / 24.0 + x.tm_min / 1440.0 + x.tm_sec / 86400.0;
	return jd;
}

Icon* icon;

void Application::init(int /*_argc*/, char** /*_argv*/)
{
	m_width = 512;
	m_height = 512;
	m_reset = BGFX_RESET_VSYNC;// | BGFX_RESET_MSAA_X16;

	bgfx::init(bgfx::RendererType::OpenGL);

	bgfx::reset(m_width, m_height, m_reset);

	// Enable debug text.
	m_debug = BGFX_DEBUG_TEXT;
	bgfx::setDebug(BGFX_DEBUG_TEXT);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		, 0x000000ff
		, 1.0f
		, 0
		);
	
	m_fontSize = 15;
	m_scale = 1;
#ifdef __EMSCRIPTEN__
	float devicePixelRatio = EM_ASM_DOUBLE_V(
	{
		if(window.devicePixelRatio != undefined)
		{
			return window.devicePixelRatio;
		}
		else
		{
			return 1.0;
		}
	}
	);
	m_scale = devicePixelRatio;
	m_fontSize = m_fontSize * devicePixelRatio;
#endif
	imguiCreate(NULL, 0, m_fontSize);
	m_camera.Reset();
	
	m_detailedReportEnabled = false;
	m_aboutEnabled = false;
	m_viewType = ViewType::FromAboveTheEclipticPlane;

	TrajectoryVertex::init();

	m_materialManager.Init();
	m_materialManager.CreateDefaultMaterials();

	m_resources.Load("data/data.xml");

	const Resources::BodyList& bodiesList = m_resources.GetBodies();

	for (Resources::BodyList::const_iterator it = bodiesList.begin(); it != bodiesList.end(); ++it)
	{
		KeplerOrbits::Trajectory trajectory = it->first.GetTrajectory(500);
		RenderableTrajectory* renderableTrajectory = new RenderableTrajectory(trajectory);
		renderableTrajectory->SetColor(it->second.orbitColorF);
		m_trajectories.push_back(renderableTrajectory);
	} 

	icon = new Icon();
	
	m_geoCoordinates = KeplerOrbits::GeoCoordinates(39.6396783 / 180.0 * M_PI, -79.95560239999999 / 180.0 * M_PI);
	m_celestialBodyPositionCalculator.SetGeoCoordinates(m_geoCoordinates);
	
	std::string licence_text;
	licence_text.resize(sizeof(licence_text_h));
	memcpy(&licence_text[0], licence_text_h, sizeof(licence_text_h));
	std::stringstream ss(licence_text);
	
	std::string line;
	while (std::getline(ss, line))
	{
		m_licenseText.push_back(line);
	}	
}

bool Application::ProcessEvent()
{
	bool result =  entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState);
	return result;
}

static double Clamp(double x, double min, double max)
{
	x = x < min ? x = min : x;
	x = x > max ? x = max : x;
	return x;
}

bool Application::update()
{
	if (ProcessEvent() )
	{
		return false;
	}

	EM_ASM(
	{
		UpdateCanvas();
	}
	);

	static bool firstFrame = true;
	if (firstFrame)
	{
		firstFrame = false;
		return true;
	}

	time_t rawtime;
	tm * ptm;

	time(&rawtime);

	ptm = gmtime(&rawtime);
	double jd = Greg2jd(*ptm);
	
	const Resources::BodyList& bodiesList = m_resources.GetBodies();

	KeplerOrbits::CartesianCoordinates observerPosition = bodiesList[3].first.GetEulaerAnglesFromJulianDay(jd);
	m_celestialBodyPositionCalculator.SetObserverPosition(observerPosition);
	
	m_celestialBodyPositionCalculator.GenerateCelestialBodyInfo(bodiesList, jd);
	
	imguiBeginFrame(m_mouseState.m_mx
		, m_mouseState.m_my
		, (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
		| (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
		| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
		, m_mouseState.m_mz
		, m_width
		, m_height
		);
	
	//ImGui::ShowTestWindow();

	bool blockMouse = false;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0, 40, 70));
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Once);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize;
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = 30 * m_scale;
	style.FramePadding = ImVec2(10 * m_scale, 5 * m_scale);
	style.ItemInnerSpacing = ImVec2(10 * m_scale, 5 * m_scale);
	ImGui::Begin("View options", NULL, window_flags);
	{
		int viewType = m_viewType;
		ImGui::Text("Julian day: %f", jd);
		ImGui::Text("GMST: %f", KeplerOrbits::GetGMST(jd));
		ImGui::Separator();
		ImGui::Text("Ecliptic coordinate system, orthogonal projection:");
		ImGui::RadioButton("View from above the ecliptic plane. Vernal equinox is to the right", &viewType, 0);
		ImGui::RadioButton("View from the edge of the ecliptic plane. Vernal equinox is to the right", &viewType, 1); //ImGui::SameLine();
		ImGui::RadioButton("View from the direction of vernal equinox.", &viewType, 2);
		ImGui::Separator();
		ImGui::RadioButton("Horisontal coordinate system. View from observer on Earth.", &viewType, 3);
		ImGui::Separator();
		float latitude = m_geoCoordinates.latitude() / M_PI * 180;
		float longitude = m_geoCoordinates.longitude() / M_PI * 180;
		ImGui::InputFloat("Latitude", &latitude, 0.01f, 1.0f, 3);
		ImGui::InputFloat("Longitude", &longitude, 0.01f, 1.0f, 3);
		latitude = latitude / 180 * M_PI;
		longitude = longitude / 180 * M_PI;
		//latitude = Clamp(latitude, -M_PI / 2.0, M_PI / 2.0);
		//longitude = Clamp(longitude, -M_PI, M_PI);
		m_geoCoordinates.latitude() = latitude;
		m_geoCoordinates.longitude() = longitude;
#ifdef __EMSCRIPTEN__
		if (ImGui::Button("Grab location from geolocation data", ImVec2(250 * m_scale, 40 * m_scale)))
		{
			EM_ASM(
			{
				 GrabLocation();
			}
			);
		}
#endif
		m_celestialBodyPositionCalculator.SetGeoCoordinates(m_geoCoordinates);
		ImGui::Separator();
		if (ImGui::Button("Generate detailed report", ImVec2(250 * m_scale, 40 * m_scale)))
		{
			m_detailedReportEnabled = true;
		}
		m_viewType = static_cast<ViewType::ViewType>(viewType);
		ImGui::SameLine();
		if (ImGui::Button("About", ImVec2(100 * m_scale, 40 * m_scale)))
		{
			m_aboutEnabled = true;
		}
	}
	blockMouse = blockMouse || ImGui::IsWindowHovered();
	ImGui::End();
	
	if (m_detailedReportEnabled)
	{
		ImGui::SetNextWindowSize(ImVec2(400 * m_scale, 600 * m_scale), ImGuiSetCond_Once);
		ImGui::Begin("Detailed Report", &m_detailedReportEnabled);
		ImGui::Text("Julian day: %f", jd);
		ImGui::Text("GMST: %f", KeplerOrbits::GetGMST(jd));
		ImGui::Separator();

		const CelestialBodyPositionCalculator::BodyInfoList& bodyInfoList = m_celestialBodyPositionCalculator.GetBodyInfoList();

		std::stringstream ss;
		for (CelestialBodyPositionCalculator::BodyInfoList::const_iterator it = bodyInfoList.begin(); it != bodyInfoList.end(); ++it)
		{
			ss << it->m_name << ":" << std::endl;
			ss << "Horizontal: " << it->m_horisontal << std::endl;
			ss << "Equatorial: " << it->m_equatorial << std::endl;
			ss << "Ecliptic: " << it->m_ecliptik << std::endl;
			ss << "Cartesian: " << it->m_cartesian << std::endl;
			ss << std::endl;
		}

		std::string line;
		while (std::getline(ss, line))
		{
			ImGui::Text(line.c_str());
		}

		blockMouse = blockMouse || ImGui::IsWindowHovered();
		ImGui::End();
	}
	
	if (m_aboutEnabled)
	{
		ImGui::Begin("About", &m_aboutEnabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		for (std::vector<std::string>::iterator it = m_licenseText.begin(); it != m_licenseText.end(); ++it)
		{
			ImGui::Text(it->c_str());
		}
		blockMouse = blockMouse || ImGui::IsWindowHovered();
		ImGui::End();
	}
	

	ImGui::PopStyleColor();

	m_camera.SetWidthHeight(m_width, m_height);
	m_camera.Update(m_mouseState, blockMouse, m_viewType);

	// Set view 0 default viewport.
	bgfx::setViewRect(0, 0, 0, m_width, m_height);

	m_camera.UpplyTransform(m_viewType);

	RenderContext cntxt;
	cntxt.m_camera = &m_camera;
	cntxt.m_height = m_height;
	cntxt.m_width = m_width;
	cntxt.m_materialManager = &m_materialManager;
	cntxt.m_state = 0;
	m_materialManager.SetViewPortScale(m_width, m_height);

	if (m_viewType != ViewType::FormObserverOnEarth)
	{
		for (std::vector<RenderableTrajectory*>::iterator it = m_trajectories.begin(); it != m_trajectories.end(); ++it)
		{
			(*it)->Render(cntxt);
		}
	}

	const CelestialBodyPositionCalculator::BodyInfoList& bodyInfoList = m_celestialBodyPositionCalculator.GetBodyInfoList();

	for (int i = 0, l = bodiesList.size(); i < l; ++i)
	{
		if (m_viewType == ViewType::FormObserverOnEarth && bodiesList[i].first.GetName() == "Earth")
		{
			continue;
		}
		KeplerOrbits::CartesianCoordinates cartesian = bodyInfoList[i].m_cartesian;

		if (m_viewType == ViewType::FormObserverOnEarth)
		{
			cartesian = bodyInfoList[i].m_horisontal.ConvertToPolar();
		}

		float screenX, screenY;
		TransformToWorldSpace(cartesian.x(), cartesian.y(), cartesian.z(), m_width, m_height, m_camera, screenX, screenY);

		float translate[16];
		bx::mtxTranslate(translate, cartesian.x(), cartesian.y(), cartesian.z());
		bgfx::setTransform(translate);
		icon->SetColor(bodiesList[i].second.orbitColorF);
		icon->Render(cntxt);
		std::string name = "  " + bodiesList[i].first.GetName();
		imguiDrawText(static_cast<int>(screenX), static_cast<int>(screenY), ImguiTextAlign::Left, name.c_str(), bodiesList[i].second.orbitColorI);
	}
	
	if (m_viewType == ViewType::FormObserverOnEarth)
	{
		int count = 200;
		for (int i = 0; i < count+1; ++i)
		{
			KeplerOrbits::EquatorialCoordinates equatorial = KeplerOrbits::EcliptikCoordinates(i * 1.0f / count * 2.0f * M_PI, 0.0);
			KeplerOrbits::HorizontalCoordinates horisontal = equatorial.ConvertToHorizontalCoordinates(m_geoCoordinates, jd);
			KeplerOrbits::CartesianCoordinates cartesian = horisontal.ConvertToPolar();

			float screenX1 = 0;
			float screenY1 = 0;
			TransformToWorldSpace(cartesian.x(), cartesian.y(), cartesian.z(), m_width, m_height, m_camera, screenX1, screenY1);

			static float screenX2 = 0;
			static float screenY2 = 0;

			if (i != 0)
				imguiDrawLine(screenX1, screenY1, screenX2, screenY2, 1.0, 0xFF00FF00);

			screenX2 = screenX1;
			screenY2 = screenY1;

			imguiDrawLine(
						m_width / 2 + sin(i * 1.0f / count * 2.0f * M_PI) * m_height / 4,
						m_height / 2 + cos(i * 1.0f / count * 2.0f * M_PI) * m_height / 4, 
						m_width / 2 + sin((i + 1) * 1.0f / count * 2.0f * M_PI) * m_height / 4,
						m_height / 2 + cos((i + 1) * 1.0f / count * 2.0f * M_PI) * m_height / 4, 1.0, 0xFF444444);
		}

		imguiDrawText(m_width / 2, 20, ImguiTextAlign::Left, "N", 0xFFFFFFFF);
		imguiDrawText(m_width / 2, m_height - 10, ImguiTextAlign::Left, "S", 0xFFFFFFFF);
		imguiDrawText(m_width / 2 - m_height / 2 + 10, m_height / 2 + 5, ImguiTextAlign::Left, "E", 0xFFFFFFFF);
		imguiDrawText(m_width / 2 + m_height / 2 - 20, m_height / 2 + 5, ImguiTextAlign::Left, "W", 0xFFFFFFFF);

		imguiDrawLine(m_width / 2 - m_height / 2, m_height / 2, m_width / 2 + m_height / 2, m_height / 2, 1.0, 0xFF444444);
		imguiDrawLine(
			m_width / 2 - m_height / 2 * sin(M_PI / 4), m_height / 2 - m_height / 2 * cos(M_PI / 4),
			m_width / 2 + m_height / 2 * sin(M_PI / 4), m_height / 2 + m_height / 2 * cos(M_PI / 4), 1.0, 0xFF444444);
		imguiDrawLine(
			m_width / 2 + m_height / 2 * sin(M_PI / 4), m_height / 2 - m_height / 2 * cos(M_PI / 4),
			m_width / 2 - m_height / 2 * sin(M_PI / 4), m_height / 2 + m_height / 2 * cos(M_PI / 4), 1.0, 0xFF444444);
		imguiDrawLine(m_width / 2 , 0, m_width / 2, m_height, 1.0, 0x44FFFFFF);
	}
	
	imguiEndFrame();

	if (m_viewType == ViewType::FormObserverOnEarth)
	{
		float maskColor[4] = { 0.3, 0.3, 0.3, 0.5 };
		m_mask.SetColor(maskColor);
		m_mask.Render(cntxt);
	}

	bgfx::touch(0);
	bgfx::dbgTextClear();
	bgfx::frame();
	return true;
}

int Application::shutdown()
{
	imguiDestroy();
	// Shutdown bgfx.
	bgfx::shutdown();
	return 0;
}

ENTRY_IMPLEMENT_MAIN(Application);

