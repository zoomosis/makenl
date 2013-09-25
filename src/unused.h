/* $Id: unused.h,v 1.3 2013/09/25 20:42:16 ozzmosis Exp $ */

/*
 *  Suppress warning about unused parameter. Written by Paul Edwards and
 *  released to the public domain.  Modified by Andrew Clarke.
 */

#ifndef unused

#if defined(PACIFIC) || defined(_MSC_VER) || defined(__LCC__) || defined(__HIGHC__)
#define unused(x)
#else
#define unused(x) ((void)(x))
#endif

#endif
