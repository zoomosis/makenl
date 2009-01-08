/* $Id: unused.h,v 1.1.1.1 2009/01/08 20:07:47 mbroek Exp $ */

/*
 *  Suppress warning about unused parameter. Written by Paul Edwards and
 *  released to the public domain.  Modified by Andrew Clarke.
 */

#ifndef unused
#if defined(PACIFIC) || defined(_MSC_VER)
#define unused(x)
#elif defined(__HIGHC__)
#define unused(x) ((x) = (x))
#else
#define unused(x) ((void)(x))
#endif
#endif
