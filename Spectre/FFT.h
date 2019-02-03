#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>
#include <complex>
#include <cstdio>
#include <algorithm>

struct FFT
{
	static constexpr int sampleCount = 8192;
	static constexpr double TwoPi = M_PI * 2;

	using ComplexSignal = std::vector<std::complex<double>>;
	using RealSignal = std::vector<double>;

	// separate even/odd elements to lower/upper halves of array respectively.
	// Due to Butterfly combinations, this turns out to be the simplest way 
	// to get the job done without clobbering the wrong elements.
	template<class RandIt>
	static void separate(RandIt begin, size_t n)
	{
		ComplexSignal b;
		b.resize(n / 2);
		for (int i = 0; i < n / 2; i++)    // copy all odd elements to heap storage
			b[i] = begin[i * 2 + 1];
		for (int i = 0; i < n / 2; i++)    // copy all even elements to lower-half of a[]
			begin[i] = begin[i * 2];
		for (int i = 0; i < n / 2; i++)    // copy all odd (from heap) to upper-half of a[]
			begin[i + n / 2] = b[i];
	}


	// N must be a power-of-2, or bad things will happen.
	// Currently no check for this condition.
	//
	// N input samples in X[] are FFT'd and results left in X[].
	// Because of Nyquist theorem, N samples means 
	// only first N/2 FFT results in X[] are the answer.
	// (upper half of X[] is a reflection with no new information).
	template<class RandIt>
	static void fft(RandIt begin, size_t N)
	{
		using namespace std;
		if (N < 2) {
			// bottom of recursion.
			// Do nothing here, because already X[0] = x[0]
		}
		else {
			separate(begin, N);      // all evens to lower half, all odds to upper half
			fft(begin, N / 2);   // recurse even items
			fft(begin + N / 2, N / 2);   // recurse odd  items
			// combine results of two half recursions
			for (int k = 0; k < N / 2; k++) {
				complex<double> e = begin[k];   // even
				complex<double> o = begin[k + N / 2];   // odd
							 // w is the "twiddle-factor"
				complex<double> w = exp(complex<double>(0, -2.*M_PI*k / N));
				begin[k] = e + w * o;
				begin[k + N / 2] = e - w * o;
			}
		}
	}

	static ComplexSignal transformToComplex(const RealSignal& signal)
	{
		ComplexSignal out;
		out.resize(signal.size());
		std::transform(std::cbegin(signal), std::cend(signal), std::begin(out), [](const double& in) {return in; });
		return out;
	}

	static RealSignal amplitudeSpectre(const ComplexSignal& spectre, double tau = 1.)
	{
		RealSignal out;
		out.resize(spectre.size());
		double scale = 2 * tau;
		std::transform(std::cbegin(spectre), std::cend(spectre), std::begin(out), [scale](const std::complex<double>& in) {return std::abs(in)/ (sampleCount/ scale); });
		return out;
	}

	static ComplexSignal& rotate(ComplexSignal& spectre)
	{
		auto sz = spectre.size();
		for (size_t i = 0; i < sz / 4; ++i)
		{
			std::swap(spectre[i], spectre[sz / 2 - i - 1]);
		}
		for (size_t i = 0; i < sz / 4; ++i)
		{
			std::swap(spectre[sz / 2 + i], spectre[sz - i - 1]);
		}
		return spectre;
	}

};
