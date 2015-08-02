unit Unit2;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ShellApi,WinProcs,Wintypes;

type
  body=record
    name:string;
    azimuth,altitude:double;
    id:byte;
  end;
  star=record
    name:string;
    azimuth,altitude:double;
    m:double;
  end;
  TForm2 = class(TForm)
    Image1: TImage;
    Button1: TButton;
    Timer1: TTimer;
    procedure Button1Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
  private
    { Private declarations }
  public
    procedure circle(x,y,r:double);
    procedure filine(fi:double);
    procedure draw_planet(azimuth,altitude:double;name:string);
    procedure draw_star(s:star);
    procedure simdraw(x,y,i,j:integer);
  end;

var
  Form2: TForm2;
  i,j,k:integer;
  picw,pich:integer;
  R:double;

  bodys:array[0..1000]of body;
  stars:array[0..1000]of star;
  bodysn:integer;
  starsn:integer;

  OutP: TStringList;
  ErrorP: TStringList;

implementation

{$R *.dfm}

procedure TForm2.circle(x,y,r:double);
begin
Image1.Picture.Bitmap.Canvas.Ellipse(round(x-r),round(y-r),round(x+r),round(y+r));
end;


procedure TForm2.filine(fi:double);
begin
Image1.Picture.Bitmap.Canvas.MoveTo(round(R),round(R));
Image1.Picture.Bitmap.Canvas.LineTo(round(cos(fi)*R+R),round(sin(-fi)*R+R));
end;

procedure TForm2.draw_planet(azimuth,altitude:double;name:string);
var
x,y,i,j:integer;
d:byte;
begin
x:=round(cos(azimuth-1.57079632679489)*R*(1-altitude/pi*2)+R);
y:=round(sin(-azimuth+1.57079632679489)*R*(1-altitude/pi*2)+R);
if name='Sun' then begin
Image1.Picture.Bitmap.Canvas.Pen.Style:=psClear;
Image1.Picture.Bitmap.Canvas.Brush.Style:=bsSolid;
Image1.Picture.Bitmap.Canvas.Brush.Color:=$ffff;
d:=7;
Image1.Picture.Bitmap.Canvas.Ellipse(x-d,y-d,x+d,y+d);
end else begin
for i := -1 to 1 do
for j := -2 to 2 do
Image1.Picture.Bitmap.Canvas.Pixels[x+i,y+j]:=$ff;
for j := -1 to 1 do
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y+j]:=$ff;
for j := -1 to 1 do
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y+j]:=$ff;
for j := -1 to 1 do
Image1.Picture.Bitmap.Canvas.Pixels[x+3,y+j]:=$ffffff;
for j := -1 to 1 do
Image1.Picture.Bitmap.Canvas.Pixels[x-3,y+j]:=$ffffff;
for j := -1 to 1 do
Image1.Picture.Bitmap.Canvas.Pixels[x+j,y+3]:=$ffffff;
for j := -1 to 1 do
Image1.Picture.Bitmap.Canvas.Pixels[x+j,y-3]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y+2]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y-2]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y+2]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y-2]:=$ffffff;
end;
end;

procedure TForm2.simdraw(x,y,i,j:integer);
begin
Image1.Picture.Bitmap.Canvas.Pixels[x+i,y+j]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x-i,y+j]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x+i,y-j]:=$ffffff;
Image1.Picture.Bitmap.Canvas.Pixels[x-i,y-j]:=$ffffff;
end;

procedure TForm2.Timer1Timer(Sender: TObject);
begin
Button1Click(Sender);
end;

procedure TForm2.draw_star(s:star);
var
xf,yf:double;
x,y,i,j,x_,y_,m,b,b2,b3,b4,b5,b6,b7,b8:integer;
d:byte;
begin
xf:=cos(s.azimuth-1.57079632679489)*R*(1-s.altitude/pi*2)+R;
x:=round(xf);
yf:=sin(-s.azimuth+1.57079632679489)*R*(1-s.altitude/pi*2)+R;
y:=round(yf);

