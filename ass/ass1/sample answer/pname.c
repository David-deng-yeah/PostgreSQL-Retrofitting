/*
 * File:    pname.c
 * Summery: Implement a datatype for Given name[s] and Family name[s]
 * Author:  Dylan Brothersotn
 * Date:    2020/03/09
 * Desctiption:
 *    Writen for COMP9315, 2020 Trimester 1.
 *    Assignemnt 1: "Adding a PersonName Data Type to PostgreSQL"
 *    C backend for a "PersonName" SQL type (see pname.source)
 */

#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "postgres.h"
#include "utils/builtins.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

// create a "string" type for convenience
typedef char* string;

// Internal struct for nicely handling PersonNames
typedef struct PersonName_
{
    string given;
    string family;
} PersonName_;

// Actual struct we will use for the datatype
typedef struct PersonName
{
    int32 vl_len_;
    char names[FLEXIBLE_ARRAY_MEMBER];
} PersonName;

// forward declarations for utility functions
static int32 struct_to_string(PersonName_ person, char **out);
static PersonName_ string_to_struct(PersonName *in);

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);
Datum pname_in(PG_FUNCTION_ARGS)
{
    string family = strdupa(PG_GETARG_CSTRING(0)); // see "pname_show()" for info on strdupa()

    string given = strchr(family, ',');            // get a pointer to the ','
    if (!given) goto pname_in__error;              // ERROR: no ',' only family name(s) given
    *given = '\0';                                 // set first ',' to '\0'
    given++;                                       // get pointer to start of given name(s)
    if (given[0] == ' ') given++;                  // skip one optianl space
    if (strchr(given, ',')) goto pname_in__error;  // ERROR: multiple ',' in the name input

    if (!family[0]) goto pname_in__error; // ERROR: empty family name
    for (char *p = family; *p ; p++) {
        // All characters must be alphabetical or '-' or '''
        if (!isalpha(*p) && *p != '\'' && *p != '-' && *p != ' ') goto pname_in__error;
        // If the letter is the first in a name
        if (p == family || (p != family && *(p-1) == ' ')) {
            // it must be upper-case
            if (!isupper(*p)) goto pname_in__error;
            // it must be followed by a valid character
            if (!isalpha(*(p+1)) && *(p+1) != '\'' && *(p+1) != '-') goto pname_in__error;
        }
        // All spaces must be followed by an upper-case letter
        if (*p == ' ' && !isupper(*(p+1))) goto pname_in__error;
    }

    if (!given[0]) goto pname_in__error; // ERROR: empty given name
    for (char *p = given; *p ; p++) {
        // All characters must be alphabetical or '-' or '''
        if (!isalpha(*p) && *p != '\'' && *p != '-' && *p != ' ') goto pname_in__error;
        // If the letter is the first in a name
        if (p == given || (p != given && *(p-1) == ' ')) {
            // it must be upper-case
            if (!isupper(*p)) goto pname_in__error;
            // it must be followed by a valid character
            if (!isalpha(*(p+1)) && *(p+1) != '\'' && *(p+1) != '-') goto pname_in__error;
        }
        // All spaces must be followed by an upper-case letter
        if (*p == ' ' && !isupper(*(p+1))) goto pname_in__error;
    }

    string name_string = NULL; // out parameter
    int32 name_length = struct_to_string((PersonName_){.given=given, .fmaily=family}, &name_string);

    // allocate the variable size struct
    PersonName *result = palloc(VARHDRSZ + name_length);
    SET_VARSIZE(result, VARHDRSZ + name_length);
    memcpy(result->names, name_string, name_length);

    PG_RETURN_POINTER(result);

// collect errors
pname_in__error:
    ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            errmsg("invalid input syntax for type %s: \"%s\"", "PersonName", PG_GETARG_CSTRING(0))));
}

PG_FUNCTION_INFO_V1(pname_out);
Datum pname_out(PG_FUNCTION_ARGS)
{
    PersonName_ pname = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PG_RETURN_CSTRING(psprintf("%s,%s", pname.family, pname.given));
}

