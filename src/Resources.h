#pragma once
#include "KeplerOrbits/Body.h"
#include <vector>

class TiXmlHandle;

class Resources
{
public:

	struct RenderProperties
	{
		float orbitColorF[3];
		unsigned int orbitColorI;
	};

	typedef std::vector<std::pair<KeplerOrbits::Body, RenderProperties> > BodyList;

	Resources();
	
	~Resources();
	
	void Load(const char* fileName);

	const BodyList& GetBodies() const;

private:
	int GetIVal(TiXmlHandle& handle, const char* name) const;
	int GetHVal(TiXmlHandle& handle, const char* name) const;
	double GetDVal(TiXmlHandle& handle, const char* name) const;
	double GetRVal(TiXmlHandle& handle, const char* name) const;
	
	BodyList m_bodies;
};