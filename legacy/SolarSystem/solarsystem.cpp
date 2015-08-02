#include <time.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

using namespace std;

#define pi 3.1415926535897932384626433832795

static const double epsilon = 1e-12;
static const double obliquity_of_the_ecliptic = 23.439281;

typedef struct euler {
  double u,w,i,r;
} euler;

typedef struct deckart {
  double x,y,z;
} deckart;

typedef struct ecliptik {
  double lambda,betta;
} ecliptik;

typedef struct equatorial {
  double alpha,delta;
} equatorial;

typedef struct horizontal {
  double a,altitude;
} horizontal;

typedef struct viewer {
  deckart d;
  ecliptik e;
} viewer;

typedef struct geocoord {
  double lat,long_;
} geocoord;


typedef struct star {
  char * name;
  equatorial coord;
  double m;
} star;


typedef struct meanorbitalelements {
  double semimajoraxis;
  double eccentricity;
  double inclination;
  double longitudeofascendingnode;
  double longitudeofperihelion;
  double meanlongitude;
  double epoch;
} meanorbitalelements;

typedef struct body {
  const char * name;
  meanorbitalelements orbit;
  double meanmotion;
  double rotationperiod;
} body;


//tsystemtime utc;
double jd;
double gmst;
deckart position;
body planet;
body bodys[1001];
deckart bodys_positions[1001];
int bodysn;
star stars[10001];
int starsn;
int i;     
char * st;
//tsystemtime time1;
//text input1 = VOID_FILE;
//cardinal hnd;
viewer pointofview;
ecliptik	geoecliptik_of_body;
equatorial  equatorial_of_body;
horizontal  horizontal_of_body;
double h;
geocoord observer;
deckart sun_position;
//boolean noinput;

double greg2jd(int day, int month, int year, int hour, int minute, int second)
{
    int a,y,m;

  a=(14-month)/ 12;
  y=year+4800-a;
  m=month+12*a-3;
  int r = day+(153*m+2)/ 5+365*y+y / 4 -y / 100+y / 400-32045;
  return r+((double)(hour-12))/24 + ((double)(minute))/1440 + ((double)(second))/86400;
}

double getgmst(double jd)
{
  double x;
  x= 18.697374558 + 24.06570982441908 * (jd -2451545);
  return ((x/24)-floor(x/24))*24;
}

deckart euler2deckart(euler x)
{
  deckart return_;
  return_.x=x.r*(cos(x.u)*cos(x.w)-sin(x.u)*cos(x.i)*sin(x.w));
  return_.y=x.r*(sin(x.u)*cos(x.w)*cos(x.i)+cos(x.u)*sin(x.w));
  return_.z=x.r*(sin(x.u)*sin(x.i));
  return return_;
}

double getefromm(double m,double eccentricity)
{
  double e=0;
  double delta=1;
  double eold=m;
  while (delta>epsilon)  {
    e=eccentricity*sin(eold)+m;
    delta=abs(eold-e);
    eold=e;
  }
  return e;
}


double getbodeccentricity(body* x,double jd)
{
  double meananomaly;
  double currentmeananomaly;

  x->meanmotion=(double)(2*pi)/x->rotationperiod;
  meananomaly=x->orbit.meanlongitude-x->orbit.longitudeofperihelion;
  currentmeananomaly=meananomaly+x->meanmotion*(jd-x->orbit.epoch);
  return getefromm(currentmeananomaly,x->orbit.eccentricity);
}

deckart getbodypositionXYZ(body* x,double e)
{
  double r,argumentofperiapsis,trueanomaly;
  euler euler_angels;
   
  r=x->orbit.semimajoraxis*(1-x->orbit.eccentricity*cos(e));
  argumentofperiapsis=x->orbit.longitudeofperihelion-x->orbit.longitudeofascendingnode;
  trueanomaly=2*atan2(sqrt(1+x->orbit.eccentricity)*sin((double)(e)/2),sqrt(1-x->orbit.eccentricity)*cos((double)(e)/2));
  euler_angels.u=trueanomaly+argumentofperiapsis;
  euler_angels.w=x->orbit.longitudeofascendingnode;
  euler_angels.i=x->orbit.inclination;
  euler_angels.r=r;
  return euler2deckart(euler_angels);
}