/*****************************************************************************
 * New Functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_family);
Datum pname_family(PG_FUNCTION_ARGS)
{
    PersonName_ pname = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PG_RETURN_TEXT_P(cstring_to_text(psprintf("%s", pname.family)));
}

PG_FUNCTION_INFO_V1(pname_given);
Datum pname_given(PG_FUNCTION_ARGS)
{
    PersonName_ pname = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PG_RETURN_TEXT_P(cstring_to_text(psprintf("%s", pname.given)));
}

PG_FUNCTION_INFO_V1(pname_show);
Datum pname_show(PG_FUNCTION_ARGS)
{
    PersonName_ pname = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PG_RETURN_TEXT_P(cstring_to_text(psprintf("%s %s", strtok(strdupa(pname.given), " "), pname.family)));
    // use strtok() to find first space in given name(s)
    // strtok() will modify its first parameter so we need to strdup the given name(s)
    // strdupa() uses alloca(), not malloc(), this can have unexpected results, it's recommended to use malloc()/strdup()
    // strdupa() is a GNU extension not POSIX, postgres builds with `-std=gnu99` so GNU extensions are usable
}

/*****************************************************************************
 * Operators
 *****************************************************************************/

static int32 internal_pname_cmp(PersonName_ a, PersonName_ b)
{
    // if family name(s) are different return the diff, else return the diff of given name(s)
    if (strcmp(a.family, b.family)) return strcmp(a.family, b.family);
    return strcmp(a.given, b.given);
}

PG_FUNCTION_INFO_V1(pname_lt);
Datum pname_lt(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_BOOL(internal_pname_cmp(pname1, pname2) < 0);
}

PG_FUNCTION_INFO_V1(pname_le);
Datum pname_le(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_BOOL(internal_pname_cmp(pname1, pname2) <= 0);
}

PG_FUNCTION_INFO_V1(pname_eq);
Datum pname_eq(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_BOOL(internal_pname_cmp(pname1, pname2) == 0);
}

PG_FUNCTION_INFO_V1(pname_ne);
Datum pname_ne(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_BOOL(internal_pname_cmp(pname1, pname2) != 0);
}

PG_FUNCTION_INFO_V1(pname_ge);
Datum pname_ge(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_BOOL(internal_pname_cmp(pname1, pname2) >= 0);
}

PG_FUNCTION_INFO_V1(pname_gt);
Datum pname_gt(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_BOOL(internal_pname_cmp(pname1, pname2) > 0);
}

/*****************************************************************************
 * Indexing Functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_hash);
Datum pname_hash(PG_FUNCTION_ARGS)
{
    // djb2 string hashing -- http://www.cse.yorku.ca/~oz/hash.html
    // "utils/hashutils.h" hash_any() is better but OTT and slow.
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));

    int32 hash = 5381;
    int c;

    while ((c = *pname1.family++)) hash = ((hash << 5) + hash) + c;
    while ((c = *pname1.given++)) hash = ((hash << 5) + hash) + c;

    PG_RETURN_INT32(hash);
}

PG_FUNCTION_INFO_V1(pname_cmp);
Datum pname_cmp(PG_FUNCTION_ARGS)
{
    PersonName_ pname1 = string_to_struct((PersonName *) PG_GETARG_POINTER(0));
    PersonName_ pname2 = string_to_struct((PersonName *) PG_GETARG_POINTER(1));

    PG_RETURN_INT32(internal_pname_cmp(pname1, pname2));
}

/*****************************************************************************
 * Internal Utility Functions
 *****************************************************************************/

static int32 struct_to_string(PersonName_ person, char **out)
{
    // alloc a string then copy struct members into the string, return total string length
    *out = palloc(strlen(person.given) + strlen(person.family) + 2);
    memcpy(*out, person.given, strlen(person.given) + 1);
    memcpy(*out + strlen(person.given) + 1, person.family, strlen(person.family) + 1);
    return strlen(person.given) + strlen(person.family) + 2;
}

static PersonName_ string_to_struct(PersonName *in)
{
    // don't alloc so we don't need to free: just return pointers to existing data.
    PersonName_ out;
    out.given = in->names;
    out.family = in->names+strlen(in->names) + 1;
    return out;
}
