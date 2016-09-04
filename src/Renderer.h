#pragma once

#include "OceanObject.h"

class CCamera;


class CRenderer
{
public:
	CRenderer (int width, int height, const CCamera &cam);

	void Update (float currentTime);
    
    void DebugSave (const char *filename);

private:
    const CCamera &mCamera;
	COceanObject mOcean;
};
