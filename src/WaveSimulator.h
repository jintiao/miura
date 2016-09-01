#pragma once

#include "GLHeader.h"

#include <cmath>
#include <complex>
#include <vector>


// most calculation is base on [2001, Jerry Tessendorf]Simulating Ocean Water.
class CWaveSimulator
{
public:
    CWaveSimulator (float windAngle, float windSpeed);

    void Update (float currentTime);
    
    const std::vector<float> &GetData () { return mHeightMap; }
    
    void DebugSave (const char *path);

private:
	glm::vec2 CalcK (int x, int y); // k vector
	float CalcW (const glm::vec2 &k); // wave frequency
	std::complex<float> CalcH0 (const glm::vec2 &k); // Fourier amplitude 0
	float CalcPh (const glm::vec2 &k); // Phillips spectrum
	std::complex<float> CalcH (float time, int x, int y); // Fourier amplitude

	void FFT2D (std::vector<std::complex<float>> &v);
	void FFT1D (std::vector<float> &real, std::vector<float> &imag);

	inline int GridLookup (int x, int y) { return y * mFFTSize + x; }

private:
	const int mFFTSize = 64; // N/M, must be power of 2
	const int mFFTSizeLog = (int)std::log (mFFTSize);
	const float mWorldSize = 100.0f; // Lx/Lz

    glm::vec2 mWindDirection;
	float mWindSpeed;


	std::vector<glm::vec2> mLUTk; // k vector look up table
	std::vector<float> mLUTw; // frequency look up table
	std::vector<std::complex<float>> mLUTh0; // h0(Fourier amplitude) look up table
	std::vector<std::complex<float>> mHeightField; // Fourier amplitudes height field

	std::vector<float> mHeightMap;
};
