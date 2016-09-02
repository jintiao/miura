#include "WaveSimulator.h"

#include <algorithm>
#include <climits>
#include <functional>
#include <random>


CWaveSimulator::CWaveSimulator (float windAngle, float windSpeed) :
    mWindDirection (std::cosf (windAngle), std::sinf (windAngle)),
    mWindSpeed (windSpeed)
{
	int size = mFFTSize * mFFTSize;
	mHeightField.resize (size);
	mHeightMap.resize (size);

	mDataLUT.resize (size);
	InitDataLUT ();
}


void CWaveSimulator::InitDataLUT ()
{
	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			IntermediaData &lookup = mDataLUT[IndexLookup (x, y)];
			lookup.k = ComputeK (x, y);
			lookup.w = ComputeWaveFrequency (lookup.k);
			lookup.h0 = ComputeFourierAmplitude0 (lookup.k);
            lookup.h0cn = std::conj (ComputeFourierAmplitude0 (-lookup.k));
		}
	}
}


// k vector, two lines right below [Equation 19]
glm::vec2 CWaveSimulator::ComputeK (int x, int y)
{
	static const float pi2byworld = 2.0f * pi / mWorldSize;
	return { (x - mFFTSizeHalf) * pi2byworld, (y - mFFTSizeHalf) * pi2byworld };
}


// wave frequency, [Equation 14]
float CWaveSimulator::ComputeWaveFrequency (const glm::vec2 &k)
{
	return std::sqrtf (glm::length (k) * g);
}


// Fourier amplitude of a wave height field, [Equation 25]
std::complex<float> CWaveSimulator::ComputeFourierAmplitude0 (const glm::vec2 &k)
{
	static std::default_random_engine generator;
	static std::normal_distribution<float> distribution (0.0f, 1.0f);
	static auto roll = std::bind (distribution, generator);

	static const float invSqr2 = 1.0f / std::sqrtf (2.0f);

	return (invSqr2 * std::sqrtf (ComputePhillipsSpectrum (k)) * std::complex<float> (roll (), roll ()));
}


// Phillips spectrum, [Equation 23]
float CWaveSimulator::ComputePhillipsSpectrum (const glm::vec2 &k)
{
	float k2 = glm::dot (k, k);
	if (k2 == 0)
		return 0.0f;

	float k4 = k2 * k2;
	float L = mWindSpeed * mWindSpeed / g;
	float L2 = L * L;
	float kL2 = k2 * L2;
	float kw = glm::dot (glm::normalize (k), mWindDirection);
	float kw2 = kw * kw;

	// [Equation 24]
	float extraFactor = std::expf (k2 * mMinimalWaveSize2);

	return (mPSpectrumConstant * (std::expf (-1.0f / kL2) / k4) * kw2 * extraFactor);
}


void CWaveSimulator::Update (float currentTime)
{
    // wave height field, [Equation 19]

	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			ComputeFourierAmplitude (x, y, currentTime);
		}
	}
	FFT2D (mHeightField);

	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			int i = IndexLookup (x, y);
			mHeightMap[i] = mHeightField[i].real () * PowNeg1 (x + y) * 100;
		}
	}
}


// Fourier amplitudes of the wave field realization at time t, [Equation 26]
void CWaveSimulator::ComputeFourierAmplitude (int x, int y, float t)
{
	int i = IndexLookup (x, y);
	auto &lookup = mDataLUT[i];

	float wt = lookup.w * t;

    // Euler's formula, exp(ix) = cos(x) + i * sin(x), exp(-ix) = conj (exp(ix))
	std::complex<float> ep = { std::cosf (wt), std::sinf (wt) };

	mHeightField[i] = lookup.h0 * ep + lookup.h0cn * std::conj (ep);
}


