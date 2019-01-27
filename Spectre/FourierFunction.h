#pragma once

#include <vector>
#include <utility>

class FourierFunction
{
public:
	FourierFunction(std::vector<double> spectre, double tau, double scale = 1.0);

	double operator()(double x) const;

private:

	std::vector<double>			_spectre;
	std::pair<double, double>	_omegaTotal;
	double						_scale{ 1 };
	
	static double lagrange1(double* x, double* y, short n, double _x);
	static double lagrange2(double* x, double* y, short n, double _x);

	static int64_t getBeginIndex(int64_t index, int64_t size);
};

