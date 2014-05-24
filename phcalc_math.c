#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>

#include "phcalc.h"
#include "phcalc_in.h"

#define D_EPSILON	0.000001

#define max(x,y)	((x)>(y) ? (x) : (y))

const phcalc_num
	MC_PI	= { M_PI, 0.0 },
	MC_E	= { M_E, 0.0 };


double dabs(double x) {
	return x > 0 ? x : -x;
}

int dcmp(double x, double y) {
	double mod = dabs(x)+dabs(y);
	double dif = dabs(x-y);
	if( dif/mod < D_EPSILON )
		return 0;
	if(x<y)
		return -1;
	else
		return 1;
}

int phcalc_cmp(phcalc_num x, phcalc_num y) {
	double mod = dabs(x.value)+dabs(y.value);
	double maxdif = max(x.error,y.error) + D_EPSILON*mod;
	if( dabs(x.value-y.value) < maxdif )
		return 0;
	if( x.value < y.value )
		return -1;
	else
		return 1;
}

int phcalc_const(const char *name, phcalc_num *res) {
	if( strcmp(name,"_PI")==0 ){
		*res = MC_PI;
		return 1;
	} else if( strcmp(name,"_E")==0 ){
		*res = MC_E;
		return 1;
	}
	return 0;

}

phcalc_num phcalc_add(phcalc_num x, phcalc_num y) {
	phcalc_num z;
	z.value = x.value + y.value;
	z.error = x.error + y.error;
	return z;
}

phcalc_num phcalc_sub(phcalc_num x, phcalc_num y) {
	phcalc_num z;
	z.value = x.value - y.value;
	z.error = x.error + y.error;
	return z;
}

phcalc_num phcalc_mul(phcalc_num x, phcalc_num y) {
	phcalc_num z;
	z.value = x.value * y.value;
	z.error = z.value*(x.error/x.value + y.error/y.value);		// !!! fix with x,y->0
	//z.error = abs( (x.value+x.error) * (y.value+y.error) - (x.value-x.error) * (y.value-y.error) );
	return z;
}

phcalc_num phcalc_div(phcalc_num x, phcalc_num y) {
	phcalc_num z;
	z.value = x.value / y.value;
	z.error = z.value*(x.error/x.value + y.error/y.value);		// !!! fix with x,y->0
	return z;
}

phcalc_num phcalc_pow(phcalc_num x, phcalc_num y) {
	phcalc_num z;
	z.value = pow( x.value, y.value );
	z.error = z.value*(x.error/x.value + y.error*log(x.value));		// !!! check
	return z;
}

phcalc_num phcalc_abs(phcalc_num x) {
	phcalc_num z;
	z.value = dabs( x.value );
	z.error = x.error;
	return z;
}

phcalc_num phcalc_sqrt(phcalc_num x) {
	phcalc_num z;
	z.value = sqrt( x.value );
	z.error = z.value*( x.error/x.value );
	return z;
}