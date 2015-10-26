#include "CelestialBodyPositionCalculator.h"

void CelestialBodyPositionCalculator::SetObserverPosition(const KeplerOrbits::CartesianCoordinates& observerPosition)
{
	m_observerPosition = observerPosition;
}

void CelestialBodyPositionCalculator::SetGeoCoordinates(const KeplerOrbits::GeoCoordinates& geoCoordinates)
{
	m_geoCoordinates = geoCoordinates;
}

void CelestialBodyPositionCalculator::GenerateCelestialBodyInfo(const Resources::BodyList& bodyList, double jd)
{
	m_bodyInfoList.clear();
	for (Resources::BodyList::const_iterator it = bodyList.begin(); it != bodyList.end(); ++it)
	{
		BodyInfo body;
		body.m_name = it->first.GetName();
		body.m_cartesian = it->first.GetEulaerAnglesFromJulianDay(jd);
		body.m_ecliptik = KeplerOrbits::EcliptikCoordinates(body.m_cartesian, m_observerPosition);

		if (it->first.GetName() != "Earth")
		{
			body.m_equatorial = body.m_ecliptik;
			body.m_horisontal = body.m_equatorial.ConvertToHorizontalCoordinates(m_geoCoordinates, jd);
		}
		
		m_bodyInfoList.push_back(body);
	}
}

const CelestialBodyPositionCalculator::BodyInfoList& CelestialBodyPositionCalculator::GetBodyInfoList() const
{
	return m_bodyInfoList;
}
