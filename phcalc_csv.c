#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "phcalc.h"
#include "phcalc_in.h"

#define LINE_BUF_SZ		512

int phcalc_csv_load(phcalc_inst inst, FILE *file, const char **fields, int n_fields, unsigned long flags) {
	char csv_sep = ',';
	char csv_dec = '.';
	char buf[LINE_BUF_SZ],
		buf2[64];
	int p, p0, i;

	while( !feof(file) ){
		fgets(buf,LINE_BUF_SZ,file);
		i = 0;
		p0 = 0;
		for(p=0; buf[p]!=0; p++){
			if( buf[p]==csv_sep || buf[p]=='\n' || buf[p]=='\0' ){
				memcpy(buf2,buf+p0,p-p0);
				buf2[p-p0] = 0;
				p0 = p+1;
			}
			if( buf[p]=='\n' || buf[p]=='\0' )
				break;
		}
	}
	return 0;
}

int phcalc_csv_save(phcalc_inst inst, FILE *file, const char **fields, int n_fields, unsigned long flags) {
	return 0;
}