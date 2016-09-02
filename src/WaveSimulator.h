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
	struct IntermediaData
	{
		glm::vec2 k; // k vector
		float w; // wave frequency
		std::complex<float> h0; // fourier amplitude of a wave height field
		std::complex<float> h0cn; // conj(h0(-k))
	};

private:
	void InitDataLUT ();
	glm::vec2 ComputeK (int x, int y);
	float ComputeWaveFrequency (const glm::vec2 &k);
	std::complex<float> ComputeFourierAmplitude0 (const glm::vec2 &k);
	float ComputePhillipsSpectrum (const glm::vec2 &k);

	void ComputeFourierAmplitude (int x, int y, float t); // Fourier amplitude of the wave field realization at time t

	void FFT2D (std::vector<std::complex<float>> &v);
	void FFT1D (std::vector<float> &real, std::vector<float> &imag);

	inline int IndexLookup (int x, int y) { return y * mFFTSize + x; }
    inline int PowNeg1 (int n) { static int pow[2] = { 1, -1 }; return pow[n & 1]; }

private:
	glm::vec2 mWindDirection;
	float mWindSpeed;

	const float g = 9.81f; // Gravity constant
	const float pi = (float)M_PI; // 

	const int mFFTSize = 512; // N/M, must be power of 2
	const int mFFTSizeLog = (int)std::log2 (mFFTSize); // log2(fftsize), use for fft calculation
	const int mFFTSizeHalf = mFFTSize / 2;
	const float mWorldSize = 1.0f; // Lx/Lz, 1000 meters
	const float mPSpectrumConstant = 1.0f;
	const float mMinimalWaveSize = mWorldSize * 0.001f; // waves length way smaller than the world size should be suppressed
	const float mMinimalWaveSize2 = -mMinimalWaveSize * mMinimalWaveSize; 

	std::vector<IntermediaData> mDataLUT; // pre-computed data we need to use in every frame
	std::vector<std::complex<float>> mHeightField; // fourier amplitude of the wave field realization at time t

	std::vector<float> mHeightMap;
};
