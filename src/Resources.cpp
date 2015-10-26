#include "Resources.h"
#include <bgfx.h>
#include "../data/data.h"
#include <tinyxml.h>
#include <vector>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

inline double toRad(double x)
{
	return x / 180.0 * M_PI;
}

Resources::Resources()
{
};

Resources::~Resources()
{
};

void Resources::Load(const char* fileName)
{
	TiXmlDocument data;
	std::vector<char> dataFromXML(sizeof(data_h));
	memcpy(&dataFromXML[0], data_h, sizeof(data_h));
	dataFromXML.push_back(0);
	data.Parse(&dataFromXML[0], 0, TIXML_ENCODING_UTF8);

	TiXmlHandle hData(&data);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);
	pElem = hData.FirstChild("data").Element();
	if (!pElem) return;
	hRoot = TiXmlHandle(pElem);
	m_bodies.clear();
	pElem = hRoot.FirstChild("bodies").FirstChild().Element();
	for (pElem; pElem; pElem = pElem->NextSiblingElement("body"))
	{
		const char *pName = pElem->Attribute("name");
		if (!pName)
		{
			continue;
		}
		TiXmlHandle hPlanet = TiXmlHandle(pElem);

		KeplerOrbits::OrbitalElements orbit;

		orbit
			.SetEllipseShape(
				GetDVal(hPlanet, "Eccentricity"), 
				GetDVal(hPlanet, "SemimajorAxis")
			)
			.SetOrbitalPlane(
				GetRVal(hPlanet, "Inclination"), 
				GetRVal(hPlanet, "LongitudeOfAscendingNode")
			)
			.SetEllipseOrientation(
				GetRVal(hPlanet, "LongitudeOfPerihelion")
			)
			.SetTimeSpecific(
				GetRVal(hPlanet, "MeanLongitude"), 
				GetDVal(hPlanet, "Epoch")
			);

		double siderealOrbitPeriod = GetDVal(hPlanet, "SiderealOrbitPeriod");

		KeplerOrbits::Body planet(pName, orbit, siderealOrbitPeriod);

		RenderProperties renderProperties;

		int orbitColor = GetHVal(hPlanet, "OrbitColor");

		renderProperties.orbitColorF[2] = static_cast<float>(orbitColor & 0xff) / 255.0f;
		renderProperties.orbitColorF[1] = static_cast<float>((orbitColor >> 8) & 0xff) / 255.0f;
		renderProperties.orbitColorF[0] = static_cast<float>((orbitColor >> 16) & 0xff) / 255.0f;
		renderProperties.orbitColorI = orbitColor & 0xff00 | (orbitColor & 0xff) << 16 | (orbitColor & 0xff0000) >> 16 | 0xFF000000;
		
		m_bodies.push_back(std::make_pair(planet, renderProperties));
	}
};

const Resources::BodyList& Resources::GetBodies() const
{
	return m_bodies;
};

int Resources::GetIVal(TiXmlHandle& handle, const char* name) const
{
	int parametr;
	handle.FirstChild( name ).Element()->QueryIntAttribute("value", &parametr);
	return parametr;
};

int Resources::GetHVal(TiXmlHandle& handle, const char* name) const
{
	int parametr;
	std::stringstream ss;
	ss << std::hex << handle.FirstChild(name).Element()->Attribute("value");
	ss >> parametr;
	return parametr;
};

double Resources::GetDVal(TiXmlHandle& handle, const char* name) const
{
	double parametr;
	handle.FirstChild(name).Element()->QueryDoubleAttribute("value", &parametr);
	return parametr;
};

double Resources::GetRVal(TiXmlHandle& handle, const char* name) const
{
	return toRad( GetDVal(handle, name) );
};
