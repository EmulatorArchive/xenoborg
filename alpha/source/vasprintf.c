#pragma warning (disable:4996)
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include "vasprintf.h"

int vasprintf( char **sptr, const char *fmt, va_list argv ) 
{ 
    int wanted = vsnprintf( *sptr = NULL, 0, fmt, argv ); 
    if( (wanted > 0) && ((*sptr = malloc( 1 + wanted )) != NULL) ) 
        return vsprintf( *sptr, fmt, argv ); 
 
    return wanted; 
} 