m:=round(int(s.m));

Image1.Picture.Bitmap.Canvas.Pixels[x,y]:=$ffffff;
if m<=5 then begin

{if m<=4 then begin
simdraw(x,y,0,1);
simdraw(x,y,1,0);
end;
if m<=3 then begin
simdraw(x,y,1,1);
end;
if m<=2 then begin
simdraw(x,y,2,0);
simdraw(x,y,0,2);
end;
if m<=1 then begin
simdraw(x,y,2,1);
simdraw(x,y,1,2);
end;
if m<=0 then begin
simdraw(x,y,2,2);
simdraw(x,y,0,3);
simdraw(x,y,3,0);
simdraw(x,y,3,1);
simdraw(x,y,1,3);
end;
if m<=-1 then begin
simdraw(x,y,0,4);
simdraw(x,y,4,0);
simdraw(x,y,4,1);
simdraw(x,y,1,4);
simdraw(x,y,3,2);
simdraw(x,y,2,3);
simdraw(x,y,3,3);
end;
}
if m>0 then begin
//b:=round(70+(1000-70)*(5-s.m)/(5+1.46));
b:=round(16000/exp(s.m));
b2:=round(b/2);
b3:=round(b/4);
b4:=round(b/8);
b5:=round(b/16);
b6:=round(b/32);
b7:=round(b/64);
b8:=round(b/128);
if b>255 then b:=255;
if b2>255 then b2:=255;
if b3>255 then b3:=255;
if b4>255 then b4:=255;
if b5>255 then b5:=255;
if b6>255 then b6:=255;
if b7>255 then b7:=255;
if b8>255 then b8:=255;

Image1.Picture.Bitmap.Canvas.Pixels[x,y]:=b+$100*b+$10000*b;

if b2>30 then begin

Image1.Picture.Bitmap.Canvas.Pixels[x+1,y]:=b2+$100*b2+$10000*b2;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y]:=b2+$100*b2+$10000*b2;
Image1.Picture.Bitmap.Canvas.Pixels[x,y+1]:=b2+$100*b2+$10000*b2;
Image1.Picture.Bitmap.Canvas.Pixels[x,y-1]:=b2+$100*b2+$10000*b2;

if b3>30 then begin
Image1.Picture.Bitmap.Canvas.Pixels[x+1,y+1]:=b3+$100*b3+$10000*b3;
Image1.Picture.Bitmap.Canvas.Pixels[x+1,y-1]:=b3+$100*b3+$10000*b3;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y+1]:=b3+$100*b3+$10000*b3;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y-1]:=b3+$100*b3+$10000*b3;
if b4>30 then begin
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y]:=b4+$100*b4+$10000*b4;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y]:=b4+$100*b4+$10000*b4;
Image1.Picture.Bitmap.Canvas.Pixels[x,y+2]:=b4+$100*b4+$10000*b4;
Image1.Picture.Bitmap.Canvas.Pixels[x,y-2]:=b4+$100*b4+$10000*b4;
if b5>30 then begin
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y+1]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y-1]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x+1,y+2]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y+2]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y+1]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y-1]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x+1,y-2]:=b5+$100*b5+$10000*b5;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y-2]:=b5+$100*b5+$10000*b5;

if b6>30 then begin
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y+2]:=b6+$100*b6+$10000*b6;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y-2]:=b6+$100*b6+$10000*b6;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y+2]:=b6+$100*b6+$10000*b6;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y-2]:=b6+$100*b6+$10000*b6;
if b7>30 then begin
Image1.Picture.Bitmap.Canvas.Pixels[x+3,y-1]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x+3,y]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x+3,y+1]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x-3,y-1]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x-3,y]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x-3,y+1]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y+3]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x,y+3]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x+1,y+3]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x-1,y-3]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x,y-3]:=b7+$100*b7+$10000*b7;
Image1.Picture.Bitmap.Canvas.Pixels[x+1,y-3]:=b7+$100*b7+$10000*b7;
if b8>30 then begin
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y+3]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y+3]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x+2,y-3]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x-2,y-3]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x+3,y+2]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x-3,y+2]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x+3,y-2]:=b8+$100*b8+$10000*b8;
Image1.Picture.Bitmap.Canvas.Pixels[x-3,y-2]:=b8+$100*b8+$10000*b8;

