#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "phcalc.h"

int main_routine();
void test_1();

int main(int argc, char *argv[]){
	test_1();
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
			if(e!=0){
				phcalc_eval(calc,e,&r);
				phcalc_strobj(&r,buf,512);
				printf("%s\n",buf);
			}
		} else if( strcmp(buf,"exit")==0 ){
			break;
		} else
			printf("Unknown command\n");
	}
	phcalc_destroy_inst(calc);
	return 0;
}