#include "Body.h"
#include "CoordinateSystems.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace KeplerOrbits;


Body::Body(const char* name, OrbitalElements&	orbitalElements, double siderealOrbitPeriod) :
		m_name(name),
		m_orbitalElements(orbitalElements),
		m_siderealOrbitPeriod(siderealOrbitPeriod)
{};

Body::~Body()
{};

EulerCoordinates Body::GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const
{
	double trueAnomaly = m_orbitalElements.GetTrueAnomaly(eccentricAnomaly);
	double argumentOfPeriapsis = m_orbitalElements.GetArgumentOfPeriapsis();

	double psi = trueAnomaly + argumentOfPeriapsis;

	double r = m_orbitalElements.GetRadius(eccentricAnomaly);
	return EulerCoordinates(
		psi, 
		m_orbitalElements.GetLongitudeOfAscendingNode(),
		m_orbitalElements.GetInclination(),
		r);
}

EulerCoordinates Body::GetEulaerAnglesFromJulianDay(double jd) const
{
	double meanMotion = 2.0 * M_PI / m_siderealOrbitPeriod;
	double currentMeanAnomaly = m_orbitalElements.GetMeanAnomaly() + meanMotion * m_orbitalElements.GetTSE(jd);

	double eccentricAnomaly = m_orbitalElements.GetEccentricAnomaly(currentMeanAnomaly);

	return GetEulaerAnglesFromEccentricAnomaly(eccentricAnomaly);
};

double Body::GetSiderealOrbitPeriod() const
{
	return m_siderealOrbitPeriod;
}

std::string Body::GetName() const
{
	return m_name;
}

Trajectory Body::GetTrajectory(int stepCount) const
{
	Trajectory trajectory;
	trajectory.resize(stepCount);
	for (int j = 0; j < stepCount; j++)
	{
		trajectory[j] = GetEulaerAnglesFromEccentricAnomaly(j * 2 * M_PI / stepCount);
	}
	return trajectory;
}
