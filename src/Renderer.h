#pragma once

#include "OceanObject.h"

class CCamera;


class CRenderer
{
public:
	CRenderer (int width, int height, const CCamera &cam);

	void Draw (float currentTime, bool noUpdate);
    
    void DebugSave (const char *filename);

private:
    const CCamera &mCamera;
	COceanObject mOcean;
};
