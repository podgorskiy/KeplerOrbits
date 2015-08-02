program solarsystem;
{$APPTYPE CONSOLE}
uses
  SysUtils,windows,math;

const
  epsilon=1e-12;
  obliquity_of_the_ecliptic=23.439281;

type

  euler=record
    u,w,i,r:double;
  end;

  deckart=record
    x,y,z:double;
  end;

  ecliptik=record
    lambda,betta:double;
  end;

  Equatorial=record
    alpha,delta:double;
  end;

  horizontal=record
    A,altitude:double;
  end;

  viewer=record
    d:deckart;
    e:ecliptik;
  end;

  geocoord=record
    lat,long:double;
  end;

  
  star=record
    name:string;
    coord:Equatorial;
    m:double;
  end;

  
  MeanOrbitalElements=record
    Semimajoraxis:double;
    Eccentricity:double;
    inclination:double;
    Longitudeofascendingnode:double;
    Longitudeofperihelion:double;
    MeanLongitude:double;
    Epoch:double;
  end;

  body=record
    name:string;
    orbit:meanorbitalelements;
    meanmotion:double;
    rotationperiod:double;
  end;


var
  UTC: TSystemTime;
  JD:double;
  GMST:double;
  position:deckart;
  planet:body;
  bodys:array[0..1000] of body;
  bodys_positions:array[0..1000] of deckart;
  bodysn:integer;
  stars:array[0..10000] of star;
  starsn:integer;
  i:integer;     
  st:string;
  time:TSystemTime;
  input:text;
  Hnd: Cardinal;
  pointofview:viewer;
  geoecliptik_of_body:ecliptik;
  Equatorial_of_body:Equatorial;
  horizontal_of_body:horizontal;
  h:double;
  Observer:geocoord;
  sun_position:deckart;
  noinput:boolean;

function greg2jd(x:TSystemTime):double;
var a,y,m:integer;
begin
  a:=(14-x.wMonth)div 12;
  y:=x.wYear+4800-a;
  m:=x.wMonth+12*a-3;
  greg2jd:=x.wDay+(153*m+2)div 5+365*y+y div 4 -y div 100+y div 400-32045+
  (x.wHour-12)/24 + x.wMinute/1440 + x.wSecond/86400 + x.wMilliseconds/86400000;
end;

function getGMST(jd:double):double;
var x:double;
begin
  x:= 18.697374558 + 24.06570982441908 * (jd -2451545);
  getGMST := frac(x/24)*24;
end;

function get_jd_now:double;
begin
  GetSystemTime(UTC);
  get_jd_now:=greg2jd(UTC);
end;

function euler2deckart(x:euler):deckart;
var return:deckart;
begin
  return.x:=x.r*(cos(x.u)*cos(x.w)-sin(x.u)*cos(x.i)*sin(x.w));
  return.y:=x.r*(sin(x.u)*cos(x.w)*cos(x.i)+cos(x.u)*sin(x.w));
  return.z:=x.r*(sin(x.u)*sin(x.i));
  euler2deckart:=return;
end;

function getEfromM(M:double;Eccentricity:double):double;
var delta,E,Eold:double;
begin
  delta:=1;
  Eold:=M;
  while delta>epsilon do begin
    E:=Eccentricity*sin(Eold)+M;
    delta:=abs(Eold-E);
    Eold:=E;
  end;
  getEfromM:=E;
end;

function getbodyposition(x:body;jd:double):deckart;
var
  Meananomaly:double;
  CurrentMeananomaly:double;
  u,r,E,Argumentofperiapsis,TrueAnomaly:double;
  position:deckart;
  euler_angels:euler;
begin
  x.meanmotion:=2*pi/x.rotationperiod;
  Meananomaly:=x.orbit.MeanLongitude-x.orbit.Longitudeofperihelion;
  CurrentMeananomaly:=Meananomaly+x.meanmotion*(jd-x.orbit.Epoch);
  E:=getEfromM(CurrentMeananomaly,x.orbit.Eccentricity);
  r:=x.orbit.Semimajoraxis*(1-x.orbit.Eccentricity*cos(E));
  Argumentofperiapsis:=x.orbit.Longitudeofperihelion-x.orbit.Longitudeofascendingnode;
  TrueAnomaly:=2*arctan2(sqrt(1+x.orbit.Eccentricity)*sin(E/2),sqrt(1-x.orbit.Eccentricity)*cos(E/2));
  u:=TrueAnomaly+Argumentofperiapsis;
  euler_angels.u:=u;
  euler_angels.w:=x.orbit.Longitudeofascendingnode;
  euler_angels.i:=x.orbit.inclination;
  euler_angels.r:=r;
  getbodyposition:=euler2deckart(euler_angels);