end;
end;
end;
end;
end;
end;
end;


end;


end;

end;

procedure ExecConsoleApp(CommandLine: AnsiString; Output: TStringList; Errors:
  TStringList);
var
  sa: TSECURITYATTRIBUTES;
  si: TSTARTUPINFO;
  pi: TPROCESSINFORMATION;
  hPipeOutputRead: THANDLE;
  hPipeOutputWrite: THANDLE;
  hPipeErrorsRead: THANDLE;
  hPipeErrorsWrite: THANDLE;
  Res, bTest: Boolean;
  env: array[0..100] of Char;
  szBuffer: array[0..256] of Char;
  dwNumberOfBytesRead: DWORD;
  Stream: TMemoryStream;
begin
  sa.nLength := sizeof(sa);
  sa.bInheritHandle := true;
  sa.lpSecurityDescriptor := nil;
  CreatePipe(hPipeOutputRead, hPipeOutputWrite, @sa, 0);
  CreatePipe(hPipeErrorsRead, hPipeErrorsWrite, @sa, 0);
  ZeroMemory(@env, SizeOf(env));
  ZeroMemory(@si, SizeOf(si));
  ZeroMemory(@pi, SizeOf(pi));
  si.cb := SizeOf(si);
  si.dwFlags := STARTF_USESHOWWINDOW or STARTF_USESTDHANDLES;
  si.wShowWindow := SW_HIDE;
  si.hStdInput := 0;
  si.hStdOutput := hPipeOutputWrite;
  si.hStdError := hPipeErrorsWrite;

  (* Remember that if you want to execute an app with no parameters you nil the
     second parameter and use the first, you can also leave it as is with no
     problems.                                                                 *)
  Res := CreateProcess(nil, pchar(CommandLine), nil, nil, true,
    CREATE_NEW_CONSOLE or NORMAL_PRIORITY_CLASS, @env, nil, si, pi);

  // Procedure will exit if CreateProcess fail
  if not Res then
  begin
    CloseHandle(hPipeOutputRead);
    CloseHandle(hPipeOutputWrite);
    CloseHandle(hPipeErrorsRead);
    CloseHandle(hPipeErrorsWrite);
    Exit;
  end;
  szBuffer[0]:=' ';
  WriteFile(hPipeOutputWrite, szBuffer, 1, dwNumberOfBytesRead,
        nil);

  CloseHandle(hPipeOutputWrite);
  CloseHandle(hPipeErrorsWrite);


  //Read output pipe
  Stream := TMemoryStream.Create;
  try
    while true do
    begin
      bTest := ReadFile(hPipeOutputRead, szBuffer, 256, dwNumberOfBytesRead,
        nil);
      if not bTest then
      begin
        break;
      end;
      Stream.Write(szBuffer, dwNumberOfBytesRead);
    end;
    Stream.Position := 0;
    Output.LoadFromStream(Stream);
  finally
    Stream.Free;
  end;

  //Read error pipe
  Stream := TMemoryStream.Create;
  try
    while true do
    begin
      bTest := ReadFile(hPipeErrorsRead, szBuffer, 256, dwNumberOfBytesRead,
        nil);
      if not bTest then
      begin
        break;
      end;
      Stream.Write(szBuffer, dwNumberOfBytesRead);
    end;
    Stream.Position := 0;
    Errors.LoadFromStream(Stream);
  finally
    Stream.Free;
  end;

  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(hPipeOutputRead);
  CloseHandle(hPipeErrorsRead);
end;




