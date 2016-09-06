#include "GlobalHeader.h"

#include "WaveSimulator.h"

#include <algorithm>
#include <climits>
#include <functional>
#include <fstream>
#include <random>


CWaveSimulator::CWaveSimulator (SOceanParams params) : mEnvironmentParams (params)
{
	int size = mFFTSize * mFFTSize;

	mHeightField.resize (size);
    mDisplacementFieldX.resize (size);
    mDisplacementFieldZ.resize (size);
    mNormalFieldX.resize (size);
    mNormalFieldZ.resize (size);

	mDisplacementData.resize (size);
    mNormalData.resize (size);

	mDataLUT.resize (size);
	InitDataLUT ();
}


void CWaveSimulator::InitDataLUT ()
{
	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			CacheData &lookup = mDataLUT[IndexLookup (x, y)];
			lookup.k = ComputeK (x, y);
            lookup.kn = Math::Length (lookup.k) > 0.00001f ? Math::Normalize (lookup.k) : Math::Vector2 (0, 0);
            lookup.expKDotX = ComputeExpKDotX (lookup.k, x, y);
			lookup.w = ComputeWaveFrequency (lookup.k);
			lookup.h0 = ComputeFourierAmplitude0 (lookup.k);
            lookup.h0cn = std::conj (ComputeFourierAmplitude0 (-lookup.k));
		}
	}
}


// k vector, two lines right below [Equation 19]
Math::Vector2 CWaveSimulator::ComputeK (int x, int y)
{
	static const float pi2byworld = 2.0f * pi / mWorldSize;
	return { (x - mFFTSizeHalf) * pi2byworld, (y - mFFTSizeHalf) * pi2byworld };
}


// exp (i dot (kvector,  horizontal position v(x, y))), two lines right below [Equation 19]
std::complex<float> CWaveSimulator::ComputeExpKDotX (const Math::Vector2 &k, int x, int y)
{
	static const float lbyn = mWorldSize / mFFTSize;
    Math::Vector2 v = { (x - mFFTSizeHalf) * lbyn, (y - mFFTSizeHalf) * lbyn };
    float theta = Math::Dot (k, v);
    return { std::cosf (theta), std::sinf (theta) };
}


// wave frequency, [Equation 14]
float CWaveSimulator::ComputeWaveFrequency (const Math::Vector2 &k)
{
	return std::sqrtf (Math::Length (k) * g);
}


// Fourier amplitude of a wave height field, [Equation 25]
std::complex<float> CWaveSimulator::ComputeFourierAmplitude0 (const Math::Vector2 &k)
{
	static std::default_random_engine generator;
	static std::normal_distribution<float> distribution (0.0f, 1.0f);
	static auto roll = std::bind (distribution, generator);

	static const float invSqr2 = 1.0f / std::sqrtf (2.0f);

	return (invSqr2 * std::sqrtf (ComputePhillipsSpectrum (k)) * std::complex<float> (roll (), roll ()));
}


// Phillips spectrum, [Equation 23]
float CWaveSimulator::ComputePhillipsSpectrum (const Math::Vector2 &k)
{
	float k2 = Math::Dot (k, k);
	if (k2 == 0)
		return 0.0f;

	static const float L = mEnvironmentParams.windSpeed * mEnvironmentParams.windSpeed / g;
	static const float L2 = L * L;
	static const float l2 = mMinimalWaveScale * mMinimalWaveScale * -1;

	float k4 = k2 * k2;
	float kL2 = k2 * L2;
	float kw = Math::Dot (Math::Normalize (k), mEnvironmentParams.windDirection);
	float kw2 = kw * kw;

	// [Equation 24]
	float extraFactor = std::expf (k2 * l2);

	return (mPhillipsSpectrumConstant * (std::expf (-1.0f / kL2) / k4) * kw2 * extraFactor);
}


void CWaveSimulator::Update (float currentTime)
{
	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			ComputeFourierField (x, y, currentTime);
		}
	}
    
	// back to space coordinate, [Equation 19]
	FFT2D (mHeightField);
	FFT2D (mDisplacementFieldX);
	FFT2D (mDisplacementFieldZ);
	FFT2D (mNormalFieldX);
	FFT2D (mNormalFieldZ);

	for (int x = 0; x < mFFTSize; x++)
	{
		for (int y = 0; y < mFFTSize; y++)
		{
			int i = IndexLookup (x, y);
            
            // FFT produce a pow(-1, x + y), we need to correct it
            int sign = PowNeg1 (x + y);
            
			mDisplacementData[i].y = mHeightField[i].real () * sign;
            mDisplacementData[i].x = mDisplacementFieldX[i].real () * sign;
            mDisplacementData[i].z = mDisplacementFieldZ[i].real () * sign;

            // [Equation 36], y(0, 1, 0) - e(x,t), then normalize it
            mNormalData[i] = Math::Normalize(Math::Vector3 (-mNormalFieldX[i].real () * sign, 1.0f, -mNormalFieldZ[i].real () * sign));
		}
	}

	// texture values must be [0,1] in gpu, so we need to normalize it, then rescale it in shader
	NormalizeData ();
}