end;


function getval_float(st:string):double;
var i,j:integer;
st2:string;
begin
  j:=length(st);
  while (byte(st[j])>byte('9'))or(byte(st[j])<byte('0')) do j:=j-1;
  i:=1;
  while ((byte(st[i])>byte('9'))or(byte(st[i])<byte('0') ))and (byte(st[i])<>byte('-') ) do i:=i+1;
  st2:=copy(st,i,j-i+1);
  getval_float:=strtofloat(st2);
end;

procedure load_planet;
label
  end_of_read;
var
  st:string;
  stringblock:array[0..20]of string[255];
  param_names:array[0..20]of string;
  stringblockn:integer;
begin
  SetConsoleTextAttribute(Hnd,15);
  st:='';
  While st='' do begin
    readln(input,st);
    if eof(input) then goto end_of_read;
    writeln(st);
  end;
  bodys[bodysn].name:=st;
  SetConsoleTextAttribute(Hnd,12);
  readln(input,st);
  writeln(st);
  SetConsoleTextAttribute(Hnd,15);

  stringblockn:=0;
  While st<>'end' do begin
    readln(input,st);
    stringblock[stringblockn]:=st;
    stringblockn:=stringblockn+1;
    writeln(st);
  end;

  param_names[0]:='Semimajor axis';
  param_names[1]:='Orbital eccentricity';
  param_names[2]:='Orbital inclination (deg)';
  param_names[3]:='Longitude of ascending node (deg)';
  param_names[4]:='Longitude of perihelion (deg)';
  param_names[5]:='Mean Longitude (deg)';
  param_names[6]:='Epoch';
  param_names[7]:='Sidereal orbit period (days)';

  for i:=0 to stringblockn-1 do begin
    if copy(stringblock[i],1,length(param_names[0]))=param_names[0] then
      bodys[bodysn].orbit.Semimajoraxis:=getval_float(stringblock[i]);
    if copy(stringblock[i],1,length(param_names[1]))=param_names[1] then
      bodys[bodysn].orbit.Eccentricity :=getval_float(stringblock[i]);
    if copy(stringblock[i],1,length(param_names[2]))=param_names[2] then
      bodys[bodysn].orbit.inclination :=getval_float(stringblock[i])/180*pi;
    if copy(stringblock[i],1,length(param_names[3]))=param_names[3] then
      bodys[bodysn].orbit.Longitudeofascendingnode :=getval_float(stringblock[i])/180*pi;
    if copy(stringblock[i],1,length(param_names[4]))=param_names[4] then
      bodys[bodysn].orbit.Longitudeofperihelion :=getval_float(stringblock[i])/180*pi;
    if copy(stringblock[i],1,length(param_names[5]))=param_names[5] then
      bodys[bodysn].orbit.MeanLongitude :=getval_float(stringblock[i])/180*pi;
    if copy(stringblock[i],1,length(param_names[6]))=param_names[6] then
      bodys[bodysn].orbit.Epoch :=getval_float(stringblock[i]);
    if copy(stringblock[i],1,length(param_names[7]))=param_names[7] then
      bodys[bodysn].rotationperiod :=getval_float(stringblock[i]);
  end;

  writeln('');
  write('Load parametrs');
  SetConsoleTextAttribute(Hnd,10);
  writeln(' OK');
  SetConsoleTextAttribute(Hnd,15);
  writeln('');
  bodysn:=bodysn+1;
  end_of_read:
end;




procedure load_star;
var
  st:string;
  i,j,n,n2:integer;
  tmp1,tmp2,tmp3:integer;
  tmpf:double;
