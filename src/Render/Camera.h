#pragma once
#include <entry/entry.h>
#include "ViewType.h"

class Camera
{
public:
	struct MouseCoords
	{
		int m_mx;
		int m_my;
		int m_mz;
	};

	Camera();
	~Camera();

	void Reset();
	
	void Update(const entry::MouseState& _mouseState, bool blockMouse, ViewType::ViewType viewType);
	void UpplyTransform(ViewType::ViewType viewSide = ViewType::FromAboveTheEclipticPlane);
	void SetPosition(const float* _pos);
	void SetFOV(float fov);
	void SetWidthHeight(int width, int height);

	void GetViewProj(float* dest) const;


private:
	MouseCoords m_mouseNow;
	MouseCoords m_mouseLast;

	int m_width;
	int m_height;

	float m_position[3];
	float m_fov;

	float viewProj[16];
	bool m_mouseDown;

};
