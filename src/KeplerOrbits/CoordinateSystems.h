#pragma once

#include <iostream>

namespace KeplerOrbits
{
	double GetGMST(double jd);

	class OrbitalElements;

	class CartesianCoordinates
	{
	public:
		CartesianCoordinates();
		CartesianCoordinates(double x, double y, double z);

		double& x();
		const double& x() const;

		double& y();
		const double& y() const;

		double& z();
		const double& z() const;

	private:
		double m_x, m_y, m_z;
	};
	
	class GeoCoordinates
	{
	public:
		GeoCoordinates();

		GeoCoordinates(double latitude, double longitude);

		double& latitude();
		const double& latitude() const;

		double& longitude();
		const double& longitude() const;

	private:
		double m_latitude, m_longitude;
	};

	class HorizontalCoordinates
	{
		friend std::ostream& operator << (std::ostream& os, const HorizontalCoordinates& horizontal);
	public:
		HorizontalCoordinates();

		HorizontalCoordinates(double azimuth, double altitude);

		CartesianCoordinates ConvertToPolar() const;
		
	private:
		double m_azimuth, m_altitude;
	};

	class EulerCoordinates
	{
	public:
		EulerCoordinates();

		EulerCoordinates(double phi, double theta, double psi, double r);
		
		operator CartesianCoordinates() const;

		std::ostream& operator<<(std::ostream& os) const;

	private:
		double phi, theta, psi, r;
	};

	class EquatorialCoordinates
	{
		friend std::ostream& operator << (std::ostream& os, const EquatorialCoordinates& cartesian);
	public:
		EquatorialCoordinates();

		EquatorialCoordinates(double alpha, double delta);

		HorizontalCoordinates ConvertToHorizontalCoordinates(const GeoCoordinates& observer, double jd) const;
		
	private:
		double m_alpha, m_delta;
	};

	class EcliptikCoordinates
	{
		friend std::ostream& operator << (std::ostream& os, const EcliptikCoordinates& cartesian);
	public:
		EcliptikCoordinates();

		EcliptikCoordinates(double lambda, double betta);

		EcliptikCoordinates(const CartesianCoordinates& cartesian, const CartesianCoordinates& observerPosition);

		operator EquatorialCoordinates() const;
		
	private:
		double m_lambda, m_betta;
	};

	std::ostream& operator << (std::ostream& os, const CartesianCoordinates& cartesian);
	std::ostream& operator << (std::ostream& os, const HorizontalCoordinates& horizontal);
	std::ostream& operator << (std::ostream& os, const EquatorialCoordinates& equatorial);
	std::ostream& operator << (std::ostream& os, const EcliptikCoordinates& ecliptik);
}