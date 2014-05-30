#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "phcalc.h"
#include "phcalc_in.h"
#include "dynarr.h"

#define LINE_BUF_SZ		512

int phcalc_csv_load(phcalc_inst inst, FILE *file, const char **fields, int n_fields, unsigned long flags) {
	char csv_sep = ',';
	char csv_dec = '.';
	char buf[LINE_BUF_SZ],
		buf2[64];
	int p, p0, i, j, count;
	double **listing;
	dynarr_create( &listing, sizeof(double*) );

	while( !feof(file) ){
		double *row;
		char *res = fgets(buf,LINE_BUF_SZ,file);
		if(res==0)
			break;
		row = NEWS(double,n_fields);
		i = 0;
		p0 = 0;
		for(p=0; 1; p++){
			if( buf[p]==csv_sep || buf[p]=='\n' || buf[p]=='\0' ){
				memcpy(buf2,buf+p0,p-p0);
				buf2[p-p0] = 0;
				if( i < n_fields )
					sscanf(buf2,"%lf",&row[i]);
				i ++;
				p0 = p+1;
			}
			if( buf[p]=='\n' || buf[p]=='\0' )
				break;
		}
		dynarr_add( &listing, row );
	}
	count = dynarr_desable( &listing );
	for(i=0; i<n_fields; i++){
		phcalc_obj field;
		field.type = PHC_OBJ_VECT;
		field.ref.vect.len  = count;
		field.ref.vect.data = NEWS(phcalc_obj,count);
		for(j=0; j<count; j++){
			field.ref.vect.data[j].type = PHC_OBJ_NUM;
			field.ref.vect.data[j].ref.num.value = listing[j][i];
			field.ref.vect.data[j].ref.num.error = 0.0;
		}
		phcalc_adddef_obj_nocopy(inst,fields[i],field,1);
	}
	for(j=0; j<count; j++)
		FREE(listing[j]);
	FREE(listing);
	return 0;
}

int phcalc_csv_save(phcalc_inst inst, FILE *file, const char **fields, int n_fields, unsigned long flags) {
	return 0;
}
