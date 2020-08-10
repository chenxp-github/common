#ifndef __MATHS_H
#define __MATHS_H

#include "cruntime.h"

class Math{
public:
	static const double EPSILON;
	static const double PI;
public:
    static double abs(double x);
    static double acos(double x);
    static double asin(double x);
    static double atan(double x);
    static double atan2(double y, double x);
    static double ceil(double x);
    static double cos(double x);
    static double exp(double x);
    static double floor(double x);
    static double log(double x);
    static double pow(double x,double y);
    static double random();
    static double sin(double x);
    static double sqrt(double x);
    static double tan(double x);
    static double degs_to_rads(double degs);
    static double rads_to_degs(double rads);
};

#endif
