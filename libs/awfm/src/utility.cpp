#include "utility.h"


bool Utility::floatCompare(double x1, double x2, double tol)
{
    return fabs(x1 - x2) < tol;
}
