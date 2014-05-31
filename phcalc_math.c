/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Mathematic functions source file
 *
 **************************************/

#define _USE_MATH_DEFINES

#include <math.h>
#include <string.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"

#define D_EPSILON	0.000001

#define max(x,y)	((x)>(y) ? (x) : (y))

typedef double (*mfunc_1arg) (double x);
typedef double (*mfunc_2arg) (double x, double y);

phcalc_num mproc_1arg(mfunc_1arg func, phcalc_num x);
phcalc_num mproc_2arg(mfunc_2arg func, phcalc_num x, phcalc_num y);

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
	} else if( strcmp(name,"ArcTan2")==0 ){
		assert(args[0].type==PHC_OBJ_NUM);
		assert(args[1].type==PHC_OBJ_NUM);
		fr = phcalc_arctan2(args[0].ref.num,args[1].ref.num);
	}
	
	else if( strcmp(name,"Length")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_length(&fr,args[0].ref.vect) );
	} else if( strcmp(name,"Sum")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_sum(&fr,args[0].ref.vect) );
	} else if( strcmp(name,"Average")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_average(&fr,args[0].ref.vect) );
	} else if( strcmp(name,"Mean")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_mean(&fr,args[0].ref.vect) );
	} else if( strcmp(name,"Dot")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert(args[1].type==PHC_OBJ_VECT);
		ret_num = phcalc_dot(&fr,args[0].ref.vect,args[1].ref.vect);
	} else if( strcmp(name,"Norm")==0 ){
		assert(args[0].type==PHC_OBJ_VECT);
		assert( phcalc_norm(&fr,args[0].ref.vect) );
	} else
		ret_num = 0;
	if(ret_num){
		res->type = PHC_OBJ_NUM;
		res->ref.num = fr;
		return 1;
	}
	return 0;
}

phcalc_num mproc_1arg(mfunc_1arg func, phcalc_num x) {
	double val = func(x.value);
	double min = val, max = val;
	double vals[3];
	int i;
	phcalc_num res;
	vals[0] = func(x.value-x.error);
	vals[1] = func(x.value+x.error);
	vals[2] = val;
	for(i=0; i<3; i++){
		if( vals[i] < min )
			min = vals[i];
		if( vals[i] > max )
			max = vals[i];
	}
	res.value = val;
	res.error = max( max-val, val-min );
	return res;
}

phcalc_num mproc_2arg(mfunc_2arg func, phcalc_num x, phcalc_num y) {
	double val = func(x.value,y.value);
	double min = val, max = val;
	double vals[9];
	int i;
	phcalc_num res;
	vals[0] = func(x.value-x.error,y.value-y.error);
	vals[1] = func(x.value+x.error,y.value-y.error);
	vals[2] = func(x.value-x.error,y.value+y.error);
	vals[3] = func(x.value+x.error,y.value+y.error);
	vals[4] = val;
	vals[5] = func(x.value,y.value-y.error);
	vals[6] = func(x.value,y.value+y.error);
	vals[7] = func(x.value-x.error,y.value);
	vals[8] = func(x.value+x.error,y.value);
	for(i=0; i<7; i++){
		if( vals[i] < min )
			min = vals[i];
		if( vals[i] > max )
			max = vals[i];
	}
	res.value = val;
	res.error = max( max-val, val-min );
	return res;
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

double phcalc_muld(double x, double y) {
	return x*y;
}
phcalc_num phcalc_mul(phcalc_num x, phcalc_num y) {
	return mproc_2arg(phcalc_muld,x,y);
}

double phcalc_divd(double x, double y) {
	return x/y;
}
phcalc_num phcalc_div(phcalc_num x, phcalc_num y) {
	return mproc_2arg(phcalc_divd,x,y);
}

double phcalc_powd(double x, double y) {
	return pow(x,y);
}
phcalc_num phcalc_pow(phcalc_num x, phcalc_num y) {
	return mproc_2arg(phcalc_powd,x,y);
}

phcalc_num phcalc_abs(phcalc_num x) {
	return mproc_1arg(dabs,x);
}

phcalc_num phcalc_sqrt(phcalc_num x) {
	return mproc_1arg(sqrt,x);
}

phcalc_num phcalc_log(phcalc_num x) {
	return mproc_1arg(log,x);
}

phcalc_num phcalc_exp(phcalc_num x) {
	return mproc_1arg(exp,x);
}

phcalc_num phcalc_sin(phcalc_num x) {
	return mproc_1arg(sin,x);
}

phcalc_num phcalc_cos(phcalc_num x) {
	return mproc_1arg(cos,x);
}

phcalc_num phcalc_tan(phcalc_num x) {
	return mproc_1arg(tan,x);
}

phcalc_num phcalc_arcsin(phcalc_num x) {
	return mproc_1arg(asin,x);
}

phcalc_num phcalc_arccos(phcalc_num x) {
	return mproc_1arg(acos,x);
}

phcalc_num phcalc_arctan(phcalc_num x) {
	return mproc_1arg(atan,x);
}

phcalc_num phcalc_arctan2(phcalc_num x, phcalc_num y) {
	return mproc_2arg(atan2,x,y);
}

int phcalc_length(phcalc_num *res, phcalc_vect vector) {
	*res = phcalc_num_new(vector.len,0.0);
	return 1;
}

int phcalc_sum(phcalc_num *res, phcalc_vect vector) {
	int i;
	phcalc_num sum = { 0, 0 };
	for(i=0; i<vector.len; i++){
		if(vector.data[i].type!=PHC_OBJ_NUM)
			return 0;
		sum = phcalc_add(sum,vector.data[i].ref.num);
	}
	*res = sum;
	return 1;
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

int phcalc_dot(phcalc_num *res, phcalc_vect vector1, phcalc_vect vector2) {
	int i;
	phcalc_num sum = { 0, 0 };
	if( vector1.len != vector2.len )
		return 0;
	for(i=0; i<vector1.len; i++){
		if( vector1.data[i].type!=PHC_OBJ_NUM || vector2.data[i].type!=PHC_OBJ_NUM )
			return 0;
		sum = phcalc_add( sum, phcalc_mul(vector1.data[i].ref.num, vector2.data[i].ref.num) );
	}
	*res = sum;
	return 1;
}

int phcalc_norm(phcalc_num *res, phcalc_vect vector) {
	int i;
	phcalc_num sum = { 0, 0 };
	for(i=0; i<vector.len; i++){
		if( vector.data[i].type!=PHC_OBJ_NUM )
			return 0;
		sum = phcalc_add( sum, phcalc_mul(vector.data[i].ref.num, vector.data[i].ref.num) );
	}
	*res = phcalc_sqrt(sum);
	return 1;
}