// Fourier amplitude/displacement/normal of the wave field realization at time t,
void CWaveSimulator::ComputeFourierField (int x, int y, float t)
{
	int i = IndexLookup (x, y);
	auto &lookup = mDataLUT[i];

	float wt = lookup.w * t;

    // Euler's formula, exp(ix) = cos(x) + i * sin(x), exp(-ix) = conj (exp(ix))
	std::complex<float> ep = { std::cosf (wt), std::sinf (wt) };

	// [Equation 26]
	mHeightField[i] = (lookup.h0 * ep + lookup.h0cn * std::conj (ep)) * lookup.expKDotX;
    
    // [Equation 29]
    mDisplacementFieldX[i] = mHeightField[i] * std::complex<float> (0, -lookup.kn.x);
    mDisplacementFieldZ[i] = mHeightField[i] * std::complex<float> (0, -lookup.kn.y);

    // [Equation 20]
    mNormalFieldX[i] = mHeightField[i] * std::complex<float> (0, lookup.k.x);
    mNormalFieldZ[i] = mHeightField[i] * std::complex<float> (0, lookup.k.y);
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


// scale displacement/normal data from [-max, max] to [0, 1]
void CWaveSimulator::NormalizeData ()
{	
	float min = std::numeric_limits<float>::max ();
	float max = std::numeric_limits<float>::min ();
	for (auto &data : mDisplacementData) {
		if (data.x > max) max = data.x;
		if (data.x < min) min = data.x;
		if (data.y > max) max = data.y;
		if (data.y < min) min = data.y;
		if (data.z > max) max = data.z;
		if (data.z < min) min = data.z;
	}
	max = std::max (std::fabs (max), std::fabs (min));
	for (auto &data : mDisplacementData) {
		data.x = (data.x / max + 1.0f) * 0.5f;
		data.y = (data.y / max + 1.0f) * 0.5f;
		data.z = (data.z / max + 1.0f) * 0.5f;
	}
    
    
	// normal data already normalized to [-1, 1]
	for (auto &data : mNormalData) {
		data.x = (data.x + 1) * 0.5f;
		data.y = (data.y + 1) * 0.5f;
		data.z = (data.z + 1) * 0.5f;
	}
}


// save all data into a gray scale bmp file
void CWaveSimulator::DebugSave (const char *path)
{
    std::string dfile (path);
    dfile += "_h.bmp";
    DebugSaveData (dfile.c_str (), mDisplacementData, 1);

    std::string dxfile (path);
    dxfile += "_dx.bmp";
    DebugSaveData (dxfile.c_str (), mDisplacementData, 0);

    std::string dzfile (path);
    dzfile += "_dz.bmp";
    DebugSaveData (dzfile.c_str (), mDisplacementData, 2);
    
    std::string nxfile (path);
    nxfile += "_nx.bmp";
    DebugSaveData (nxfile.c_str (), mNormalData, 0);

	std::string nyfile (path);
	nyfile += "_ny.bmp";
	DebugSaveData (nyfile.c_str (), mNormalData, 1);

	std::string nzfile (path);
	nzfile += "_nz.bmp";
	DebugSaveData (nzfile.c_str (), mNormalData, 2);
}


void CWaveSimulator::DebugSaveData (const char *path, const std::vector<Math::Vector3> &v, int index)
{
#define INT2CHAR_BIT(num, bit) (unsigned char)(((num) >> (bit)) & 0xff)
#define INT2CHAR(num) INT2CHAR_BIT((num),0), INT2CHAR_BIT((num),8), INT2CHAR_BIT((num),16), INT2CHAR_BIT((num),24)
    unsigned char buf[54] = { 'B', 'M', INT2CHAR (54 + mFFTSize*mFFTSize * 32), INT2CHAR (0), INT2CHAR (54), INT2CHAR (40), INT2CHAR (mFFTSize), INT2CHAR (mFFTSize), 1, 0, 32, 0 };
    std::ofstream ofs (path, std::ios_base::out | std::ios_base::binary);
    ofs.write ((char *)buf, sizeof (buf));
    for (auto &data : v) {
		float h = data.x;
		if (index == 1)
			h = data.y;
		else if (index == 2)
			h = data.z;

        buf[0] = (unsigned char)std::min (255, (int)(h * 255));
        buf[1] = (unsigned char)std::min (255, (int)(h * 255));
        buf[2] = (unsigned char)std::min (255, (int)(h * 255));
        buf[3] = 0;
        ofs.write ((char *)buf, 4);
    }
}