ecliptik getecliptik(deckart b,viewer v)
{
  double x,y,z;
  ecliptik result;

  x=b.x-v.d.x;
  y=b.y-v.d.y;
  z=b.z-v.d.z;
  result.lambda=atan2(y,x);
  if (result.lambda<0)  result.lambda=result.lambda+2*pi;
  result.betta=atan2(z,sqrt(x*x+y*y));
  return result;
}

equatorial ecliptik2equatorial(ecliptik x,double e)
{
    equatorial y;

  equatorial ecliptik2equatorial_result;
  y.alpha=atan2((sin(x.lambda)*cos(e)-tan(x.betta)*sin(e)),cos(x.lambda));
  if (y.alpha<0)  y.alpha=y.alpha+2*pi;

  y.delta=asin(sin(x.betta)*cos(e)+cos(x.betta)*sin(e)*sin(x.lambda));
  ecliptik2equatorial_result=y;
  return ecliptik2equatorial_result;
}

double getlha(double alpha,double lambda)
{
  return (double)(gmst)/12*pi+lambda-alpha;
}

horizontal equatorial2horisontal(equatorial x,geocoord observer)
{
    horizontal y;
  double h;

  horizontal equatorial2horisontal_result;
  h=getlha(x.alpha,observer.long_);
  if (h<0)  h=h+2*pi;
  y.a=atan2(sin(h),cos(h)*sin(observer.lat)-tan(x.delta)*cos(observer.lat));
  y.altitude=asin(sin(observer.lat)*sin(x.delta)+cos(observer.lat)*cos(x.delta)*cos(h));
  equatorial2horisontal_result=y;
  return equatorial2horisontal_result;
}