begin
  st:='';
  readln(input,st);
  for i := 1 to length(st)-3 do
      if (st[i]='R')and(st[i+1]='A')and(st[i+2]=':') then
        break;
  n:=i;
  stars[starsn].name:=copy(st,0,n-1);

  for I := n+3 to length(st)-3 do
    if (byte(st[i])>=$30)and(byte(st[i])<=$39) then
        break;
  n:=i;
  for I :=n  to length(st)-3 do
    if (st[i]='h') then
        break;

  tmp1:=strtoint(copy(st,n,i-n));

  for I := i to length(st)-3 do
    if (byte(st[i])>=$30)and(byte(st[i])<=$39) then
        break;
  n:=i;
  for I :=n  to length(st)-3 do
    if (st[i]='m') then
        break;

  tmp2:=strtoint(copy(st,n,i-n));

  for I := i to length(st)-3 do
    if ((byte(st[i])>=$30)and(byte(st[i])<=$39))or(st[i]='.') then
        break;
  n:=i;
  for I :=n  to length(st)-3 do
    if (st[i]='s') then
        break;

  tmpf:=strtofloat(copy(st,n,i-n));

  stars[starsn].coord.alpha:=(tmp1+tmp2/60+tmpf/60/60)/12*pi;
  //////////////////////////////////
    
  for i := i to length(st)-3 do
      if (st[i]='D')and(st[i+1]='e')and(st[i+2]='c') then
        break;
  n:=i;

  for I := n to length(st)-3 do
    if (byte(st[i])>=$30)and(byte(st[i])<=$39) then
        break;
  n:=i;
  for I :=n  to length(st)-3 do
    if (st[i]=' ') then
        break;

  tmp1:=strtoint(copy(st,n,i-n));

  for I := i to length(st)-3 do
    if (byte(st[i])>=$30)and(byte(st[i])<=$39) then
        break;
  n:=i;
  for I :=n  to length(st)-3 do
    if (st[i]=' ') then
        break;

  tmp2:=strtoint(copy(st,n,i-n));

  for I := i to length(st)-3 do
    if ((byte(st[i])>=$30)and(byte(st[i])<=$39))or(st[i]='.') then
        break;
  n:=i;
  for I :=n  to length(st)-3 do
    if (st[i]=' ') then
        break;

  tmpf:=strtofloat(copy(st,n,i-n));
                                     
  stars[starsn].coord.delta:=(tmp1+tmp2/60+tmpf/60/60)/180*pi;
  /////////////////////////////////////////////////////

  for i := i to length(st)-3 do
      if (st[i]='m')and(st[i+1]=':') then
        break;
  n:=i;

  for I := n to length(st) do
    if ((byte(st[i])>=$30)and(byte(st[i])<=$39))or(st[i]='.')or(st[i]='-') then
        break;
  n:=i;
  for I :=n  to length(st) do
    if (st[i]=' ')or(i>length(st)) then
        break;

  tmpf:=strtofloat(copy(st,n,i-n));

  stars[starsn].m:=tmpf;

  starsn:=starsn+1;

end;



function getecliptik(b:deckart;v:viewer):ecliptik;
var x,y,z:double;
fi,theta:double;
result_:ecliptik;
begin
  x:=b.x-v.d.x;
  y:=b.y-v.d.y;
  z:=b.z-v.d.z;
  result_.lambda:=arctan2(y,x);
  if result_.lambda<0 then result_.lambda:=result_.lambda+2*pi;
  result_.betta:=arctan2(z,sqrt(sqr(x)+sqr(y)));
  getecliptik:=result_;
end;

function ecliptik2equatorial(x:ecliptik;e:double):Equatorial;
var y:Equatorial;
begin
  y.alpha:=arctan2((sin(x.lambda)*cos(e)-tan(x.betta)*sin(e)),cos(x.lambda));
  if y.alpha<0 then y.alpha:=y.alpha+2*pi;
  
  y.delta:=arcsin(sin(x.betta)*cos(e)+cos(x.betta)*sin(e)*sin(x.lambda));
  ecliptik2equatorial:=y;
end;

function GETLHA(alpha,lambda:double):double;
begin
  GETLHA:=GMST/12*pi+lambda-alpha;
end;

function equatorial2horisontal(x:Equatorial;Observer:geocoord):horizontal;
var y:horizontal;
  h:double;
begin
  h:=GETLHA(x.alpha,Observer.long);
  if h<0 then h:=h+2*pi;
  y.A:=arctan2(sin(h),cos(h)*sin(Observer.lat)-tan(x.delta)*cos(Observer.lat));
  y.altitude:=arcsin(sin(Observer.lat)*sin(x.delta)+cos(Observer.lat)*cos(x.delta)*cos(h));
  equatorial2horisontal:=y;