procedure TForm2.Button1Click(Sender: TObject);
var st:string;
starti,endi:integer;
begin
  OutP := TStringList.Create;
  ErrorP := TstringList.Create;
           //solarsystem.exe
  ExecConsoleApp('solarsystem.exe -i', OutP, ErrorP);

  bodysn:=0;
  for i:=0 to OutP.Count do begin
    st:=OutP.Strings[i];
    for j := 1 to length(st) do
      if st[j]<>' ' then break;
    st:=copy(st,j,length(st)-j+1);
    if st='RESULT' then break;
  end;
  starti:=i+2;
  for i := starti to OutP.Count - 1 do
    if OutP.Strings[i]='' then break;
  endi:=i-1;
  for I := starti to endi  do  begin
    st:=OutP.Strings[i];
    for j := 1 to length(st) do
      if st[j]=' ' then break;
    bodys[bodysn].name:=copy(st,1,j-1);
    for j := j to length(st) do
      if st[j]<>' ' then break;
    for k := j to length(st) do
      if st[k]=' ' then break;
    bodys[bodysn].azimuth:=strtofloat(copy(st,j,k-j));
    for j := k to length(st) do
      if st[j]<>' ' then break;
    for k := j to length(st) do
      if st[k]=' ' then break;
    bodys[bodysn].altitude:=strtofloat(copy(st,j,k-j));
    bodysn:=bodysn+1;
  end;

  starsn:=0;

  for i := endi+2 to OutP.Count-1 do begin
    st:=OutP.Strings[i];
    for j := 1 to length(st) do
      if st[j]<>' ' then break;
    for k := j to length(st) do
      if st[k]=' ' then break;
    stars[starsn].azimuth:=strtofloat(copy(st,j,k-j));
    for j := k to length(st) do
      if st[j]<>' ' then break;
    for k := j to length(st) do
      if st[k]=' ' then break;
    stars[starsn].altitude:=strtofloat(copy(st,j,k-j));
    for j := k to length(st) do
      if st[j]<>' ' then break;
    for k := j to length(st) do
      if st[k]=' ' then break;
    stars[starsn].m:=strtofloat(copy(st,j,k-j));
    starsn:=starsn+1;
  end;

    

  OutP.Free;
  ErrorP.Free;

pich:=Form2.Height-40;
picw:=Form2.Width-150;
if pich<picw then picw:=pich;
if pich>picw then pich:=picw;
R:=pich/2;
Image1.Left:=0;
Image1.Top:=0;
Image1.Picture.Bitmap.Width:=picw;
Image1.Picture.Bitmap.Height:=pich;
Image1.Width:=picw;
Image1.Height:=pich;
Image1.Picture.Bitmap.Canvas.Pen.Style:=psClear;
Image1.Picture.Bitmap.Canvas.Brush.Color:=Form2.Color;
Image1.Picture.Bitmap.Canvas.Rectangle(0,0,picw+1,pich+1); 
Image1.Picture.Bitmap.Canvas.Pen.Style:=psSolid;
Image1.Picture.Bitmap.Canvas.Brush.Color:=0;
Image1.Picture.Bitmap.Canvas.Ellipse(0,0,picw,pich);
Image1.Picture.Bitmap.Canvas.Pen.Color:=$707070;
Image1.Picture.Bitmap.Canvas.Brush.Style:=bsClear;
circle(R,R,R/2);
filine(0);
filine(pi/4);
filine(pi/2);
filine(pi*3/4); 
filine(pi);
filine(pi*5/4);
filine(pi*3/2);
filine(pi*7/4);        
Image1.Picture.Bitmap.Canvas.Pen.Color:=$303030;   
circle(R,R,R*3/4);
circle(R,R,R/4);
filine(0+pi/8);
filine(pi/4+pi/8);
filine(pi/2+pi/8);
filine(pi*3/4+pi/8);
filine(pi+pi/8);
filine(pi*5/4+pi/8);
filine(pi*3/2+pi/8);
filine(pi*7/4+pi/8);

for i:=0 to bodysn -1 do   begin
  draw_planet(bodys[i].azimuth,bodys[i].altitude,bodys[i].name);
end;


for i:=0 to starsn -1 do   begin
  draw_star(stars[i]);
end;


end;

end.