int main(int argc, const char* argv[])
{
	char day[3],month[3],year[5],hour[3],minute[3];
	bool use_d=false;
	bool use_t=false;
	if (argc>1){  
		for (int i=1;i<argc;i++){
			if (argv[i][0]=='-'){
				switch (argv[i][1]){
				case 'd' : if (strlen(argv[i+1])==10){
								strncpy(month,argv[i+1],2);
								month[2] = 0;
								strncpy(day,argv[i+1]+3,2);
								day[2] = 0;
								strncpy(year,argv[i+1]+6,4);
								year[4] = 0;
								use_d=true;
						   }
				case 't' : if (strlen(argv[i+1])==5){
								strncpy(hour,argv[i+1],2);
								hour[2] = 0;
								strncpy(minute,argv[i+1]+3,2);
								minute[2] = 0;
								use_t=true;
						   }
				}
			}
		}
	}

  //pio_initialize(argc, argv);
  observer.long_=36.329/180*pi;
  observer.lat=50.015883/180*pi;
  
  if (use_d==true){
	if (use_t==true){
		jd=greg2jd(atoi(day),atoi(month),atoi(year),atoi(hour),atoi(minute),0);
		printf("Date %d/%d/%d\n", atoi(day),atoi(month),atoi(year));
		printf("Time %d:%d\n", atoi(hour),atoi(minute));
	}else{
		jd=greg2jd(atoi(day),atoi(month),atoi(year),12,0,0);
		printf("Date %d/%d/%d\n", atoi(day),atoi(month),atoi(year));
		printf("Time 12:00\n");
	}
  } else {
	if (use_t==true){
		time_t t = time(NULL);
		jd = t/86400 + 2440587.5 + atoi(hour)/24.0 + atoi(minute)/24.0/60.0;
		printf("Time %d:%d\n", atoi(hour),atoi(minute));
	}else{	
		time_t t = time(NULL);
		jd = t/86400.0 + 2440587.5;
	}
  }
   
  gmst=getgmst(jd);

  printf("JD=%f\n", jd);

  printf("GMST=%f\n", gmst);

  


bodysn= 0;
bodys[bodysn]	.orbit.	semimajoraxis = 	0.38709893	;
bodys[bodysn]	.orbit.	eccentricity = 	0.20563069	;
bodys[bodysn]	.orbit.	inclination = 	7.00487/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	48.33167/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	77.45645/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	252.25084/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	87.969	;
bodys[bodysn]	.name = "Mercury";
				
bodysn++;				
bodys[bodysn]	.orbit.	semimajoraxis = 	0.72333199	;
bodys[bodysn]	.orbit.	eccentricity = 	0.00677323	;
bodys[bodysn]	.orbit.	inclination = 	3.39471/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	76.68069/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	131.53298/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	181.97973/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	224.701	;
bodys[bodysn]	.name = "Venus";
				
bodysn++;	
bodys[bodysn]	.orbit.	semimajoraxis = 	1.00000011	;
bodys[bodysn]	.orbit.	eccentricity = 	0.01671022	;
bodys[bodysn]	.orbit.	inclination = 	0.00005/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	-11.26064/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	102.94719/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	100.46435/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	365.256	;
bodys[bodysn]	.name = "Earth";
				
bodysn++;	
bodys[bodysn]	.orbit.	semimajoraxis = 	1.52366231	;
bodys[bodysn]	.orbit.	eccentricity = 	0.09341233	;
bodys[bodysn]	.orbit.	inclination = 	1.85061/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	49.57854/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	336.04084/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	355.45332/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	686.98	;
bodys[bodysn]	.name = "Mars";
				
bodysn++;	
bodys[bodysn]	.orbit.	semimajoraxis = 	5.203363011	;
bodys[bodysn]	.orbit.	eccentricity = 	0.04839266	;
bodys[bodysn]	.orbit.	inclination = 	1.3053/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	100.55615/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	14.75385/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	34.40438/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	4332.589	;
bodys[bodysn]	.name = "Jupiter";

bodysn++;	
bodys[bodysn]	.orbit.	semimajoraxis = 	9.53707032	;
bodys[bodysn]	.orbit.	eccentricity = 	0.0541506	;
bodys[bodysn]	.orbit.	inclination = 	2.48446/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	113.71504/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	92.43194/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	49.94432/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	10759.22	;
bodys[bodysn]	.name = "Saturn";

bodysn++;	
bodys[bodysn]	.orbit.	semimajoraxis = 	19.19126393	;
bodys[bodysn]	.orbit.	eccentricity = 	0.04716771	;
bodys[bodysn]	.orbit.	inclination = 	0.76986/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	74.22988/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	170.96424/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	313.23218/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	30685.4	;
bodys[bodysn]	.name = "Uranus";
		
bodysn++;	
bodys[bodysn]	.orbit.	semimajoraxis = 	30.06896348	;
bodys[bodysn]	.orbit.	eccentricity = 	0.00858587	;
bodys[bodysn]	.orbit.	inclination = 	1.76917	/180*pi;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	131.72169/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	44.97135/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	304.88003/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	60189	;
bodys[bodysn]	.name = "Neptune";
				
bodysn++;		
bodys[bodysn]	.orbit.	semimajoraxis = 	39.48168677	;
bodys[bodysn]	.orbit.	eccentricity = 	0.24880766	;
bodys[bodysn]	.orbit.	inclination = 	17.14175/180*pi	;
bodys[bodysn]	.orbit.	longitudeofascendingnode = 	110.30347/180*pi	;
bodys[bodysn]	.orbit.	longitudeofperihelion = 	224.06676/180*pi	;
bodys[bodysn]	.orbit.	meanlongitude = 	238.92881/180*pi	;
bodys[bodysn]	.orbit.	epoch = 	2451545	;
bodys[bodysn]	.	rotationperiod = 	90465	;	
bodys[bodysn]	.name = "Pluto";
bodysn++;	

  
  /*
  assignfile(input1,"planets.txt");
  reset(input1, 0, 0, 0);

  bodysn=0;
  while (! eof(input1))  {
    load_planet();
  }

  closefile(input1);

  starsn=0;

  assignfile(input1,"Ursa Major.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Ursa Minor.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Orion.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Canis Major.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Hydra.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Gemini.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Virgo.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }

  assignfile(input1,"Cetus.txt");
  reset(input1, 0, 0, 0);
  while (! eof(input1))  {
    load_star();
  }
  */


  for( i = 0; i <= bodysn - 1; i ++) {
	double e = getbodeccentricity(&bodys[i],jd);
    position=getbodypositionXYZ(&bodys[i],e);
    printf(" planet %s %f %f %f\n",bodys[i].name,position.x,position.y,position.z);
	for( int j = 0; j <= 200; j ++) {
		position=getbodypositionXYZ(&bodys[i],j/200.0*2*pi);
		printf(" %f %f %f\n",position.x,position.y,position.z);
	}
  }

  /*

    for( i = 0; i <= bodysn - 1; i ++) {

    position=getbodyposition(bodys[i],jd);
    bodys_positions[i]=position;
    printf(" %f %f %f\n",position.x,position.y,position.z);
  }


  pointofview.d=bodys_positions[2];

  printf("                             RESULT\n");     
  
  sun_position.x=0;
  sun_position.y=0;
  sun_position.z=0;
  geoecliptik_of_body=getecliptik(sun_position,pointofview);       
  equatorial_of_body=ecliptik2equatorial(geoecliptik_of_body,obliquity_of_the_ecliptic/180*pi);
  horizontal_of_body=equatorial2horisontal(equatorial_of_body,observer);     
  
  printf("Sun %f, %f \n", horizontal_of_body.a,horizontal_of_body.altitude);


  for( i = 0; i <= bodysn - 1; i ++) {
    if (i!=2)  {
      geoecliptik_of_body=getecliptik(bodys_positions[i],pointofview);
      equatorial_of_body=ecliptik2equatorial(geoecliptik_of_body,obliquity_of_the_ecliptic/180*pi);
      horizontal_of_body=equatorial2horisontal(equatorial_of_body,observer);
      //st=bodys[i].name;
	  	  
	  printf("%f, %f \n", horizontal_of_body.a,horizontal_of_body.altitude);

      write('Horizontal: ');
      writeln('azimuth ',floattostrf(horizontal_of_body.A*180/pi,ffFixed,15,5) ,', altitude ',floattostrf(horizontal_of_body.altitude*180/pi,ffFixed,15,5) ,' ');
      write('Equatorial: ');
      writeln('right ascension ',floattostrf(Equatorial_of_body.alpha*180/pi,ffFixed,15,5) ,', altitude ',floattostrf(Equatorial_of_body.delta*180/pi,ffFixed,15,5) ,' ');
      write('Ecliptic: ');
      writeln('longitude ',floattostrf(geoecliptik_of_body.lambda*180/pi,ffFixed,15,5) ,', latitude ',floattostrf(geoecliptik_of_body.betta*180/pi,ffFixed,15,5) ,' ');
      write('Descartes: ');
      writeln('x ',floattostrf(bodys_positions[i].x,ffFixed,15,5) ,', y ',floattostrf(bodys_positions[i].y,ffFixed,15,5) ,'z ',floattostrf(bodys_positions[i].z,ffFixed,15,5));
 
//	}
 // }

  cwrite("\n");

  for( i = 0; i <= starsn - 1; i ++) {
      horizontal_of_body=equatorial2horisontal(stars[i].coord,observer);
\\write(stars[i].name+': ');      \\if stars[i].m<=3.5 then      cwrite(" %? %? %?\n", horizontal_of_body.a,horizontal_of_body.altitude,stars[i].m);
  }


  for I := 0 to 1000 do begin
      position:=getbodyposition(bodys[2],JD+i);
      pointofview.d:=position;
      position:=getbodyposition(bodys[0],JD+i);
      geoecliptik_of_body:=getecliptik(position,pointofview);
      Equatorial_of_body:=ecliptik2equatorial(geoecliptik_of_body,23.439281/180*pi);
      horizontal_of_body:=equatorial2horisontal(Equatorial_of_body,Observer);
    //writeln(' ',geoecliptik_of_body.lambda,' ',geoecliptik_of_body.betta ,' ');
      //writeln(' ',Equatorial_of_body.alpha,' ',Equatorial_of_body.delta ,' ');
      writeln(' ',horizontal_of_body.A ,' ',horizontal_of_body.altitude ,' ');
  end;

  for( i = 1; i <= paramcount; i ++) {
    if (paramstr(i)=="-i")  noinput=true;
  }
  if (noinput==false) 
  cread("\n");

  */
  return 0;
}
