#pragma once

#include "OceanObject.h"


class CRenderer
{
public:
	CRenderer ();
	void Render ();

private:
	COceanObject mOcean;
};
