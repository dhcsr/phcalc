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
	MC_E	= { M_E, 0.0 },
	MC_c	= { 299792458, 0.0 },
	MC_g	= { 9.80665, 0.0 },
	MC_G	= { 6.67384E-11, 0.0 },
	MC_Vm	= { 2.24141E-2, 0.0 },
	MC_NA	= { 6.02214078E+23, 0.0 },
	MC_e	= { 1.60217656535E-19, 0.0 },
	MC_me	= { 9.1093829140E-31, 0.0 },
	MC_mp	= { 1.67262177774E-27, 0.0 },
	MC_h	= { 6.6260692729E-34, 0.0 },
	MC_k	= { 1.380648813E-23, 0.0 };


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
	} else if( strcmp(name,"_c")==0 ){
		*res = MC_c;
		return 1;
	} else if( strcmp(name,"_g")==0 ){
		*res = MC_g;
		return 1;
	} else if( strcmp(name,"_G")==0 ){
		*res = MC_G;
		return 1;
	} else if( strcmp(name,"_Vm")==0 ){
		*res = MC_Vm;
		return 1;
	} else if( strcmp(name,"_NA")==0 ){
		*res = MC_NA;
		return 1;
	} else if( strcmp(name,"_e")==0 ){
		*res = MC_e;
		return 1;
	} else if( strcmp(name,"_me")==0 ){
		*res = MC_me;
		return 1;
	} else if( strcmp(name,"_mp")==0 ){
		*res = MC_mp;
		return 1;
	} else if( strcmp(name,"_h")==0 ){
		*res = MC_h;
		return 1;
	} else if( strcmp(name,"_k")==0 ){
		*res = MC_k;
		return 1;
	}
	return 0;

}

int phcalc_mfunc(phcalc_evalctx *ctx, const char *name, phcalc_obj *args, int nargs, phcalc_obj *res) {
	int ret_num = 1;
	phcalc_num fr;
	if( strcmp(name,"Sqrt")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_sqrt(args[0].ref.num);
	} else if( strcmp(name,"Abs")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_abs(args[0].ref.num);
	} else if( strcmp(name,"Log")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_log(args[0].ref.num);
	} else if( strcmp(name,"Exp")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_exp(args[0].ref.num);
	} else if( strcmp(name,"Sin")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_sin(args[0].ref.num);
	} else if( strcmp(name,"Cos")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_cos(args[0].ref.num);
	} else if( strcmp(name,"Tan")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_tan(args[0].ref.num);
	} else if( strcmp(name,"ArcSin")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_arcsin(args[0].ref.num);
	} else if( strcmp(name,"ArcCos")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_arccos(args[0].ref.num);
	} else if( strcmp(name,"ArcTan")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		fr = phcalc_arctan(args[0].ref.num);
	}
	
	else if( strcmp(name,"Average")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_average(&fr,args[0].ref.vect) );
	} else if( strcmp(name,"Mean")==0 ){
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
	z.error = z.value*( x.error/x.value )/2.0;
	return z;
}

phcalc_num phcalc_log(phcalc_num x) {
	phcalc_num z;
	z.value = log( x.value );
	z.error = log( 1.0 + x.error/x.value );
	return z;
}

phcalc_num phcalc_exp(phcalc_num x) {
	phcalc_num z;
	z.value = exp( x.value );
	z.error = z.value * ( exp( x.error ) - 1.0 );
	return z;
}

phcalc_num phcalc_sin(phcalc_num x) {
	phcalc_num z;
	z.value = sin( x.value );
	z.error = max( dabs( sin( x.value+x.error ) - z.value ), dabs( sin( x.value-x.error ) - z.value ) );
	return z;
}

phcalc_num phcalc_cos(phcalc_num x) {
	phcalc_num z;
	z.value = sin( x.value );
	z.error = max( dabs( cos( x.value+x.error ) - z.value ), dabs( cos( x.value-x.error ) - z.value ) );
	return z;
}

phcalc_num phcalc_tan(phcalc_num x) {
	phcalc_num z;
	z.value = tan( x.value );
	z.error = max( dabs( tan( x.value+x.error ) - z.value ), dabs( tan( x.value-x.error ) - z.value ) );
	return z;
}

phcalc_num phcalc_arcsin(phcalc_num x) {
	phcalc_num z;
	z.value = asin( x.value );
	z.error = max( dabs( asin( x.value+x.error ) - z.value ), dabs( asin( x.value-x.error ) - z.value ) );
	return z;
}

phcalc_num phcalc_arccos(phcalc_num x) {
	phcalc_num z;
	z.value = acos( x.value );
	z.error = max( dabs( acos( x.value+x.error ) - z.value ), dabs( acos( x.value-x.error ) - z.value ) );
	return z;
}

phcalc_num phcalc_arctan(phcalc_num x) {
	phcalc_num z;
	z.value = atan( x.value );
	z.error = max( dabs( atan( x.value+x.error ) - z.value ), dabs( atan( x.value-x.error ) - z.value ) );
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