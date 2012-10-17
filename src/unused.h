/* $Id: unused.h,v 1.2 2012/10/17 11:36:54 ozzmosis Exp $ */

/*
 *  Suppress warning about unused parameter. Written by Paul Edwards and
 *  released to the public domain.  Modified by Andrew Clarke.
 */

#ifndef unused
#if defined(PACIFIC) || defined(_MSC_VER) || defined(__LCC__)
#define unused(x)
#elif defined(__HIGHC__)
#define unused(x) ((x) = (x))
#else
#define unused(x) ((void)(x))
#endif
#endif
