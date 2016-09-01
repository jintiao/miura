#include "WaveSimulator.h"

#include <algorithm>
#include <functional>
#include <random>

static const float g = 9.81f;


CWaveSimulator::CWaveSimulator (float wind, float waveSizeMax) : mWind (std::cosf (wind), std::sinf (wind)), mWaveSizeMax (waveSizeMax)
{
	int size = mFFTSize * mFFTSize;
	mLUTk.resize (size);
	mLUTh0.resize (size);
	mHeightField.resize (size);
	mHeightMap.resize (size);

	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			int i = GridLookup (x, y);

			auto &k = mLUTk[i];
			k = CalcK ((float)x, (float)y);
			mLUTh0[i] = CalcH0 (k);
		}
	}
}


// k vector, two lines right below [Equation 19]
glm::vec2 CWaveSimulator::CalcK (float x, float y)
{
	static float pi = (float)M_PI;
	static float pi2byworld = 2.0f * pi / mWorldSize;
	static float halfSize = mFFTSize / 2.0f;

	return { ((float)x - halfSize) * pi2byworld, ((float)y - halfSize) * pi2byworld };
}


// Fourier amplitude, [Equation 25]
std::complex<float> CWaveSimulator::CalcH0 (const glm::vec2 &k)
{
	static float invSqr2 = 1.0f / std::sqrtf (2.0f);

	static std::default_random_engine generator;
	static std::normal_distribution<float> distribution (0.0f, 1.0f);
	static auto roll = std::bind (distribution, generator);

	std::complex<float> e = { roll (), roll () };
	return e * invSqr2 * std::sqrtf (CalcPh (k));
}


// Phillips spectrum, [Equation 23]
float CWaveSimulator::CalcPh (const glm::vec2 &k)
{
	static const float A = 1.0f;

	if (glm::length (k) == 0)
		return 0.0f;

	float k2 = glm::dot (k, k);
	float w2 = glm::dot (mWind, mWind);
	float L = w2 / g;
	return (A * std::expf (-1.0f / (k2 * L * L)) * (std::powf (glm::dot (k, mWind), 2) / (k2 * w2)) / (k2 * k2));
}


#include <fstream>
#include <climits>
// [Equation 19]
void CWaveSimulator::Update (float currentTime)
{
	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			mHeightField[GridLookup (x, y)] = CalcH (currentTime, x, y);
		}
	}
	FFT2D (mHeightField);
	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			int i = GridLookup (x, y);
			auto &height = mHeightField[i];
			height *= PowNeg1 (x + y) * mWaveSizeMax;
			mHeightMap[i] = -height.real ();
		}
	}

//	static int cccc = 0;
//	if (cccc++ == 1)
//	{
//		float min = std::numeric_limits<float>::max ();
//		float max = std::numeric_limits<float>::min ();
//		for (auto &field : mHeightField) {
//			float h = -field.real ();
//			if (h > max) max = h;
//			if (h < min) min = h;
//		}
//		float scale = 1 / (max - min);
//
//#define INT2CHAR_BIT(num, bit) (unsigned char)(((num) >> (bit)) & 0xff)
//#define INT2CHAR(num) INT2CHAR_BIT((num),0), INT2CHAR_BIT((num),8), INT2CHAR_BIT((num),16), INT2CHAR_BIT((num),24)
//		unsigned char buf[54] = { 'B', 'M', INT2CHAR (54 + mFFTSize*mFFTSize * 32), INT2CHAR (0), INT2CHAR (54), INT2CHAR (40), INT2CHAR (mFFTSize), INT2CHAR (mFFTSize), 1, 0, 32, 0 };
//		std::ofstream ofs ("height.bmp", std::ios_base::out | std::ios_base::binary);
//		ofs.write ((char *)buf, sizeof (buf));
//		for (auto &field : mHeightField) {
//			float h = -field.real ();
//			h = (h - min) * scale;
//			buf[0] = (unsigned char)std::min (255, (int)(h * 255));
//			buf[1] = (unsigned char)std::min (255, (int)(h * 255));
//			buf[2] = (unsigned char)std::min (255, (int)(h * 255));
//			buf[3] = 0;
//			ofs.write ((char *)buf, 4);
//		}
//	}
}


std::complex<float> CWaveSimulator::CalcH (float time, int x, int y)
{
	int i = GridLookup (x, y);
	int iNeg = GridLookup (mFFTSize - 1 - x, mFFTSize - 1 - y);
	auto &k = mLUTk[i];
	auto &h = mLUTh0[i];
	auto &hNeg = mLUTh0[iNeg];

	// [Equation 14]
	float w = std::sqrtf (glm::length (k) * g);

	// [Equation 26]
	float freq = w * time;
	std::complex<float> ep = (std::cosf (freq), std::sinf (freq));
	return (h * ep + std::conj (hNeg) * std::conj (ep));
}


void CWaveSimulator::FFT2D (std::vector<std::complex<float>> &c)
{
	std::vector<float> real (mFFTSize);
	std::vector<float> imag (mFFTSize);

	for (int y = 0; y < mFFTSize; y++)
	{
		for (int x = 0; x < mFFTSize; x++)
		{
			auto &curC = c[GridLookup (x, y)];
			real[x] = curC.real ();
			imag[x] = curC.imag ();
		}

		FFT1D (real, imag);

		for (int x = 0; x < mFFTSize; x++)
		{
			c[GridLookup (x, y)] = { real[x], imag[x] };
		}
	}

	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			auto &curC = c[GridLookup (x, y)];
			real[y] = curC.real ();
			imag[y] = curC.imag ();
		}

		FFT1D (real, imag);

		for (int y = 0; y < mFFTSize; y++)
		{
			c[GridLookup (x, y)] = { real[y], imag[y] };
		}
	}
}


void CWaveSimulator::FFT1D (std::vector<float> &real, std::vector<float> &imag)
{
	long nn, i, i1, j, k, i2, l, l1, l2;
	float c1, c2, fReal, fImag, t1, t2, u1, u2, z;

	nn = mFFTSize;
	i2 = nn >> 1;
	j = 0;
	for (i = 0; i<nn - 1; ++i)
	{
		if (i<j)
		{
			fReal = real[i];
			fImag = imag[i];
			real[i] = real[j];
			imag[i] = imag[j];
			real[j] = fReal;
			imag[j] = fImag;
		}

		k = i2;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}

		j += k;
	}

	// FFT computation
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

