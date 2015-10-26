#define _USE_MATH_DEFINES

#include "Camera.h"

#include <entry/entry.h>
#include <bx/timer.h>
#include <bx/fpumath.h>

#include <entry/entry.h>
#include <bgfx_utils.h>

#include <bgfx.h>
#include <bx/timer.h>
#include <bx/fpumath.h>

Camera::Camera()
{
	Reset();
	entry::MouseState mouseState;
	Update(mouseState, false, ViewType::FromAboveTheEclipticPlane);
};

Camera::~Camera()
{};

void Camera::Reset()
{
	m_mouseNow.m_mx = 0;
	m_mouseNow.m_my = 0;
	m_mouseNow.m_mz = 0;
	m_mouseLast.m_mx = 0;
	m_mouseLast.m_my = 0;
	m_mouseLast.m_mz = 0;
	m_position[0] = 0;
	m_position[1] = 0;
	m_position[2] = 0;
	m_fov = 30.0f;
	m_mouseDown = false;
};

void Camera::Update(const entry::MouseState& _mouseState, bool blockMouse, ViewType::ViewType viewType)
{
	if (!m_mouseDown)
	{
		m_mouseLast.m_mx = _mouseState.m_mx;
		m_mouseLast.m_my = _mouseState.m_my;
	}

	m_mouseDown = !!_mouseState.m_buttons[entry::MouseButton::Right] || !!_mouseState.m_buttons[entry::MouseButton::Left];

	if (m_mouseDown)
	{
		m_mouseNow.m_mx = _mouseState.m_mx;
		m_mouseNow.m_my = _mouseState.m_my;
	}  

	m_mouseNow.m_mz = _mouseState.m_mz;
	int32_t deltaZ = m_mouseNow.m_mz - m_mouseLast.m_mz;
	if(deltaZ >100)
	{
		deltaZ = 100;
	}
	if(deltaZ<-100)
	{
		deltaZ = -100;
	}
	m_fov *= (1.0f - deltaZ / 20.0f / 100.0f);
	if(m_fov<0.001)
	{
		m_fov = 0.001;
	}
	m_mouseLast.m_mz = m_mouseNow.m_mz;

	if (m_mouseDown)
	{
		int32_t deltaX = m_mouseNow.m_mx - m_mouseLast.m_mx;
		int32_t deltaY = m_mouseNow.m_my - m_mouseLast.m_my;

		if (deltaX != 0)
		{
			deltaX = deltaX;
		}
		if (!blockMouse)
		{
			switch (viewType)
			{
			case ViewType::FromAboveTheEclipticPlane:
				m_position[0] -= float(deltaX) / m_height * m_fov;
				m_position[1] += float(deltaY) / m_height * m_fov;
				break;
			case ViewType::FromTheEdgeOfTheEclipticPlane:
				m_position[0] -= float(deltaX) / m_height * m_fov;
				m_position[2] += float(deltaY) / m_height * m_fov;
				break;
			case ViewType::FromTheDirectionOfVernalEquinox:
				m_position[1] -= float(deltaX) / m_height * m_fov;
				m_position[2] += float(deltaY) / m_height * m_fov;
				break;
			case ViewType::FormObserverOnEarth:
			default:
				break;
			}
		}

		m_mouseLast.m_mx = m_mouseNow.m_mx;
		m_mouseLast.m_my = m_mouseNow.m_my;
	}
};

void Camera::UpplyTransform(ViewType::ViewType viewType)
{
	float eye[3] = { 0, 0, 0 };
	float up[3] = { 0, 0, 0 };
	float at[3] = { 0, 0, 0 };
	switch (viewType)
	{
	case ViewType::FromAboveTheEclipticPlane:
		eye[2] = -1.0f;
		up[1] = 1.0f;
		break;
	case ViewType::FromTheEdgeOfTheEclipticPlane:
		eye[1] = 1.0f;
		up[2] = 1.0f;
		break;
	case ViewType::FromTheDirectionOfVernalEquinox:
		eye[0] = -1.0f;
		up[2] = 1.0f;
		break;
	case ViewType::FormObserverOnEarth:
		eye[2] = -1.0f;
		up[1] = 1.0f;
		break;
	}

	float w, h;

	if (viewType != ViewType::FormObserverOnEarth)
	{
		for (int i = 0; i < 3; i++)
		{
			eye[i] += m_position[i];
			at[i] = m_position[i];
		}

		w = m_width * m_fov / m_height;
		h = m_fov;
	}
	else
	{
		w = m_width * static_cast<float>(M_PI) / m_height;
		h = M_PI;
	}

	float view[16];
	bx::mtxLookAt(view, eye, at, up);

	float ortho[16];
	bx::mtxOrtho(ortho, -w / 2, w / 2, -h / 2, h / 2, -1000.0f, 1000.0f);
	bgfx::setViewTransform(0, view, ortho);


	bx::mtxMul(viewProj, view, ortho);

	bgfx::setViewRect(0, 0, 0, m_width, m_height);
};

void Camera::GetViewProj(float* dest) const
{
	memcpy(dest, viewProj, sizeof(viewProj));
}

void Camera::SetPosition(const float* _pos)
{
	memcpy(m_position, _pos, sizeof(float) * 2);
};

void Camera::SetFOV(float fov)
{
	m_fov = fov;
};

void Camera::SetWidthHeight(int width, int height)
{
	m_width = width;
	m_height = height;
};