end;


begin
  Observer.long:=36.329/180*pi;
  Observer.lat:=50.015883/180*pi;

  JD:=get_jd_now;
  
  GMST:=getGMST(JD);

  writeln('JD='+floattostrf(JD,ffFixed,15,5));
  writeln('GMST='+floattostrf(GMST,ffFixed,15,5),', ' + floattostrf(GMST/24*360,ffFixed,15,5)+' grad');
  Hnd:=GetStdHandle(STD_OUTPUT_HANDLE);
  writeln('READING...');
  writeln('');

  assignfile(input,'planets.txt');
  reset(input);

  bodysn:=0;
  while not eof(input) do begin
    load_planet;
  end;

  closefile(input);

  starsn:=0;

  assignfile(input,'Ursa Major.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Ursa Minor.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Orion.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Canis Major.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Hydra.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Gemini.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Virgo.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  assignfile(input,'Cetus.txt');
  reset(input);
  while not eof(input) do begin
    load_star;
  end;

  for I := 0 to bodysn - 1 do begin
    position:=getbodyposition(bodys[i],JD);
    bodys_positions[i]:=position;
    //writeln(' ',position.x,' ',position.y,' ',position.z);
  end;

  pointofview.d:=bodys_positions[2];

  writeln;                          
  SetConsoleTextAttribute(Hnd,10);
  writeln('                             RESULT');     
  SetConsoleTextAttribute(Hnd,15);
  writeln;

  sun_position.x:=0;
  sun_position.y:=0;
  sun_position.z:=0;
  geoecliptik_of_body:=getecliptik(sun_position,pointofview);       
  Equatorial_of_body:=ecliptik2equatorial(geoecliptik_of_body,obliquity_of_the_ecliptic/180*pi);
  horizontal_of_body:=equatorial2horisontal(Equatorial_of_body,Observer);     
  st:='Sun';
  while length(st)<15 do st:=st+' ';
  write(st);
  writeln(' ',horizontal_of_body.A ,' ',horizontal_of_body.altitude ,' ');


  for I := 0 to bodysn - 1 do begin
    if i<>2 then begin
      geoecliptik_of_body:=getecliptik(bodys_positions[i],pointofview);
      Equatorial_of_body:=ecliptik2equatorial(geoecliptik_of_body,obliquity_of_the_ecliptic/180*pi);
      horizontal_of_body:=equatorial2horisontal(Equatorial_of_body,Observer);
      st:=bodys[i].name;
      while length(st)<15 do st:=st+' ';

      write(st);

      writeln(' ',horizontal_of_body.A ,' ',horizontal_of_body.altitude ,' ');
      {
      write('Horizontal: ');
      writeln('azimuth ',floattostrf(horizontal_of_body.A*180/pi,ffFixed,15,5) ,', altitude ',floattostrf(horizontal_of_body.altitude*180/pi,ffFixed,15,5) ,' ');
      write('Equatorial: ');
      writeln('right ascension ',floattostrf(Equatorial_of_body.alpha*180/pi,ffFixed,15,5) ,', altitude ',floattostrf(Equatorial_of_body.delta*180/pi,ffFixed,15,5) ,' ');
      write('Ecliptic: ');
      writeln('longitude ',floattostrf(geoecliptik_of_body.lambda*180/pi,ffFixed,15,5) ,', latitude ',floattostrf(geoecliptik_of_body.betta*180/pi,ffFixed,15,5) ,' ');
      write('Descartes: ');
      writeln('x ',floattostrf(bodys_positions[i].x,ffFixed,15,5) ,', y ',floattostrf(bodys_positions[i].y,ffFixed,15,5) ,'z ',floattostrf(bodys_positions[i].z,ffFixed,15,5));
      }
    end;
  end;

  writeln;

  for I := 0 to starsn - 1 do begin
      horizontal_of_body:=equatorial2horisontal(stars[i].coord,Observer);
      //write(stars[i].name+': ');
      //if stars[i].m<=3.5 then
      writeln(' ',horizontal_of_body.A ,' ',horizontal_of_body.altitude ,' ',stars[i].m);
  end;

  {
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
  }      

  for I := 1 to paramcount do begin
    if paramstr(i)='-i' then noinput:=true;
  end;
  if noinput=false then
  readln;


end.
