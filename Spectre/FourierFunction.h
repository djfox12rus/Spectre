#pragma once

#include <vector>
#include <utility>

class FourierFunction
{
public:
	FourierFunction(std::vector<double> spectre, double tau);

	double operator()(double x) const;

private:

	std::vector<double>			_spectre;
	std::pair<double, double>	_omegaTotal;
	
};

