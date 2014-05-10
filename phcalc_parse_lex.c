/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 *
 * Lexic analysator source file (phcalc_parse.c)
 *
 **************************************/

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "phcalc.h"
#include "phcalc_in.h"
#include "dynarr.h"
#include "phcalc_parse.h"

#define LEX_MAXNAMELEN	256

int phcalc_parse_lexic(FILE *fd, ttoken **tokens, int *len, tparseerr *err) {
	int line = 0, pos = -1;
	ttokentype state = TOKEN_NEW;

	char name[LEX_MAXNAMELEN];
	int name_p = 0;
	int token_pos = -1;

	dynarr_create((void**)tokens,sizeof(ttoken));

	for(;;){
		char c = fgetc(fd);
		int fnext = 0;
		pos ++;
		if(feof(fd))
			break;
		while(!fnext){
			fnext = 1;
			switch(state){
			case TOKEN_NEW:
				token_pos = pos;
				name_p = 0;
				switch(c){
				case '+':
					dynarr_add((void**)tokens,new_token(TOKEN_PLUS,0,line,pos));
					break;
				case '-':
					dynarr_add((void**)tokens,new_token(TOKEN_MINUS,0,line,pos));
					break;
				case '*':
					dynarr_add((void**)tokens,new_token(TOKEN_ASTER,0,line,pos));
					break;
				case '/':
					dynarr_add((void**)tokens,new_token(TOKEN_SLASH,0,line,pos));
					break;
				case ';':
					dynarr_add((void**)tokens,new_token(TOKEN_ENDLN,0,line,pos));
					break;
				case '=':
					dynarr_add((void**)tokens,new_token(TOKEN_ASSIGN,0,line,pos));
					break;
				case '(':
					dynarr_add((void**)tokens,new_token(TOKEN_BRPO,0,line,pos));
					break;
				case ')':
					dynarr_add((void**)tokens,new_token(TOKEN_BRPC,0,line,pos));
					break;
				case '[':
					dynarr_add((void**)tokens,new_token(TOKEN_BRSO,0,line,pos));
					break;
				case ']':
					dynarr_add((void**)tokens,new_token(TOKEN_BRSC,0,line,pos));
					break;
				case '{':
					dynarr_add((void**)tokens,new_token(TOKEN_BRCO,0,line,pos));
					break;
				case '}':
					dynarr_add((void**)tokens,new_token(TOKEN_BRCC,0,line,pos));
					break;
				default:
					if( (c>'a'&&c<'z') || (c>'A'&&c<'Z') || c=='_' ){
						state = TOKEN_NAME;
						fnext = 0;
					} else if( isspace(c) ){
						state = TOKEN_SPACE;
						fnext = 0;
					} else if( (c>'0'&&c<'9') ){
						state = TOKEN_NUMBER;
						fnext = 0;
					} else {
						char buf[2] = { c, 0 };
						phcalc_parse_newerror(err,line,pos,0,"Unknown symbol",buf);
						*len = dynarr_desable((void**)tokens);
						return 0;
					}
				}
				break;
			case TOKEN_SPACE:
				if( c=='\n' ){
					line ++;
					pos = -1;
				}
				if( !isspace(c) ){
					state = TOKEN_NEW;
					fnext = 0;
				}
				break;
			case TOKEN_NAME:
				if( (c>'a'&&c<'z') || (c>'A'&&c<'Z') || (c>'0'&&c<'9') || c=='_' ){
					name[name_p++] = c;
				} else {
					name[name_p] = 0;
					dynarr_add((void**)tokens,new_token(TOKEN_NAME,name,line,token_pos));
					state = TOKEN_NEW;
					fnext = 0;
				}
				break;
			case TOKEN_NUMBER:
				if( (c>='0'&&c<='9') || c=='.' ){
					name[name_p++] = c;
				} else {
					name[name_p] = 0;
					dynarr_add((void**)tokens,new_token(TOKEN_NUMBER,name,line,token_pos));
					state = TOKEN_NEW;
					fnext = 0;
				}
				break;
			}
		}
	}
	dynarr_add((void**)tokens,new_token(TOKEN_EOF,0,line,pos));
	*len = dynarr_desable((void**)tokens);
	return 1;
}

ttoken new_token(ttokentype type, const char *name, int line, int pos) {
	ttoken token;
	token.type = type;
	token.line = line;
	token.pos  = pos;
	if(name==0)
		token.str = 0;
	else
		token.str  = _strdup(name);
	return token;
}