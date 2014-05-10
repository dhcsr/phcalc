#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "phcalc.h"

int main(int argc, char *argv[]){
	FILE *fd = fopen("prog.txt","rt");
	phcalc_parsefile(fd);
	/*char buf[256];
	phcalc_inst calc = phcalc_create_inst();
	phcalc_expr e0 = phcalc_parse(calc,"x:={1,2}");
	phcalc_expr e1 = phcalc_parse(calc,"x+5");
	phcalc_obj r1;
	//phcalc_strexpr(calc,e0,buf,256);
	phcalc_query(calc,e0);
	phcalc_eval(calc,e1,&r1);
	phcalc_expr_release(calc,e0);
	//phcalc_expr e1 = phcalc_parse(calc,"x:={0.1,0.2,0.3,0.4,0.5,0.6}");
	//phcalc_expr e2 = phcalc_parse(calc,"f[x]:=x*sin[x]");
	for(;;){
		phcalc_expr e;
		gets(buf);
		e = phcalc_parse(calc,buf);
		phcalc_strexpr(calc,e,buf,256);
		puts(buf);
	}
	phcalc_destroy_inst(calc);
	return 0;*/
}