#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "phcalc.h"

int main_routine();

int main(int argc, char *argv[]){
	/*FILE *fd = fopen("prog.txt","rt");
	phcalc_inst calc = phcalc_parsefile(fd);
	phcalc_expr e1 = phcalc_parse("x");
	phcalc_obj r1;
	phcalc_eval(calc,e1,&r1);
	return 0;*/
	main_routine();
}

int main_routine() {
	phcalc_inst calc = phcalc_create_inst();
	char buf[512];
	for(;;){
		char *arg;
		gets(buf);
		arg = strchr(buf,' ');
		if(arg!=0){
			arg[0] = 0;
			arg ++;
		}
		if( strcmp(buf,"query")==0 && arg!=0 ){
			phcalc_expr e = phcalc_parse(arg);
			if(e==0){
				printf("Unable to parse\n");
			} else {
				phcalc_strexpr(calc,e,buf,256);
				printf("%s\n",buf);
				phcalc_query(calc,e);
			}
		} else if( strcmp(buf,"eval")==0 && arg!=0 ){
			phcalc_expr e = phcalc_parse(arg);
			phcalc_obj r;
			phcalc_eval(calc,e,&r);
			phcalc_strobj(&r,buf,512);
			printf("%s\n",buf);
		} else
			printf("Unknown command\n");
		//puts(buf);
	}
	phcalc_destroy_inst(calc);
	return 0;
}