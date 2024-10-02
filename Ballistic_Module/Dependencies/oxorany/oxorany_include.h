#ifndef OXORANY_INCLUDE_H
#define OXORANY_INCLUDE_H

//disable /GL for warning C4307 in msvc
#define OXORANY_DISABLE_OBFUSCATION
//use OXORANY_USE_BIT_CAST for remove float double src data
#define OXORANY_USE_BIT_CAST

#include "oxorany.h"

//to change Marco in a easy way
#define WRAPPER_MACRO oxorany
#define WRAPPER_MACRO_FLT oxorany_flt

template <typename T>
static OXORANY_FORCEINLINE void copy_string_without_return(T* target, const T* source)
{
	while (*source) {
		*target = *source;
		++source;
		++target;
	}
	*target = 0;
}

#define WRAPPER_MACRO_DEFINE_STRING_ARRAY(type,name,s) type name[sizeof(s)/sizeof(type)]; copy_string_without_return(name, WRAPPER_MACRO(s))

#endif