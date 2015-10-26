#pragma once

#include "KeplerOrbits/CoordinateSystems.h"
#include "Resources.h"

#include <vector>

class CelestialBodyPositionCalculator
{
public:
	struct BodyInfo
	{
		std::string m_name;
		KeplerOrbits::CartesianCoordinates m_cartesian;
		KeplerOrbits::EcliptikCoordinates m_ecliptik;
		KeplerOrbits::EquatorialCoordinates m_equatorial;
		KeplerOrbits::HorizontalCoordinates m_horisontal;
	};
	
	typedef std::vector<BodyInfo> BodyInfoList;
	
	void SetObserverPosition(const KeplerOrbits::CartesianCoordinates& observerPosition);

	void SetGeoCoordinates(const KeplerOrbits::GeoCoordinates& geoCoordinates);

	void GenerateCelestialBodyInfo(const Resources::BodyList& bodylist, double jd);
	
	const BodyInfoList& GetBodyInfoList() const;

private:
	KeplerOrbits::CartesianCoordinates m_observerPosition;
	KeplerOrbits::GeoCoordinates m_geoCoordinates;
	BodyInfoList m_bodyInfoList;
};