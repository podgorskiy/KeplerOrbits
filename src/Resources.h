#include "Planet.h"
#include <tinyxml/tinyxml.h>
#include <vector>

#define toRad(x) (x/180.0*pi)

class Resources
{
public:
	typedef std::vector<Planet> Planets;
	Resources(){};
	~Resources(){};
	void Load( const char* fileName ){
		TiXmlDocument data( fileName );
		data.LoadFile();
		TiXmlHandle hData(&data);
		TiXmlElement* pElem;
		TiXmlHandle hRoot(0);
		pElem=hData.FirstChild("data").Element();
		if (!pElem) return;
		hRoot=TiXmlHandle(pElem);
		m_planets.clear();
		pElem=hRoot.FirstChild( "planets" ).FirstChild().Element();
		for( pElem; pElem; pElem=pElem->NextSiblingElement("planet") )
		{
			const char *pName=pElem->Attribute("name");
			if (!pName) 
			{
				continue;
			}
			TiXmlHandle hPlanet = TiXmlHandle(pElem);

			OrbitalElements orbit = OrbitalElements().
				EllipseShape( GetVal( hPlanet, "Eccentricity" ), GetVal( hPlanet, "SemimajorAxis" ) ).
				OrbitalPlane( GetRVal( hPlanet, "Inclination" ), GetRVal( hPlanet, "LongitudeOfAscendingNode" ) ).
				EllipseOrientation( GetRVal( hPlanet, "LongitudeOfPerihelion" ) ).
				TimeSpecific( GetRVal( hPlanet, "MeanLongitude" ), GetVal( hPlanet, "Epoch" ) );
			
			double siderealOrbitPeriod = GetVal( hPlanet, "SiderealOrbitPeriod" );
			m_planets.push_back( Planet( pName, orbit, siderealOrbitPeriod ) );
		}
	};
	const Planets& GetPlanets(){
		return m_planets;
	};
private:
	double GetVal(TiXmlHandle& handle, const char* name) const
	{
		double parametr;
		handle.FirstChild( name ).Element()->QueryDoubleAttribute("value", &parametr);
		return parametr;
	};
	double GetRVal(TiXmlHandle& handle, const char* name) const
	{
		return toRad( GetVal(handle, name) );
	};
	Planets m_planets;
};