// reverse 2d fft. reference: [1998, Paul Bourke]2 Dimensional FFT.
void CWaveSimulator::FFT2D (std::vector<std::complex<float>> &c)
{
	std::vector<float> real (mFFTSize);
	std::vector<float> imag (mFFTSize);

	for (int y = 0; y < mFFTSize; y++)
	{
		for (int x = 0; x < mFFTSize; x++)
		{
			auto &curC = c[IndexLookup (x, y)];
			real[x] = curC.real ();
			imag[x] = curC.imag ();
		}

		FFT1D (real, imag);

		for (int x = 0; x < mFFTSize; x++)
		{
			c[IndexLookup (x, y)] = { real[x], imag[x] };
		}
	}

	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			auto &curC = c[IndexLookup (x, y)];
			real[y] = curC.real ();
			imag[y] = curC.imag ();
		}

		FFT1D (real, imag);

		for (int y = 0; y < mFFTSize; y++)
		{
			c[IndexLookup (x, y)] = { real[y], imag[y] };
		}
	}
}


// reverse 1d fft. reference: [1993, Paul Bourke] DFT (Discrete Fourier Transform) FFT(Fast Fourier Transform)
void CWaveSimulator::FFT1D (std::vector<float> &real, std::vector<float> &imag)
{
	long nn, i, i1, j, k, i2, l, l1, l2;
	float c1, c2,tx,ty, t1, t2, u1, u2, z;

	nn = mFFTSize;
	i2 = nn >> 1;
	j = 0;
	for (i = 0; i<nn - 1; ++i)
	{
		if (i<j)
		{
			tx = real[i];
            ty = imag[i];
            real[i] = real[j];
            imag[i] = imag[j];
            real[j] = tx;
            imag[j] = ty;
		}

		k = i2;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}

		j += k;
	}

	c1 = -1.0f;
	c2 = 0.0f;
	l2 = 1;
	for (l = 0; l < mFFTSizeLog; ++l)
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j<l1; ++j)
		{
			for (i = j; i<nn; i += l2)
			{
				i1 = i + l1;
				t1 = u1 * real[i1] - u2 * imag[i1];
				t2 = u1 * imag[i1] + u2 * real[i1];
				real[i1] = real[i] - t1;
				imag[i1] = imag[i] - t2;
				real[i] += t1;
				imag[i] += t2;
			}

			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}

		c2 = std::sqrtf ((1.0f - c1) * 0.5f);
		c1 = std::sqrtf ((1.0f + c1) * 0.5f);
	}
}


#include <fstream>
void CWaveSimulator::DebugSave (const char *path)
{
		float min = std::numeric_limits<float>::max ();
		float max = std::numeric_limits<float>::min ();
		for (auto &field : mHeightField) {
			float h = -field.real ();
			if (h > max) max = h;
			if (h < min) min = h;
		}
		float scale = 1 / (max - min);

#define INT2CHAR_BIT(num, bit) (unsigned char)(((num) >> (bit)) & 0xff)
#define INT2CHAR(num) INT2CHAR_BIT((num),0), INT2CHAR_BIT((num),8), INT2CHAR_BIT((num),16), INT2CHAR_BIT((num),24)
		unsigned char buf[54] = { 'B', 'M', INT2CHAR (54 + mFFTSize*mFFTSize * 32), INT2CHAR (0), INT2CHAR (54), INT2CHAR (40), INT2CHAR (mFFTSize), INT2CHAR (mFFTSize), 1, 0, 32, 0 };
		std::ofstream ofs (path, std::ios_base::out | std::ios_base::binary);
		ofs.write ((char *)buf, sizeof (buf));
		for (auto &field : mHeightField) {
			float h = -field.real ();
			h = (h - min) * scale;
			buf[0] = (unsigned char)std::min (255, (int)(h * 255));
			buf[1] = (unsigned char)std::min (255, (int)(h * 255));
			buf[2] = (unsigned char)std::min (255, (int)(h * 255));
			buf[3] = 0;
			ofs.write ((char *)buf, 4);
		}
}

