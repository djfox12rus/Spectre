#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>

struct FFT
{

	static constexpr double TwoPi = M_PI * 2;

	static std::vector<double> transform(const std::vector<double>& in)
	{
		int i, j, n, m, Mmax, Istp;
		double Tmpr, Tmpi, Wtmp, Theta;
		double Wpr, Wpi, Wr, Wi;
		const auto sz = in.size();

		n = sz * 2;
		std::vector<double> temp(n);

		for (i = 0; i < n; i += 2)
		{
			temp[i] = 0;
			temp[i + 1] = in[i / 2];
		}

		i = 1; j = 1;
		while (i < n)
		{
			if (j > i)
			{
				Tmpr = temp[i]; temp[i] = temp[j]; temp[j] = Tmpr;
				Tmpr = temp[i + 1]; temp[i + 1] = temp[j + 1]; temp[j + 1] = Tmpr;
			}
			i = i + 2; m = sz;
			while ((m >= 2) && (j > m))
			{
				j = j - m; m = m >> 1;
			}
			j = j + m;
		}

		Mmax = 2;
		while (n > Mmax)
		{
			Theta = -TwoPi / Mmax; Wpi = std::sin(Theta);
			Wtmp = std::sin(Theta / 2); Wpr = Wtmp * Wtmp * 2;
			Istp = Mmax * 2; Wr = 1; Wi = 0; m = 1;

			while (m < Mmax)
			{
				i = m; m = m + 2; Tmpr = Wr; Tmpi = Wi;
				Wr = Wr - Tmpr * Wpr - Tmpi * Wpi;
				Wi = Wi + Tmpr * Wpi - Tmpi * Wpr;

				while (i < n)
				{
					j = i + Mmax;
					Tmpr = Wr * temp[j] - Wi * temp[j - 1];
					Tmpi = Wi * temp[j] + Wr * temp[j - 1];

					temp[j] = temp[i] - Tmpr; temp[j - 1] = temp[i - 1] - Tmpi;
					temp[i] = temp[i] + Tmpr; temp[i - 1] = temp[i - 1] + Tmpi;
					i = i + Istp;
				}
			}

			Mmax = Istp;
		}

		std::vector<double> out(sz);
		for (i = 0; i < sz; i++)
		{
			j = i * 2;
			out[i] = 2 * std::sqrt(temp[j]* temp[j] + temp[j + 1] * temp[j + 1]) / sz;
		}

		return out;
	}

	static std::vector<double>& rotate(std::vector<double>& in)
	{
		auto sz = in.size();
		auto half = sz / 2;
		for (int i = 0; i < half / 2; ++i)
		{
			std::swap(in[i], in[half - i - 1]);
		}
		for (auto i = half; i < half + half / 2; ++i)
		{
			std::swap(in[i], in[sz - (i - half) - 1]);
		}

		return in;
	}

	static std::vector<double> lowFreqFilter(const std::vector<double>& in, double threshold = 0.005)
	{
		std::vector<double> out;
		auto sz = in.size();
		for (int i = 0; i < sz; ++i)
		{
			if (in[i] > threshold)
			{
				out.push_back(in[i]);
			}
			else if (i > 0 && i < sz - 1)
			{
				out.push_back((in[i - 1] + in[i + 1]) / 2);
			}
		}
		return out;
	}
};
