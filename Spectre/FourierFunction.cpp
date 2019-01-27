#include "FourierFunction.h"

#define _USE_MATH_DEFINES
#include <math.h>

FourierFunction::FourierFunction(std::vector<double> spectre, double tau, double scale)
	:_spectre(std::move(spectre)), _omegaTotal(-2*M_PI /tau, 2 * M_PI /tau), _scale(scale)
{
}

double FourierFunction::operator()(double x) const
{
	x /= _scale;
	if (x < _omegaTotal.first || x > _omegaTotal.second)
	{
		return 0.0;
	}

	auto step = (_omegaTotal.second - _omegaTotal.first) / _spectre.size();
	auto relativeIndex = std::llround (std::floor (x / step));

	auto index = relativeIndex + _spectre.size() / 2 - 1;
	if (index < 0 || index > _spectre.size())
	{
		return 0.0;
	}

	//const double dx = x - relativeIndex * step;
	//const double dy = (_spectre[index + 1] - _spectre[index]) / (std::abs(step - dx));

	std::vector<double> xCoords;
	std::vector<double> yCoords;

	size_t begIndex = getBeginIndex(index, _spectre.size());

	for (auto i = begIndex; i < begIndex + 4; ++i)
	{
		xCoords.push_back(_omegaTotal.first + step * (i + 1));
		yCoords.push_back(_spectre[i]);
	}

	return lagrange1(xCoords.data(), yCoords.data(), 4, x);
	
	//return   _spectre[index];
}

double FourierFunction::lagrange1(double * x, double * y, short n, double _x)
{
	double result = 0.0;

	for (short i = 0; i < n; i++)
	{
		double P = 1.0;

		for (short j = 0; j < n; j++)
			if (j != i)
				P *= (_x - x[j]) / (x[i] - x[j]);

		result += P * y[i];
	}

	return result;
}

double FourierFunction::lagrange2(double * x, double * y, short n, double _x)
{
	double result = 0.0;

	double h = x[1] - x[0];

	for (short i = 0; i < n; i++)
	{
		double P = 1.0;

		for (short j = 0; j < n; j++)
			if (i != j)
				P *= (_x - x[0] - h * j) / h / (i - j);

		result += P * y[i];
	}

	return result;
}

int64_t FourierFunction::getBeginIndex(int64_t index, int64_t size)
{
	if (index < 1) return index;
	if (index == (size - 1)) return index - 3;
	if (index == (size - 2)) return index - 2;
	return index -1;
}
