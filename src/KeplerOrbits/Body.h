#pragma once
#include "OrbitalElements.h"
#include <string>
#include <vector>

namespace KeplerOrbits
{
	class EulerCoordinates;
	class CartesianCoordinates;


	typedef std::vector<CartesianCoordinates> Trajectory;

	class Body
	{
	public:
		Body(
			const char*			name,
			OrbitalElements&	orbitalElements,
			double				siderealOrbitPeriod);

		~Body();

		EulerCoordinates GetEulaerAnglesFromJulianDay(double jd) const;
		
		double GetSiderealOrbitPeriod() const;

		std::string GetName() const;
		
		Trajectory GetTrajectory(int stepCount) const;
		
	private:
		EulerCoordinates GetEulaerAnglesFromEccentricAnomaly(double eccentricAnomaly) const;

		OrbitalElements m_orbitalElements;
		std::string m_name;
		double m_siderealOrbitPeriod;
	};
}