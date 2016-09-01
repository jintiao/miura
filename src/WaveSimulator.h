#pragma once

#include "GLHeader.h"

#include <cmath>
#include <complex>
#include <vector>


// most calculation is base on [01, Tessendorf]Simulating Ocean Water.
class CWaveSimulator
{
public:
    CWaveSimulator (float wind, float waveSizeMax);

    void Update (float currentTime);

private:
	glm::vec2 CalcK (float x, float y); // k vector
	std::complex<float> CalcH0 (const glm::vec2 &k); // Fourier amplitude 0
	float CalcPh (const glm::vec2 &k); // Phillips spectrum
	std::complex<float> CalcH (float time, int x, int y); // Fourier amplitude

	void FFT2D (std::vector<std::complex<float>> &v);
	void FFT1D (std::vector<float> &real, std::vector<float> &imag);

	inline int GridLookup (int x, int y) { return y * mFFTSize + x; }
	inline static int PowNeg1 (int n) { static int pow[2] = { 1, -1 }; return pow[n & 1]; }

private:
	int mFFTSize = 256; // N/M, must be power of 2
	int mFFTSizeLog = (int)std::log (mFFTSize);
	float mWorldSize = 100.0f; // Lx/Lz
	float mWaveSizeMax;
	glm::vec2 mWind;

	std::vector<glm::vec2> mLUTk; // k vector look up table
	std::vector<std::complex<float>> mLUTh0; // h0(Fourier amplitude) look up table
	std::vector<std::complex<float>> mHeightField; // Fourier amplitudes height field

	std::vector<float> mHeightMap;
};
