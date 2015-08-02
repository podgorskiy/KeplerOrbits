#include <cstring>
#include <string>
#include <math.h>
#define pi 3.1415926535897932384626433832795f
#define epsilon 1e-12f


class OrbitalElements	
{	
	friend class EulerCoordinateSystem;	
public:	
	OrbitalElements& EllipseShape(double eccentricity, double semimajorAxis)	
	{	
		m_eccentricity = eccentricity;	
		m_semimajorAxis = semimajorAxis;	
		return *this;	
	};	
	OrbitalElements& OrbitalPlane(double inclination, double longitudeOfAscendingNode)	
	{	
		m_inclination = inclination;	
		m_longitudeOfAscendingNode = longitudeOfAscendingNode;	
		return *this;	
	};	
	OrbitalElements& EllipseOrientation(double longitudeOfPerihelion)	
	{	
		m_longitudeOfPerihelion = longitudeOfPerihelion;	
		return *this;	
	};	
	OrbitalElements& TimeSpecific(double meanLongitude, double epoch)	
	{	
		m_meanLongitude = meanLongitude;	
		m_epoch = epoch;	
		return *this;	
	};	
	double GetMeanAnomaly() const	
	{	
		return m_meanLongitude-m_longitudeOfPerihelion;	
	};	
	double GetTSE(double jd) const	
	{	
		return jd - m_epoch;	
	};	
	double GetEccentricAnomaly( double m ) const	
	{	
		double e;	
		for( double delta=1, e_ = m; delta>epsilon;)  	
		{	
			e = m_eccentricity * sin( e_ ) + m;	
			delta = abs( e_ - e );	
			e_ = e;	
		}	
		return e;	
	};	
	double GetRadius( double eccentricAnomaly ) const	
	{	
		return m_semimajorAxis * ( 1 - m_eccentricity * cos( eccentricAnomaly ) );	
	};	
	double GetArgumentOfPeriapsis() const	
	{	
		return m_longitudeOfPerihelion - m_longitudeOfAscendingNode;	
	};	
	double GetTrueAnomaly( double eccentricAnomaly ) const	
	{	
		return 2.0f * atan2(	sqrt( 1.0f + m_eccentricity ) * sin( eccentricAnomaly / 2 ), 
								sqrt( 1.0f - m_eccentricity ) * cos( eccentricAnomaly / 2 ) );
	};	

private:	
	double m_eccentricity;	
	double m_semimajorAxis;	

	double m_inclination;	
	double m_longitudeOfAscendingNode;	
		
	double m_longitudeOfPerihelion;	
	double m_meanLongitude;	
	double m_epoch;	
};	


class CartesianCoordinateSystem
{
public:
	CartesianCoordinateSystem(double x, double y, double z):x(x),y(y),z(z){};
	double x, y, z;
};

class EulerCoordinateSystem
{
public:
	EulerCoordinateSystem(double phi, double theta, double psi, double r):phi(phi),theta(theta),psi(psi),r(r){};
	EulerCoordinateSystem(double phi, const OrbitalElements& orbitalElements, double r)
		:phi(phi),theta(orbitalElements.m_longitudeOfAscendingNode),psi(orbitalElements.m_inclination),r(r){};
	operator CartesianCoordinateSystem() const
	{
		float x = r * ( cos(phi) * cos(theta) - sin(phi) * cos(psi) * sin(theta) );
		float y = r * ( sin(phi) * cos(theta) * cos(psi) + cos(phi) * sin(theta) );
		float z = r * ( sin(phi) * sin(psi) ); 
		return CartesianCoordinateSystem( x, y, z );
	};
	double phi, theta, psi, r;
};

class Planet : OrbitalElements
{
public:
	Planet(	const char*			name,
			OrbitalElements&	orbitalElements,
			double				siderealOrbitPeriod):	m_name(name),
														OrbitalElements(orbitalElements),
														m_siderealOrbitPeriod(siderealOrbitPeriod){};
	~Planet(){};
	EulerCoordinateSystem const& getEulaerAngles(double jd) const
	{
		double meanMotion			= 2 * pi / m_siderealOrbitPeriod;
		double currentMeanAnomaly	= GetMeanAnomaly() + meanMotion * GetTSE( jd );

		double eccentricAnomaly		= GetEccentricAnomaly( currentMeanAnomaly );
		double trueAnomaly			= GetTrueAnomaly( eccentricAnomaly );
		double argumentOfPeriapsis	= GetArgumentOfPeriapsis();

		double psi = trueAnomaly + argumentOfPeriapsis;
		
		double r = GetRadius( eccentricAnomaly );
		return EulerCoordinateSystem( psi, *this, r );	
	};
	double GetSiderealOrbitPeriod() const
	{
		return m_siderealOrbitPeriod;
	}
private:
	std::string m_name;
	double m_siderealOrbitPeriod;
};