#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <assert.h>

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
	if(x==y)
		return 0;
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

int phcalc_mfunc(phcalc_evalctx *ctx, const char *name, phcalc_obj *args, int nargs, phcalc_obj *res) {
	int ret_num = 1;
	phcalc_num fr;
	if( strcmp(name,"Abs")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_abs(args[0].ref.num);
	} else if( strcmp(name,"Sqrt")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_sqrt(args[0].ref.num);
	} else  if( strcmp(name,"Average")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_average(&fr,args[0].ref.vect) );
	}  else  if( strcmp(name,"Mean")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_mean(&fr,args[0].ref.vect) );
	} else
		ret_num = 0;
	if(ret_num){
		res->type = PHC_OBJ_NUM;
		res->ref.num = fr;
		return 1;
	}
	return 0;
}

phcalc_num phcalc_neg(phcalc_num x) {
	phcalc_num z;
	z.value = -x.value;
	z.error = x.error;
	return z;
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

int phcalc_average(phcalc_num *res, phcalc_vect vector) {
	int i;
	phcalc_num sum = { 0, 0 };
	for(i=0; i<vector.len; i++){
		if(vector.data[i].type!=PHC_OBJ_NUM)
			return 0;
		sum = phcalc_add(sum,vector.data[i].ref.num);
	}
	*res = phcalc_div( sum, phcalc_num_new(vector.len,0) );
	return 1;
}

int phcalc_mean(phcalc_num *res, phcalc_vect vector) {
	int i;
	double sum = 0.0;
	double sdevsum = 0.0;
	double errsum = 0.0;
	double v_mean;
	double v_sigma;
	double v_merr;
	for(i=0; i<vector.len; i++){
		if(vector.data[i].type!=PHC_OBJ_NUM)
			return 0;
		sum += vector.data[i].ref.num.value;
		errsum += vector.data[i].ref.num.error;
	}
	v_mean = sum / vector.len;
	v_merr = errsum / vector.len / sqrt( (double)vector.len );
	for(i=0; i<vector.len; i++){
		if(vector.data[i].type!=PHC_OBJ_NUM)
			return 0;
		sdevsum += ( vector.data[i].ref.num.value - v_mean )*( vector.data[i].ref.num.value - v_mean );
	}
	v_sigma = sqrt( sdevsum / vector.len );
	res->value = v_mean;
	if( v_sigma > v_merr )
		res->error = v_sigma;
	else
		res->error = v_merr;
	return 1;
}