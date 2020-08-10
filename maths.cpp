#include <math.h>
#include "maths.h"
#include "syslog.h"
#include "mem_tool.h"

const double Math::EPSILON = 1.192092896e-07;
const double Math::PI = 3.14159265;

double Math::abs(double x)
{
	return ::fabs(x);
}

double Math::acos(double x)
{
	return ::acos(x);
}

double Math::asin(double x)
{
	return ::asin(x);
}

double Math::atan(double x)
{
	return ::atan(x);
}

double Math::atan2(double y, double x)
{
	return ::atan2(y,x);
}

double Math::ceil(double x)
{
	return ::ceil(x);
}

double Math::cos(double x)
{
	return ::cos(x);
}

double Math::exp(double x)
{
	return ::exp(x);
}

double Math::floor(double x)
{
	return ::floor(x);
}

double Math::log(double x)
{
	return ::log(x);
}

double Math::pow(double x,double y)
{
	return ::pow(x,y);
}

double Math::random()
{
	return (double)::rand()/(double)RAND_MAX;
}

double Math::sin(double x)
{
	return ::sin(x);
}

double Math::sqrt(double x)
{
	return ::sqrt(x);
}

double Math::tan(double x)
{
	return ::tan(x);
}

double Math::degs_to_rads(double degs)
{
	return degs*PI/180.0;
}

double Math::rads_to_degs(double rads)
{
	return rads*180.0/PI;
}
