#include <cassert>

const unsigned char
    wight[] = { 255,255,255 },
    red[]   = { 255,0,0 },
    yellow[]   = { 255,255,0 },
    green[] = { 0,255,0 },
    blue [] = { 0,0,255 },
    black[] = { 0,0,0 };

namespace cimg_library
{
	class CImgDisplay;
	template <class T> class CImg;
}

typedef cimg_library::CImg<unsigned char> Canvas; 

class PlotWinow
{
public:
	PlotWinow():m_disp(0),m_canvas(0){};
	~PlotWinow();
	void Init(int width, int height, float size, const char* title);
	void BeginDraw();
	void EndDraw();
	bool GoingToClose() const;
	void toScreen(float w_x, float w_y, int& s_x, int& s_y) const;
	void toWorld(float& w_x, float& w_y, int s_x, int s_y) const;
	void DrawAxes() const;
	void DrawLine(float wx0, float wy0, float wx1, float wy1, const unsigned char* color) const;
	template<typename T>
	void DrawSequenceOfLines(int n, const T* points, const unsigned char* color) const;
	template<typename T>
	void DrawSequenceOfLines(const T& points, const unsigned char* color) const;
	template<typename T>
	void DrawClosedSequenceOfLines(const T& points, const unsigned char* color) const;
	void DrawPlanet(float wx, float wy, const unsigned char* color) const;
	void SaveToFile(const char* filename) const;
private:
	void DrawLineScreenSpace(int x0, int y0, int x1, int y1, const unsigned char* color) const;
	cimg_library::CImgDisplay* m_disp;
	float	m_worldOffset_x;
	float	m_worldOffset_y;
	float	m_worldHeight;
	float	m_size;
	float	m_aspect;
	Canvas* m_canvas;
};

template<typename T>
void PlotWinow::DrawSequenceOfLines(int n, const T* points, const unsigned char* color) const
{
	int x0,y0,x1,y1;
	toScreen(points[0].x, points[0].y, x0, y0);
	for( int i=1; i < n; i++ )
	{
		toScreen(points[i].x, points[i].y,x1,y1);
		DrawLineScreenSpace(x0, y0, x1, y1, color);
		x0 = x1;
		y0 = y1;
	}
}
template<typename T>
void PlotWinow::DrawSequenceOfLines(const T& points, const unsigned char* color) const
{
	int x0,y0,x1,y1;
	toScreen(points[0].x, points[0].y, x0, y0);
	for( int i=1, l = points.size(); i < l; i++ )
	{
		toScreen(points[i].x, points[i].y,x1,y1);
		DrawLineScreenSpace(x0, y0, x1, y1, color);
		x0 = x1;
		y0 = y1;
	}
}
template<typename T>
void PlotWinow::DrawClosedSequenceOfLines(const T& points, const unsigned char* color) const
{
	int x0,y0,x1,y1;
	toScreen(points[0].x, points[0].y, x0, y0);
	for( int i=1, l = points.size(); i < l; i++ )
	{
		toScreen(points[i].x, points[i].y,x1,y1);
		DrawLineScreenSpace(x0, y0, x1, y1, color);
		x0 = x1;
		y0 = y1;
	}
	toScreen(points.back().x, points.back().y, x0, y0);
	toScreen(points[0].x, points[0].y,x1,y1);
	DrawLineScreenSpace(x0, y0, x1, y1, color);
}
