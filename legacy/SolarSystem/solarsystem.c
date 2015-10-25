/*
SolarSystem

Author: Stanislav Pidhorskyi (Podgorskiy)
stanislav@podgorskiy.com
stpidhorskyi@mix.wvu.edu

The source code available here: https://github.com/podgorskiy/KeplerOrbits/legacy

This programm written in pure C and it is an ancestor of KeplerOrbites. 

The MIT License (MIT)

Copyright (c) 2015 Stanislav Pidhorskyi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _USE_MATH_DEFINES

#include <math.h>

#define DEG_TO_RAD(X) (X / 180.0 * M_PI)
#define RAD_TO_DEG(X) (X * 180.0 / M_PI)

static const double obliquity_of_the_ecliptic = DEG_TO_RAD(23.439281);

typedef struct euler 
{
	double u, w, i, r;
} euler;

typedef struct deckart 
{
	double x, y, z;
} deckart;

typedef struct ecliptik 
{
	double lambda, betta;
} ecliptik;

typedef struct equatorial 
{
	double alpha, delta;
} equatorial;

typedef struct horizontal 
{
	double a, altitude;
} horizontal;

typedef struct viewer 
{
	deckart d;
	ecliptik e;
} viewer;

typedef struct geocoord 
{
	double lat, long_;
} geocoord;


typedef struct star 
{
	char * name;
	equatorial coord;
	double m;
} star;


typedef struct meanorbitalelements 
{
	double semimajoraxis;
	double eccentricity;
	double inclination;
	double longitudeofascendingnode;
	double longitudeofperihelion;
	double meanlongitude;
	double epoch;
} meanorbitalelements;

typedef struct body
{
	const char * name;
	meanorbitalelements orbit;
	double meanmotion;
	double rotationperiod;
} body;


double jd;
double gmst;
deckart position;
body planet;
body bodys[100];
deckart bodys_positions[100];
int bodysn;
star stars[1000];
int starsn;
int i;
char * st;
viewer pointofview;
ecliptik	geoecliptik_of_body;
equatorial  equatorial_of_body;
horizontal  horizontal_of_body;
double h;
geocoord observer;
deckart sun_position;

double greg2jd(int day, int month, int year, int hour, int minute, int second)
{
	int a, y, m, r;

	a = (14 - month) / 12;
	y = year + 4800 - a;
	m = month + 12 * a - 3;
	r = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
	return r + ((double)(hour - 12)) / 24 + ((double)(minute)) / 1440 + ((double)(second)) / 86400;
}

double getgmst(double jd)
{
	double x;
	x = 18.697374558 + 24.06570982441908 * (jd - 2451545);
	return ((x / 24) - floor(x / 24)) * 24;
}

deckart euler2deckart(euler x)
{
	deckart return_;
	return_.x = x.r*(cos(x.u)*cos(x.w) - sin(x.u)*cos(x.i)*sin(x.w));
	return_.y = x.r*(sin(x.u)*cos(x.w)*cos(x.i) + cos(x.u)*sin(x.w));
	return_.z = x.r*(sin(x.u)*sin(x.i));
	return return_;
}

double getefromm(double m, double eccentricity)
{
	double e, lastDelta, delta, e_;
	for (delta = 1, e_ = m, lastDelta = 2.0, e = 0.0; delta < lastDelta;)
	{
		e = eccentricity*sin(e_) + m;
		lastDelta = delta;
		delta = fabs(e_ - e);
		e_ = e;
	}
	return e;
}

double getbodeccentricity(body* x, double jd)
{
	double meananomaly;
	double currentmeananomaly;

	x->meanmotion = (double)(2 * M_PI) / x->rotationperiod;
	meananomaly = x->orbit.meanlongitude - x->orbit.longitudeofperihelion;
	currentmeananomaly = meananomaly + x->meanmotion*(jd - x->orbit.epoch);
	return getefromm(currentmeananomaly, x->orbit.eccentricity);
}

deckart getbodypositionXYZ(body* x, double e)
{
	double r, argumentofperiapsis, trueanomaly;
	euler euler_angels;

	r = x->orbit.semimajoraxis*(1 - x->orbit.eccentricity*cos(e));
	argumentofperiapsis = x->orbit.longitudeofperihelion - x->orbit.longitudeofascendingnode;
	trueanomaly = 2 * atan2(sqrt(1 + x->orbit.eccentricity)*sin((double)(e) / 2), sqrt(1 - x->orbit.eccentricity)*cos((double)(e) / 2));
	euler_angels.u = trueanomaly + argumentofperiapsis;
	euler_angels.w = x->orbit.longitudeofascendingnode;
	euler_angels.i = x->orbit.inclination;
	euler_angels.r = r;
	return euler2deckart(euler_angels);
}

ecliptik getecliptik(deckart b, viewer v)
{
	double x, y, z;
	ecliptik result;

	x = b.x - v.d.x;
	y = b.y - v.d.y;
	z = b.z - v.d.z;
	result.lambda = atan2(y, x);
	if (result.lambda<0)  result.lambda = result.lambda + 2 * M_PI;
	result.betta = atan2(z, sqrt(x*x + y*y));
	return result;
}

equatorial ecliptik2equatorial(ecliptik x, double e)
{
	equatorial y;

	equatorial ecliptik2equatorial_result;
	y.alpha = atan2((sin(x.lambda)*cos(e) - tan(x.betta)*sin(e)), cos(x.lambda));
	if (y.alpha<0)  y.alpha = y.alpha + 2 * M_PI;

	y.delta = asin(sin(x.betta)*cos(e) + cos(x.betta)*sin(e)*sin(x.lambda));
	ecliptik2equatorial_result = y;
	return ecliptik2equatorial_result;
}

double getlha(double alpha, double lambda)
{
	return (double)(gmst) / 12 * M_PI + lambda - alpha;
}

horizontal equatorial2horisontal(equatorial x, geocoord observer)
{
	horizontal y;
	double h;

	horizontal equatorial2horisontal_result;
	h = getlha(x.alpha, observer.long_);
	if (h<0)  h = h + 2 * M_PI;
	y.a = atan2(sin(h), cos(h)*sin(observer.lat) - tan(x.delta)*cos(observer.lat));
	y.altitude = asin(sin(observer.lat)*sin(x.delta) + cos(observer.lat)*cos(x.delta)*cos(h));
	equatorial2horisontal_result = y;
	return equatorial2horisontal_result;
}


int main(int argc, const char* argv[])
{
	time_t t = time(NULL);

	observer.lat = DEG_TO_RAD(39.6396783);
	observer.long_ = DEG_TO_RAD(-79.95560239999999);

	jd = t / 86400.0 + 2440587.5;
	
	gmst = getgmst(jd);

	printf("JD=%f\n", jd);
	printf("GMST=%f\n", gmst);

	bodysn = 0;
	bodys[bodysn].orbit.semimajoraxis = 0.0;
	bodys[bodysn].orbit.eccentricity = 0.0;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(0.0);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(0.0);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(0.0);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(0.0);
	bodys[bodysn].orbit.epoch = 0;
	bodys[bodysn].rotationperiod = 1.0;
	bodys[bodysn].name = "Sun";
	bodysn++;

	bodys[bodysn].orbit.semimajoraxis = 0.38709893;
	bodys[bodysn].orbit.eccentricity = 0.20563069;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(7.00487);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(48.33167);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(77.45645);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(252.25084);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 87.969;
	bodys[bodysn].name = "Mercury";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 0.72333199;
	bodys[bodysn].orbit.eccentricity = 0.00677323;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(3.39471);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(76.68069);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(131.53298);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(181.97973);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 224.701;
	bodys[bodysn].name = "Venus";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 1.00000011;
	bodys[bodysn].orbit.eccentricity = 0.01671022;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(0.00005);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(-11.26064);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(102.94719);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(100.46435);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 365.256;
	bodys[bodysn].name = "Earth";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 1.52366231;
	bodys[bodysn].orbit.eccentricity = 0.09341233;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(1.85061);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(49.57854);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(336.04084);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(355.45332);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 686.98;
	bodys[bodysn].name = "Mars";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 5.203363011;
	bodys[bodysn].orbit.eccentricity = 0.04839266;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(1.3053);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(100.55615);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(14.75385);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(34.40438);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 4332.589;
	bodys[bodysn].name = "JuM_PIter";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 9.53707032;
	bodys[bodysn].orbit.eccentricity = 0.0541506;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(2.48446);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(113.71504);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(92.43194);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(49.94432);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 10759.22;
	bodys[bodysn].name = "Saturn";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 19.19126393;
	bodys[bodysn].orbit.eccentricity = 0.04716771;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(0.76986);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(74.22988);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(170.96424);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(313.23218);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 30685.4;
	bodys[bodysn].name = "Uranus";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 30.06896348;
	bodys[bodysn].orbit.eccentricity = 0.00858587;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(1.76917);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(131.72169);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(44.97135);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(304.88003);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 60189;
	bodys[bodysn].name = "Neptune";

	bodysn++;
	bodys[bodysn].orbit.semimajoraxis = 39.48168677;
	bodys[bodysn].orbit.eccentricity = 0.24880766;
	bodys[bodysn].orbit.inclination = DEG_TO_RAD(17.14175);
	bodys[bodysn].orbit.longitudeofascendingnode = DEG_TO_RAD(110.30347);
	bodys[bodysn].orbit.longitudeofperihelion = DEG_TO_RAD(224.06676);
	bodys[bodysn].orbit.meanlongitude = DEG_TO_RAD(238.92881);
	bodys[bodysn].orbit.epoch = 2451545;
	bodys[bodysn].rotationperiod = 90465;
	bodys[bodysn].name = "Pluto";
	bodysn++;

	for (i = 0; i <= bodysn - 1; i++) {
		double e = getbodeccentricity(&bodys[i], jd);
		position = getbodypositionXYZ(&bodys[i], e);
		bodys_positions[i] = position;
		printf(" %f %f %f\n", position.x, position.y, position.z);
	}


	pointofview.d = bodys_positions[3];

	printf("                             RESULT\n");

	for (i = 0; i <= bodysn - 1; i++) {
		geoecliptik_of_body = getecliptik(bodys_positions[i], pointofview);
		equatorial_of_body = ecliptik2equatorial(geoecliptik_of_body, obliquity_of_the_ecliptic);
		horizontal_of_body = equatorial2horisontal(equatorial_of_body, observer);
		
		printf("%s:  \n", bodys[i].name);

		if (i != 3)  
		{
			printf("Horizontal: ");
			printf("azimuth %f, altitude %f \n", RAD_TO_DEG(horizontal_of_body.a), RAD_TO_DEG(horizontal_of_body.altitude));
			printf("equatorial: ");
			printf("right ascension %f, altitude %f\n", RAD_TO_DEG(equatorial_of_body.alpha), RAD_TO_DEG(equatorial_of_body.delta));
			printf("Geo Ecliptic: ");
			printf("longitude %f, latitude %f\n", RAD_TO_DEG(geoecliptik_of_body.lambda), RAD_TO_DEG(geoecliptik_of_body.betta));
		}
		
		printf("Descartes: ");
		printf("x: %f, y: %f, z: %f\n", bodys_positions[i].x, bodys_positions[i].y, bodys_positions[i].z);
		printf("\n");
	}

	return 0;
}
