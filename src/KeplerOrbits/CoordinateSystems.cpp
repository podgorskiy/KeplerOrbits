#define _USE_MATH_DEFINES

#include "CoordinateSystems.h"
#include "OrbitalElements.h"
#include <cmath>

using namespace KeplerOrbits;


namespace KeplerOrbits
{
	double GetGMST(double jd)
	{
		double x;
		x = 18.697374558 + 24.06570982441908 * (jd - 2451545);
		return ((x / 24) - floor(x / 24)) * 24;
	}
}

static double getlha(double alpha, double lambda, double jd)
{
	double gmst = GetGMST(jd);
	return (double)(gmst) / 12 * M_PI + lambda - alpha;
}

CartesianCoordinates::CartesianCoordinates() :
	m_x(0),
	m_y(0),
	m_z(0)
{
}

CartesianCoordinates::CartesianCoordinates(double x, double y, double z) :
	m_x(x),
	m_y(y),
	m_z(z)
{};

EulerCoordinates::EulerCoordinates(double phi, double theta, double psi, double r) :
	phi(phi), 
	theta(theta), 
	psi(psi), 
	r(r)
{};

EulerCoordinates::operator CartesianCoordinates() const
{
	double x = r * (cos(phi) * cos(theta) - sin(phi) * cos(psi) * sin(theta));
	double y = r * (sin(phi) * cos(theta) * cos(psi) + cos(phi) * sin(theta));
	double z = r * (sin(phi) * sin(psi));
	return CartesianCoordinates(x, y, z);
};

EcliptikCoordinates::EcliptikCoordinates() : m_lambda(0), m_betta(0)
{
}

EcliptikCoordinates::EcliptikCoordinates(double lambda, double betta) : m_lambda(lambda), m_betta(betta)
{
}

EcliptikCoordinates::EcliptikCoordinates(const CartesianCoordinates& cartesian, const CartesianCoordinates& observerPosition)
{
	double x = cartesian.x() - observerPosition.x();
	double y = cartesian.y() - observerPosition.y();
	double z = cartesian.z() - observerPosition.z();

	m_lambda = atan2(y, x);
	if (m_lambda < 0)
	{
		m_lambda = m_lambda + 2 * M_PI;
	}
	m_betta = atan2(z, sqrt(x*x + y*y));
}

EcliptikCoordinates::operator EquatorialCoordinates() const
{
	double k_obliquityOfTheEcliptic = 23.439281 / 180.0 * M_PI;

	double alpha = atan2(
		sin(m_lambda)*cos(k_obliquityOfTheEcliptic) - tan(m_betta)*sin(k_obliquityOfTheEcliptic),
		cos(m_lambda)
		);
	if (alpha < 0)
	{
		alpha = alpha + 2 * M_PI;
	}

	double delta = asin(
		sin(m_betta) * cos(k_obliquityOfTheEcliptic) +
		cos(m_betta) * sin(k_obliquityOfTheEcliptic) * sin(m_lambda)
		);

	return EquatorialCoordinates(alpha, delta);
}

EquatorialCoordinates::EquatorialCoordinates() :m_alpha(0), m_delta(0)
{
}

EquatorialCoordinates::EquatorialCoordinates(double alpha, double delta) :m_alpha(alpha), m_delta(delta)
{
}

HorizontalCoordinates::HorizontalCoordinates() : m_azimuth(0), m_altitude(0)
{
}

HorizontalCoordinates::HorizontalCoordinates(double azimuth, double altitude) : m_azimuth(azimuth), m_altitude(altitude)
{
}

HorizontalCoordinates EquatorialCoordinates::ConvertToHorizontalCoordinates(const GeoCoordinates& observer, double jd) const
{
	double h = getlha(m_alpha, observer.longitude(), jd);
	if (h < 0)
	{
		h = h + 2 * M_PI;
	}
	double azimuth = atan2(
		sin(h), 
		cos(h) * sin(observer.latitude()) - tan(m_delta) * cos(observer.latitude())
	);
	double altitude = asin(sin(observer.latitude()) * sin(m_delta) + cos(observer.latitude()) * cos(m_delta) * cos(h));
	return HorizontalCoordinates(azimuth, altitude);
}

CartesianCoordinates HorizontalCoordinates::ConvertToPolar() const
{
	// Note that Azimuth(A) is measured from the South point, turning positive to the West.
	return CartesianCoordinates(
		sin(m_azimuth) * (M_PI / 2.0 - m_altitude),
		-cos(m_azimuth) * (M_PI / 2.0 - m_altitude),
		0.0);
}

GeoCoordinates::GeoCoordinates() : m_latitude(0), m_longitude(0)
{
}

GeoCoordinates::GeoCoordinates(double latitude, double longitude) : m_latitude(latitude), m_longitude(longitude)
{
}

double ToDeg(double rad)
{
	return rad / M_PI * 180;
}

std::ostream& KeplerOrbits::operator << (std::ostream& os, const CartesianCoordinates& cartesian)
{
	return os << "x: " << cartesian.x() << " y: " << cartesian.y() << " z: " << cartesian.z();
}

std::ostream& KeplerOrbits::operator << (std::ostream& os, const HorizontalCoordinates& horizontal)
{
	return os << "azimuth : " << ToDeg(horizontal.m_azimuth) << " altitude: " << ToDeg(horizontal.m_altitude);
}

std::ostream& KeplerOrbits::operator << (std::ostream& os, const EquatorialCoordinates& equatorial)
{
	return os << "right ascension: " << ToDeg(equatorial.m_alpha) << " declination: " << ToDeg(equatorial.m_delta);
}

std::ostream& KeplerOrbits::operator << (std::ostream& os, const EcliptikCoordinates& ecliptik)
{
	return os << "longitude: " << ToDeg(ecliptik.m_lambda) << " latitude: " << ToDeg(ecliptik.m_betta);
}

double& CartesianCoordinates::x()
{
	return m_x;
};

const double & CartesianCoordinates::x() const
{
	return m_x;
};

double & CartesianCoordinates::y()
{
	return m_y;
};

const double & CartesianCoordinates::y() const
{
	return m_y;
};

double & CartesianCoordinates::z()
{
	return m_z;
};

const double & CartesianCoordinates::z() const
{
	return m_z;
};

double& GeoCoordinates::latitude()
{
	return m_latitude;
}
const double& GeoCoordinates::latitude() const
{
	return m_latitude;
}

double& GeoCoordinates::longitude()
{
	return m_longitude;
}

const double& GeoCoordinates::longitude() const
{
	return m_longitude;
}
