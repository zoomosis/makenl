/* $Id: version.h,v 1.33 2014/03/28 19:47:45 ajleary Exp $  */

#include "os.h"

#define MAKENL_VERSION      "3.4.3"

#define MAKENL_LONG_VERSION "MakeNL " MAKENL_VERSION " (" OS_NAME ") compiled with " CC_NAME " on " __DATE__ " " __TIME__

#define MAKENL_LICENSE \
  "This program is free software; you can redistribute it and/or modify" "\n" \
  "it under the terms of the GNU General Public License as published by" "\n" \
  "the Free Software Foundation; either version 2 of the License, or" "\n" \
  "(at your option) any later version." "\n" \
  "\n" \
  "This program is distributed in the hope that it will be useful," "\n" \
  "but WITHOUT ANY WARRANTY; without even the implied warranty of" "\n" \
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" "\n" \
  "GNU General Public License for more details." "\n" \
  "\n" \
  "You should have received a copy of the GNU General Public License along" "\n" \
  "with this program; if not, write to the Free Software Foundation, Inc.," "\n" \
  "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA." "\n" \
  "\n"

#define MAKENL_CREDITS \
  "\n" \
  "This version of MakeNL is based upon work by Michael Karcher with" "\n" \
  "modifications by Björn Felten, Robert Couture, Andrew Clarke," "\n" \
  "Michiel Broek, Malcolm Miles, Johan Billing, Andrew Leary & others." "\n" \
  "\n" \
  "Dedicated to Ben Baker - The author of the original MakeNL.\n" \
  "\n" \
  MAKENL_LICENSE
