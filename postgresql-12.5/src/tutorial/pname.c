/*
 * src/tutorial/pname.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
//#include "access/hash.h"
#include "utils/hashutils.h"
#include "utils/builtins.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>


#define TRUE 1
#define FALSE 0

PG_MODULE_MAGIC;

typedef struct
{
	int32 length;
	char name[FLEXIBLE_ARRAY_MEMBER];
} PersonName;


// function used to compile the regrex and execute it
static int regexMatch(char * str, char * regexPattern) {
	regex_t regex;
	int match = FALSE;
	// compile the regex
	if(regcomp(&regex, regexPattern, REG_EXTENDED)){
		return FALSE;
	}
	// execute the regex
	if(regexec(&regex, str, 0, NULL, 0) == 0) {
		match = TRUE;
	}
	// free the regex
	regfree(&regex);
	return match;
}


// function to check family name by regex
static int check_family_name(char * s){
	
	// match the one or more string start with a Capital letter and then any letter(ignore case)
    // and '-' and '\''. then end with ' ' or not 	
	char * re = "^(([A-Z][-a-zA-Z\']+)[ ]?)+$";
	if(!regexMatch(s, re)) {	
		return FALSE;
	}
	return TRUE;
	
}


// function to check given name by regex
static int check_given_name(char * s){
	
	// match the one or more string start with ' 'or not, followed by a Capital letter and then any letter(ignore case)
    // and '-' and '\''. then end with ' ' or not 	
 	char * re = "^[ ]?(([A-Z][-a-zA-Z\']+)[ ]?)+$";
	if(!regexMatch(s, re)) {	
		return FALSE;
	}
	return TRUE;
}



// function to check the name is valid
static int valid_name(char *str)
{
	char *family_name=NULL;
	char *given_name=NULL;
	int have=0;
	// find the first ','
	for(int i=0;i<strlen(str);i++){
			if(str[i]==','){
				have=1;
				break;
			}
	}
	// if there is no ',' it is a invalid name
	if(have==0){
		return FALSE;
	}
	if(str[0]==','){
		return FALSE;
	}
	// split the name by ','
	family_name=strtok_r(str,",",&given_name);
	
	// if family_name starts with ' ', it is not valid
	if(family_name[0]==' '){
		return FALSE;
	}
	// if the last name of family_name or given_name end with ' ',is it not valid 
	if(family_name[strlen(family_name)-1]==' '||given_name[strlen(given_name)-1]==' '){
		return FALSE;
	}
	// check family_name and given_name by the function above
	if(check_family_name(family_name)==FALSE||check_given_name(given_name)==FALSE){
		return FALSE;
	}
	
	return TRUE;
	

}


// function to delete all the ' ' in the left of a string
// eg "  a b " -> "a b "
static char *lltrim(char *str){
	int i;
	for(i=0;i<=strlen(str);i++){
		if(isspace(*str)){
			str++;
		}
		else{
			break;
		}
	}
	return str;
}



// function to compare the person name
static int pname_cmp(PersonName *p1,PersonName *p2){
	int len1=VARSIZE_ANY_EXHDR(p1);
	int len2=VARSIZE_ANY_EXHDR(p2);
	char *name1;
	char *name2;
	char *family_name1=NULL;
	char *family_name2=NULL;
	char *given_name1=NULL;
	char *given_name2=NULL;

	name1=(char *) palloc(len1);
	name2=(char *) palloc(len2);

	// store the name in a new memory, in order not to change the original one
	snprintf(name1, len1, "%s", p1->name);
	snprintf(name2, len2, "%s", p2->name);
	
	//split the string by ',', the first part become family_name and second part become given_name
	family_name1=strtok_r(name1,",",&given_name1);
	family_name2=strtok_r(name2,",",&given_name2);

	// if the family_name is the same
	if(strcmp(family_name1,family_name2)==0){
		
		// compare the given_name
		return strcmp(given_name1,given_name2);
	}
	else{
		return strcmp(family_name1,family_name2);
	}

}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

// function for input
PG_FUNCTION_INFO_V1(pname_in);

Datum
pname_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	int		   len;
	PersonName    *result;
	char *family = NULL;
	char *given = NULL;
	char *temp1=(char *)palloc(strlen(str)+1);
	char *temp2=(char *)palloc(strlen(str)+1);	
	// copy the name
	char *temp=(char *)palloc(strlen(str)+1);
	snprintf(temp, strlen(str)+1, "%s", str);
	
	// check the name 
	if(!valid_name(temp)){
		ereport(ERROR,
		(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
		 errmsg("invalid input syntax for type %s: \"%s\"",
				"PersonName", str)));
	}
	snprintf(temp1, strlen(str)+1, "%s", str);
	snprintf(temp2, strlen(str)+1, "%s", str);
	// the name is valid, then split them and format them
	family=strtok_r(temp1,",",&given);
	given=lltrim(given);
	// regenerate the name and remove the space if there exists in the begining of given name 
	sprintf(temp2,"%s,%s",family,given);	
	len=strlen(temp2)+1;
	result = (PersonName *) palloc(VARHDRSZ+len);
	SET_VARSIZE(result,VARHDRSZ+len); 
	snprintf(result->name,len,"%s",temp2);
	PG_RETURN_POINTER(result);
}

// function for output
PG_FUNCTION_INFO_V1(pname_out);

Datum
pname_out(PG_FUNCTION_ARGS)
{
	PersonName    *str = (PersonName *) PG_GETARG_POINTER(0);
	int len=VARSIZE_ANY_EXHDR(str);
	char	   *result;
	result = (char *) palloc(len);
	snprintf(result, len, "%s", str->name);
	PG_RETURN_CSTRING(result);
}


// function to show the family name
PG_FUNCTION_INFO_V1(family);

Datum
family(PG_FUNCTION_ARGS)
{
	text    *pname = (text *) PG_GETARG_TEXT_PP(0);
	text   *result;
	char *temp;
	char *family=NULL;
	char *given=NULL;
	text *family_result;
	temp=text_to_cstring(pname);
	family=strtok_r(temp,",",&given);
	family_result=cstring_to_text(family);
	result = (text *) palloc(VARHDRSZ+VARSIZE_ANY_EXHDR(family_result));
	SET_VARSIZE(result,VARHDRSZ+VARSIZE_ANY_EXHDR(family_result));
	memcpy(VARDATA(result),VARDATA_ANY(family_result),VARSIZE_ANY_EXHDR(family_result));
	PG_RETURN_TEXT_P(result);
}


// function to show the given name
PG_FUNCTION_INFO_V1(given);

Datum
given(PG_FUNCTION_ARGS)
{
	text    *pname = (text *) PG_GETARG_TEXT_PP(0);
	text   *result;
	char *temp;
	char *given=NULL;
	text *given_result;
	temp=text_to_cstring(pname);
	strtok_r(temp,",",&given);

	given_result=cstring_to_text(given);
	result = (text *) palloc(VARHDRSZ+VARSIZE_ANY_EXHDR(given_result));
	SET_VARSIZE(result,VARHDRSZ+VARSIZE_ANY_EXHDR(given_result));
	memcpy(VARDATA(result),VARDATA_ANY(given_result),VARSIZE_ANY_EXHDR(given_result));
	PG_RETURN_TEXT_P(result);

}


// function to show the name
PG_FUNCTION_INFO_V1(show);

Datum
show(PG_FUNCTION_ARGS)
{
	text    *pname = (text *) PG_GETARG_TEXT_PP(0);
	text   *result;
	char *temp;
	char *final;
	char *family=NULL;
	char *given=NULL;
	char *result_given;
	text *temp_result;
	temp=text_to_cstring(pname);
	family=strtok_r(temp,",",&given);

	
	result_given=(char *)palloc(strlen(given)+1);
	final=(char *)palloc(VARHDRSZ+VARSIZE_ANY_EXHDR(pname)+1);
	if(isspace(*given)){
		sscanf(given,"%*[ \t\n]%s",result_given);
	}
	else{
		sscanf(given,"%s",result_given);
	}
	sprintf(final,"%s %s",result_given,family);


	temp_result=cstring_to_text(final);
	result = (text *) palloc(VARHDRSZ+VARSIZE_ANY_EXHDR(temp_result));
	SET_VARSIZE(result,VARHDRSZ+VARSIZE_ANY_EXHDR(temp_result));
	memcpy(VARDATA(result),VARDATA_ANY(temp_result),VARSIZE_ANY_EXHDR(temp_result));
	PG_RETURN_TEXT_P(result);

}

// function to judge one name larger than the other
PG_FUNCTION_INFO_V1(pname_bigger);

Datum
pname_bigger(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_BOOL(a>0);
}

// function to judge one name larger and equal than the other
PG_FUNCTION_INFO_V1(pname_bigger_equal);

Datum
pname_bigger_equal(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_BOOL(a>=0);
}

// function to judge one name less than the other
PG_FUNCTION_INFO_V1(pname_less);

Datum
pname_less(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_BOOL(a<0);
}

// function to judge one name less and equal than the other
PG_FUNCTION_INFO_V1(pname_less_equal);

Datum
pname_less_equal(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_BOOL(a<=0);
}

// function to judge one name equal to the other
PG_FUNCTION_INFO_V1(pname_equal);

Datum
pname_equal(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_BOOL(a==0);
}

// function to judge one name not equal to the other
PG_FUNCTION_INFO_V1(pname_not_equal);

Datum
pname_not_equal(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_BOOL(a!=0);
}

// function for btree
PG_FUNCTION_INFO_V1(pname_abs_cmp);

Datum
pname_abs_cmp(PG_FUNCTION_ARGS){
	PersonName    *pname1 = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *pname2 = (PersonName *) PG_GETARG_POINTER(1);
	int a;
	a=pname_cmp(pname1,pname2);
	PG_RETURN_INT32(a);
}

// function for hash
PG_FUNCTION_INFO_V1(pname_own_hash);

Datum
pname_own_hash(PG_FUNCTION_ARGS){
	PersonName    *pname = (PersonName *) PG_GETARG_POINTER(0);
	int len=VARSIZE_ANY_EXHDR(pname);
	char *result;
	result=(char *)palloc(len);
	snprintf(result, len, "%s", pname->name);
	
	PG_RETURN_INT32(DatumGetInt32(hash_any((const unsigned char *)result,len)));
}