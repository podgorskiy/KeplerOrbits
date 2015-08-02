#include <tinyxml/tinyxml.h>
#include <sstream>
#include "PlotWindow.h"
#include "Resources.h"

class point
{
public: 
	point(double x, double y):x(x),y(y){};
	point():x(0),y(0){};
	float x;
	float y;
};

typedef std::vector<point> Trajectory;
typedef std::vector<Trajectory> VecTrajectory;

int main(int argc,char **argv)
{	
	PlotWinow plot;
		
	plot.Init(800,600,10.f,"KEPLER ORBITS");

	point* points = new point[11];
	for( int i=0; i<11; i++)
	{
		points[i].x = static_cast<float>(i)/10;
		points[i].y = points[i].x*points[i].x;
	}

	Resources res;
	res.Load("data.xml");

	VecTrajectory trajectories;

	int planetCount = res.GetPlanets().size();

	trajectories.resize( planetCount );
	double jd = 2456567.0f;

	for( int i = 0; i < planetCount; i++ )
	{	
		trajectories[i].resize(100);
		const Planet& planet = res.GetPlanets()[i];
		for(int j=0; j<100; j++)
		{
			EulerCoordinateSystem f = planet.getEulaerAngles(jd + j*planet.GetSiderealOrbitPeriod() / 100.0 );
			CartesianCoordinateSystem d = f;
			trajectories[i][j] = point(d.x, d.y);
		}
	}

	int k=0;
	while( !plot.GoingToClose() )
	{
		plot.BeginDraw();
		plot.DrawPlanet(0.0, 0.0, yellow);
		plot.DrawAxes();
		for( int i = 0, l = res.GetPlanets().size(); i < l; i++ )
		{
			jd+=0.3;
			const Planet& p = res.GetPlanets()[i];
			EulerCoordinateSystem f = p.getEulaerAngles(jd);
			CartesianCoordinateSystem d = f;
			plot.DrawPlanet(d.x,d.y, red);
			plot.DrawClosedSequenceOfLines(trajectories[i],red);
		}
		//std::stringstream file;
		//char buff[10];
		//sprintf(buff,"%03d",k++);
		//file<<"img"<<buff<<".bmp";
		//plot.SaveToFile( file.str().c_str() );
		plot.EndDraw(); 
	}
	return 0;
}