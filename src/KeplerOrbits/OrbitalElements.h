#pragma once

namespace KeplerOrbits
{
	class OrbitalElements
	{
	public:
		OrbitalElements& SetEllipseShape(double eccentricity, double semimajorAxis);

		OrbitalElements& SetOrbitalPlane(double inclination, double longitudeOfAscendingNode);

		OrbitalElements& SetEllipseOrientation(double longitudeOfPerihelion);

		OrbitalElements& SetTimeSpecific(double meanLongitude, double epoch);

		double GetMeanAnomaly() const;

		double GetTSE(double jd) const;

		double GetEccentricAnomaly(double m) const;

		double GetRadius(double eccentricAnomaly) const;

		double GetArgumentOfPeriapsis() const;

		double GetTrueAnomaly(double eccentricAnomaly) const;

		double GetLongitudeOfAscendingNode() const;

		double GetInclination() const;

	private:
		double m_eccentricity;
		double m_semimajorAxis;

		double m_inclination;
		double m_longitudeOfAscendingNode;

		double m_longitudeOfPerihelion;
		double m_meanLongitude;
		double m_epoch;
	};
}