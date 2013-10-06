/* $Id: version.h,v 1.29 2013/10/06 19:44:49 ajleary Exp $  */

#include "os.h"

#define MAKENL_VERSION      "3.4.0"

#define MAKENL_LONG_VERSION "MakeNL " MAKENL_VERSION " (" OS_NAME ") compiled with " CC_NAME " on " __DATE__ " " __TIME__

#define MAKENL_CREDITS \
  "\n" \
  "This version of MakeNL is based upon work by Michael Karcher with" "\n" \
  "modifications by Björn Felten, Robert Couture, Andrew Clarke," "\n" \
  "Michiel Broek, Malcolm Miles, Johan Billing, Andrew Leary & others." "\n" \
  "\n" \
  "Dedicated to Ben Baker - The author of the original MakeNL.\n" \
  "\n"
