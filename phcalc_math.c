#include <math.h>

#include "phcalc.h"
#include "phcalc_in.h"


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
	z.value = abs( x.value );
	z.error = x.error;
	return z;
}

phcalc_num phcalc_sqrt(phcalc_num x) {
	phcalc_num z;
	z.value = sqrt( x.value );
	z.error = z.value*( x.error/x.value );
	return z;
}