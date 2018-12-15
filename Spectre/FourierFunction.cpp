#include "FourierFunction.h"

#define _USE_MATH_DEFINES
#include <math.h>

FourierFunction::FourierFunction(std::vector<double> spectre, double tau)
	:_spectre(std::move(spectre)), _omegaTotal(-1 /tau, 1  /tau)
{
}

double FourierFunction::operator()(double x) const
{
	if (x < _omegaTotal.first || x > _omegaTotal.second)
	{
		return 0.0;
	}

	auto step = (_omegaTotal.second - _omegaTotal.first) / _spectre.size();
	auto relativeIndex = std::llround (std::floor (x / step));

	auto index = relativeIndex + _spectre.size() / 2;
	if (index < 0 || index >= _spectre.size() - 1)
	{
		return 0.0;
	}

	//const double dx = x - relativeIndex * step;
	//const double dy = (_spectre[index + 1] - _spectre[index]) / (std::abs(step - dx));

	return   _spectre[index];
}
