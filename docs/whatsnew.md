## MakeNL Revision History

### v3.5.1 --

### June 30, 2019

* Fixed a bug where *.MSG files failed to be null-terminated.

### June 24, 2019

* The MSGID sequence file sequence.dat is now stored as text for maximum
  portability between 16/32/64-bit systems and big/little endian CPUs.
  An existing sequence.dat in binary format will be converted to text
  automatically.

### v3.5.0 -- May 04, 2019

* Release as version 3.5.0 for wide distribution.

-- Robert James Clay

### May 04, 2019

* Fix Slackware package build.
* Clean up "this statement may fall through" warnings.

-- Andrew Clarke

### May 03, 2019

* Always require snprintf() for MSVC build.
* Reduce warnings in src/makefie.msvc from '-W3' to '-W2'.
* Update src/os.h that Microsoft C/C++ 8.0 (1993) and above is MSVC.
* Convert the WHATSNEW and README files to Markdown format.
* Use Pandoc in the makefiles to convert Markdown files to HTML.
* Move the example 'CTL' files from 'docs' to their own 'examples' directory.

-- Andrew Clarke

### May 01, 2019

* Convert the MakeNL 2.51 manual to Markdown format.
* Add using Pandoc in the CMake files to convert Markdown files to HTML.
* Remove obsolete $Id tags that don't work with Git.

-- Andrew Clarke

### v3.4.9 -- January 11, 2019

* Release version 3.4.9 for wide distribution.
* Remove old and unneeded cvs ID comment line from src/history.txt.

-- Robert James Clay

### November 25, 2018

* Bug fix: mklog() would clobber errno.
* Fix warning from the src/os.c file.
* Add xstrerror(), a wrapper for strerror().
* Use $ prefix with mklog() and let it call xstrerror().
* Correct mode of src/makefile.dmc from '100755' to '100644'.
* Improve error checking in the src/fileutil.c, src/lsttool.c, src/makenl.c,
  src/merge.c, src/mkdiff.c, src/msgtool.c, src/output.c, and src/os.c files.

-- Andrew Clarke

### November 21, 2018

* Remove redundant code in src/fts5.c and src/output.c.
* Updated declaration of unpacker() as it is an internal function.

-- Andrew Clarke

### November 17, 2018

* Fix a couple of our HAVE_ #defines to equal 1, so we can use either #ifdef or
  #if to discover them.
* Fix OS X build: Our strlcpy() was conflicting with the one supplied by the OS.

-- Andrew Clarke

### v3.4.8 -- December 3, 2017

* Add support for TODAY and TOMORROW as valid nodelist publishing
  days. Thanks to Kees van Eeten for the patch.

### v3.4.7 -- December 2, 2016

* Fix a possible data corruption issue with the RemoveBOM option.
  Thanks to Wilfred van Velzen for the report.

-- Andrew Leary

### v3.4.6 -- November 27, 2016

* Add the RemoveBOM option requested by Michiel van der Vlist.
When enabled (RemoveBOM 1 in makenl.ctl), UTF-8 Byte Order Marks
will be stripped from incoming segments and comment files.

-- Andrew Leary

### January 1, 2015

* Reorganize the install of the documentation history and examples
into separate sub-directories when using CMake.

-- Robert James Clay

### v3.4.5 -- November 13, 2014

* Fix incorrect default destination for receipt messages.

-- Andrew Leary

### v3.4.4 -- July 6, 2014

* MakeNL will no longer add a trailing comma to nodelist entries
which have no flags.  FTS-5000 allows the comma, but does not
require it.

-- Andrew Leary

### v3.4.3 -- March 28, 2014

* Increased the maximum archiver command line to 256 bytes.
Thanks to Wilfred van Velzen (2:280/464) for the report and
patches.

-- Andrew Leary

### v3.4.2 -- December 30, 2013

* Fix incorrect .MSG header when SUBmitting segments to an
inter-zone destination.  The INTL line was correct, but the .MSG
header was not.

-- Andrew Leary

### v3.4.1 -- October 19, 2013

* Include GNU General Public License Version 2 as the license
that applies to all MakeNL code.  See license.txt.  Add
licensing information to the MakeNL credits screen.

-- Andrew Clarke

* Ignore CTRL-Z lines in COPyright, PROlog, and EPIlog files.
They were causing CRC errors in the Linux version.

-- Andrew Leary

### v3.4.0 -- October 6, 2013

* Release version of v3.3.10 for wide distribution.  Also fixed
the Slackware Slackbuild script for generating a Slackware
installation package.  Added information on the new Allow8Bit
control file option to the whatsnew.txt.

-- Andrew Leary

### v3.3.10 -- September 29, 2013

* Updated history.txt with a very brief synopsis of the changes
made since the last release.
* Added missing os_fullpath() function to allow the Watcom DOS
16-bit and DOS 32-bit versions to sucessfully build again.

-- Andrew Leary

* Added Microsoft C 6.0 for DOS port.
* Switched to strlcpy() and strlcat() whereever possible, to
prevent crashes/segfaults related to buffer overflows.
* Switched to using portable snprintf(), Copyright 1999 by Mark
Martinec.
* Added Solaris port.

-- Andrew Clarke

### v3.3.9 -- September 21, 2013

* Moved all of the operating system specific code to src/os.c.
Added port to TinyCC.

-- Andrew Clarke

* Added support for changing drives when needed to the DOS, OS/2,
and Windows versions.

-- Andrew Leary

### v3.3.8 -- September 4, 2013

* Fixed the OldExtensions processing again; I missed a couple
places...
* Bump version to 3.3.8; this is a release candidate for 3.4.0.

-- Andrew Leary

* Revert my changes that tried to fix Watcom OS/2 version.
* Fix code in src/msgtool.c that clang was complaining about.

-- Andrew Clarke

### v3.3.7 -- September 3, 2013

* Fix OldExtensions processing in src/process.c to properly go
back 7 weeks.
* Bump version to 3.3.7; this hopefully will be the last test
version before 3.4.0.

-- Andrew Leary

* Use Watcom's fnmatch() instead of patmat().
* Add CFLAGS to link stage in the BSD & Linux Makefiles.
free(WorkFile) sometimes fails because it's pointing to a (valid)
memory location instead of somewhere allocated with strdup() or
malloc().
* Add debug build with clang using AddressSanitizer.
* Uncomment in src/process.c code suspected of causing a Segmentation
Fault but may be fixed and shouldn't be simply commented out.

-- Andrew Clarke

### September 2, 2013

* Reformat src/CMakeLists.txt and add creation of install targets
to it.
* Import docs/CMakeLists.txt and top level CMakeLists.txt from my
local development branch.

-- Robert James Clay

### August 29, 2013

* EMX fix: gcc 3.3 does not support mixed mode executables.
* Remove the 'add_definitions' line from src/CMakeLists.txt as it
breaks the MSVC build.

-- Andrew Clarke

### August 28, 2013

* Add clean up of the 'WorkFile' to /src/makenl.c
* Create initial version of CMake build file src/CMakeLists.txt.

-- Andrew Clarke

### August 23, 2013

* IBM VisualAge C/C++ for OS/2 port.
* Fix OldExtensions[] array overrun segfault.
* Correction to how the stack is getting fixed plus cosmetic fixes in
the src/msgtool.c file.
* Possible fix for crash in the Watcom OS/2 version (bug #17), plus
patches for the IBM VisualAge C/C++, Borland C & Metaware High C
OS/2 versions.

-- Andrew Clarke

### August 11, 2013

* Borland C++ for OS/2 port.
* MetaWare High C for OS/2 port. Requires GNU Make.
* EMX makefile for building a standalone binary not requiring EMX.DLL.

-- Andrew Clarke

### v3.3.6 -- May 25, 2013

* Add new configuration option "Allow8Bit" which disables the
replacement of characters above 0x7f with question marks.  This
option defaults to 0 (off); to enable it add Allow8Bit 1 to your
.CTL file.

-- Andrew Leary

### v3.3.5 -- Feb 07, 2013

* Update docs/history.txt and src/version.h to UTF-8.
* Update Tracker information in the docs/whatsnew.txt file.

-- Robert James Clay

### v3.3.4 -- Jan 30, 2013

* Increase maximum nodelist segment age to 7 weeks.

-- Andrew Leary

### v3.3.3 -- Jan 14, 2013

* Update the docs/felten.txt file to UTF-8
* Update docs/fts-5000.txt with current FTS-5000.004 version.

-- Robert James Clay

### v3.3.2 -- Dec 28, 2012

* Change default for MINphone to 1.

-- Andrew Leary

### v3.3.1 -- Dec 25, 2012

* Include Copyright, Prolog, and Epilog files in all nodelist
segments, not just in composite nodelists.  The filenames (by
default) are cpyright.txt, prolog.txt, and epilog.txt in your
master directory.  They can be overridden with the COPyright,
PROlog, and EPIlog statements in your makenl.ctl file.

-- Andrew Leary

### v3.3.0 -- Nov 12, 2012

* Release version of v3.2.19 for wide distribution.

-- Andrew Leary

### v3.2.19 -- Nov 09, 2012

* Fixed a &lowbar;findfirst() bug in the MS C versions. Thanks for Wilfred van Velzen (2:280/464) for tracking this down.

-- Andrew Leary

### v3.2.18 -- Nov 04, 2012

*  Changed MakeNL to default to allowing -Unpublished- phone
numbers for Down nodes.  This is the behavior of Ben Baker's
original MakeNL.

-- Andrew Leary

### v3.2.17 -- Oct 23, 2012

* MakeNL now creates .MSG files with FTS-1 date/time fields,
instead of using SEAdog format.

-- Andrew Leary

### v3.2.16 -- Oct 16, 2012

* Added test directory for .CTL files to test functionality of
development versions.  Refactored mklog().  Debug logs are sent
to the logfile if LogLevel is higher than 1, or we're running
in debug mode (makenl -d.)
* Added support & fixes for building with lcc-win32.
* Added support for Watcom Linux build.

-- Andrew Clarke

### v3.2.15 -- Oct 15, 2012

* Fixed a bug that resulted in setting the FTS-1 Unused bit
instead of the HoldForPickup bit in MakeNL-created .msg files.
Fixed another bug that resulted in stripping all attributes
except Crash and Hold from MakeNL-created Receipt .msg files.

-- Andrew Leary


### v3.2.14 -- Oct 14, 2012

* Various logging code changes, attempting to fix a few
potential buffer overflows, including writing the command-line
arguments to the log file. Minor code clean up in other areas.
Also cleaned up the output slightly when using MakeNL
interactively.

-- Andrew Clarke


### v3.2.13 -- Oct 2, 2012

* Counting v3.2.9a as v3.2.10, v3.2.9b as v3.2.11, and v3.2.9c
as 3.2.12; setting this patch version as v3.2.13.
* Debian packaging is separately versioned and is now tracked
on its own branch; remove debian/ directory.

-- Robert James Clay


### v3.2.9c -- Sep 19, 2012 (Bugfix)

* Incorporated logfile bugfix (SourceForge ID#3531669.)
Thanks to Ulrich Schroeter (2:244/1120) for tracking
this one down and submitting a patch.

-- Andrew Leary


### v3.2.9b -- Feb 23, 2012 (Bugfix)

* Allow use of -Unpublished- with Hold nodes without
enabling Allowunpub, as per FTS-5000.002.

-- Andrew Leary


### v3.2.9a -- Feb 8, 2010 (Bugfixes)

* Removing superfluous usage of MAKE_SS() in src/makenl.c
and src/msgtool.c.
* Remove format related warnings in src/mklog.c.
* Fixes for Watcom C build.

-- Andrew Clarke

* Updated the support information in docs/whatsnew.txt,
especiallly that for the Bug Tracker.

-- Robert James Clay


### v3.2.9 -- Jan 31, 2010

* Release version of 3.2.7e for wide distribution.

-- Robert James Clay


### v3.2.8 -- Feb 1, 2009 (Not Released)

* Someone hatched out an entirely unofficial v3.2.8 archive
and the consensus was that we would skip that version and
go directly to v3.2.9.

-- Robert James Clay


### v3.2.7e -- Dec 8, 2007 (Bugfix)

* Fix for a bufferoverflow in the OS/2 version.

-- Michiel Broek


### v3.2.7d -- June 1, 2007 (Bugfixes)

* Don't allow -Unpublished- for Down and Hold nodes if Allowunpub
is not set.
* Don't include copyright, prolog and epilog unless we make the
composite list.
* If copyright, prolog or epilog is missing and we are making a
composite list, a warning is shown and logged.

-- Michiel Broek


### v3.2.7c -- May 28, 2007 (Bugfix)

* Improved check for misspelled -Unpublished-.
* Check for Alpha phone number, if there are no alpha characters
in the phone field treat the field as a normal phone number and
do the normal checks for a valid phone number.

-- Michiel Broek


### v3.2.7b -- May 23, 2007 (Bugfix)

* Added compiler defines to support 64 bit platforms.
* Fixed case sensivity to change -unpublished-, only change real
errors.
-- Michiel Broek

### v3.2.7a -- ???


### v3.2.7 -- Apr 19, 2007 (Release version)

* Release version of 3.2.6d for wide distribution.


### v3.2.6d -- Apr 08, 2007 (Another bugfix)

* lineread in mkdiff.c returned the empty line with the EOF char.
This causes a diff file with the last Copy command to copy one
line too many.
* Fixed file open in some places to handle textfiles as binary to
make the code more portable between platforms.
* Cleanup old files now uses the right filename extensions for the
archiver that is used in the .ctl file.

-- Michiel Broek & Kees van Eeten


### v3.2.6c -- Mar 31, 2007 (Bugfix version again)

* Removed the EOF character from the diff file because some systems
can't process a diff with EOF character.

-- Michiel Broek


### v3.2.6b -- Mar 24, 2007 (Bugfix version)

* The diff files created on &#42;nix systems had mixed cr-lf and lf
line terminators. I think the non &#42;nix versions were ok.

* I added a EOF character too in the diff file.

-- Michiel Broek.


### V3.2.6a -- Mar 04, 2007 (Bugfix version)

* Hold or Down nodes in the RIN section of the nodelist were flagged
as error.

-- Michiel Broek


### V3.2.6 -- Jul 22, 2005 (Release version)

* Bugtracker #2 addendum - corrected baudrate check for 9600 - mbroek
Requested Change from Z2:

* As requested by Franz Zimmer via Ulrich Schroeter's translation:

  the last changes in code after 3.12 to 3.13 is about Nodes and
  their points. If a Bossnode is flagged as Hold in the nodelist,
  the points listed under this bossnodes should be listed. It
  makes sense, 'causethe Hold implies a temporary offline
  state, so points doesn't moves to another bossnodes.
  If the Bossnode is flagged Down or Pvt, the points under such
  a boss should be removed by makenl. If a Bossnode is Down,
  it implies that this is not only temporary and Points often moves
  to other Bossnodes.

  Hopefully this will welcome R24 nodes to try this version and report
any issues they encounter.

    (Gee the commit message took longer than implementing the
code snippet :)


### V3.2.5c -- Jul 18, 2005 (development/bughunt version)

* Bugtracker bug #2. While this bug was about the name field,
it is fixed by adding a check on the baudrate field that
should have been there. Any list if valid baudrates in the
.ctl file wasn't checked too.
NOTE: not a new version yet, we wait on some other tests.


### V3.2.5b -- May 23, 2005 (development/bughunt version)

* Added logging of the OS and used compiler. Added logging of the
commandline and arguments.

* Minor change to detect a bug in the OS/2 32 bit version


### V3.2.5 -- December 12, 2004

* Added new .ctl keyword FORcesubmit, parameter must be 0 or 1,
default is 0. This forces submitting of unchanged segments but
only if the -P is used on the commandline.

* Fixed some configuration keywords with wrong parameter checks.

-- Michiel Broek


### V3.2.4 -- December 6, 2004

* MakeNL now no longer supports the '/' character for specifying
switches on the command line.  This is to allow full path names
to be used in the UNIX port.  The non-UNIX versions of MakeNL
no longer support '/' style switches either, for consistency.

* When specifying switches on the command line please use '-'
instead of '/', eg. "makenl -c", NOT "makenl /c".

-- Andrew Clarke


### V3.2.3 -- October 8, 2004

* When makenl starts with logging enabled, the current
directory and the name of the .ctl file is logged so that
if you run several setups using a common logfile you can
see which setup is being used.

* Added a check to see of both ArcCopy and ArcMove keywords
are used or not used.

-- Michiel Broek


### V3.2.2 -- September 5, 2004

* Added logging. Two new keywords for the .ctl file are
added, LOGFile and LOGLevel.

* Fixed the error that the wrong incoming segments were
processed if several filenames before the extension were the
same. This bug was introduced with version 3.1.10
Fixed problems with created netmails with strange numbers,
dissapearing netmails etc caused by a buffer overflow.

* Changed the way netmail headers are written, it must now
be machine endian independent.

* The netmail fromname now shows "MakeNL versionnr".

-- Michiel Broek


### V3.2.1 -- August 8, 2004

* It is now possible to send hub or host network segments
compressed. See whatsnew.txt and the example .ctl files how
to use this.

* Added /C to the usage screen.

-- Michiel Broek


### V3.2.0 -- August 1, 2004 -- Version nr change

* Only the version number is changed to allow packaging with
8.3 filenames.

-- Michiel Broek


### V3.1.10 -- August 1, 2004 -- New Features

* 10 ArcOpen commands are allowed in the .ctl file.
New segments received in the uploads or mailfiles directories
may be compressed with any defined ArcOpen compressor.

* Increased the optional parameters for the archiver commands
to five fields for the rar archiver.

* Minor bugfixes

-- Michiel Broek


### V3.1.9 -- July 25, 2004 -- New Features

* Always will write full node address into NetMails with or
without INTL in ctl file.

* NetMail now contains MSGID's.  A tracking file sequence.dat
is stored in the "master" directory.

* Case of file names should all be lower regardless of OS.

-- Michiel Broek


### V3.1.8 -- July 20, 2004 -- New Features

* Added /c commandline (/credits)  Displays a brief credit
paragraph and exits. -- Robert Couture

* The /h parameter now displays a nice, concise help message
that better explains the commandline parameters.

-- Andrew Clarke.

Minor bugfixes.


### V3.1.7 -- July 12, 2004 -- New Feature additions

* Changed version numbering to: Major.Minor.Sub

* Added:

    "&#42;&#42; Dedicated to Ben Baker - The Original Author of MakeNL &#42;&#42;"

    To the splash line when the program runs.

    All  credits for additional work will be added to a credits.txt
    file.  I think  it  is  most appropriate that Ben gets the main
    credit for the original work.

* Behaviour  of  -Unpublished- without Pvt  is  NOT  allowed by
default.  This allows  for  current  drop  in  replacement in
systems  today with  no  changes.  This makes it usable right
away while we decide the outcome of separating the two.

  To change it to allow -Unpublished- w/o Pvt add:

  ALLOwunpub 1  (0 or not in CFG disables)

* I have added  the  ability  to have ALPHA characters in the
Phone field.

  As with ALLOwunpub, it is OFF by default.

  To turn it on add:

  ALPHaphone 1 (0 or not in CFG disables)


### V3.1416 -- 13.07.2004 (BF)

* proper dedication inserted...


### V3.141 -- 11.07.2004 (BF)

* four letters needed to make the ARCxxxx verbs unique...


### V3.14 -- 09.07.2004 (BF)

* defaulted the ARCxxxx verbs to the old ARC ditto
* made some minor adjustments regarding case sensitive
  command lines, now no longer needed
* removed obsolete (c) message from output screen


### V3.13 -- 06.07.2004 (BF)

* fixed the old bug with MakeNl exiting ungracefully
  after some non-fatal (aka ";E "-class) error
* changed baudrate control to only use isdigit
* added ARCcopy/ARCmove/ARCopen verbs to make it
  possible to use any archiver


### V3.12 -- 01.07.2001

* Bug in mkdiff.c fixed (Borland only)
* Debug-mode implemented for directory-checking
* os.c/os.h splitted into several compiler/system-dependant files
* minor changes for source code prettyfying
* Bug in osdosful.c fixed (Borland/DJGPP only): first-level directories work under DOS
* memory-leak in oslnxful.c fixed (Linux only)
* differ works again with 16 bit memory model
* spawning should work now on DOS again
* relative directories work under unix
* added some debug output in ex-buggy parts


### V3.11 -- 09.01.2001

* compiles under EMX-OS/2+DOS and Watcom DOS+OS/2+Win32
* new keywords in makenl.ctl:
  * BatchFile <filename>
  * CalledBatchFile <filename>
    (look in makenl.txt for usage)
* translated history.txt to english
* provided plain ASCII makenl.doc without overstriked characters from original
makenl.prn. To be read with any simple text viewer.
+ changed filenames for Unix:
* MAKENL.CTL default name changed to lowercase
* &#42;.MSG created by MakeNl changed to lowercase
* minor changes for stdout output-prettyfying
* bugfix according to renaming files with generic names
do not rename, if old name ist equal to the new name,
otherwise you may lose files in the master-dir
* internal line length limit now #defined in makenl.h

### V3.10 -- 04.11.2000

* make nodelist line handling more robust against long lines>159 char length
* implemented 32-bit capability
* first Linux version
* compiles under Borland C++ 3.1, Linux-gcc, DOS/Win-gcc (DJGPP) without
  source modifications

### V3.00 -- 24.03.2000 first public release

* bug fixes and optional point support


### V2.52 -- April 16, 1998 -- Y2k patch

* Patched  the  year  written  in  attach  and  receipt  mail.
Removed  the  check for Hi ASCII and "erroneous" baud rates.
Reformatted PRN files into TXT files.

* All filenames were converted to lower case.

* makenl.h: Added MAXLINELENGTH constant. Set to 512

* address.c, appdiff.c, comment.c, config.c, instlst.c, makenl.c,
  merge.c, mkdiff.c, output.c, process.c, variable.c: All hard-coded
  line lengths of 160 were changed to use the MAXLINELENGTH constant.

* config.c: Hard-coded 159 character line length was changed to MAXLINELENGTH-1

* fts5_2.c: Added code to pass through any -Unpublished- phone number without further
checking. Capitalisation of -Unpublished- is standardized.

* lib.c: Changed function definition of CRC16DoByte (line 79) to use shorts instead of ints as ints are 32-bit in DJGPP GCC.

* Removed execute_core() and execute() functions. Replaced by execute() function
in execute.c

* Changed return code checking of findnext() call (line 403) to check for a non-
zero result, instead of -1

* makenl.c: Bumped version number to 2.6. Added warning banner.

* srchmax.c: Removed spurious semicolon from end of line 18

* execute.c: New function used to launch the ARC compressor. Replaces the assembly language routines in the original version with the C library spawn() function.

* makenl.mak: Rewritten for use with DJGPP GCC


-- Björn Felten (2:203/0)


### V2.51 -- June 15, 1992 -- Maintenance release

* To check for erroneous duplicate numbers, MakeNL maintains a
stack of zone numbers, region/net numbers within the current
zone,  and  node  numbers  within  the  current  region/net.
FidoNet Zone  1 was  about to  overflow  this stack.    This
release changes the stack implementation.  It is now limited
only by available memory.

* The entire program was recompiled with MS C V7.0 and several
minor changes were made to eliminate new warnings.


### V2.50 -- February 12, 1991 -- New features

* By default,  the  first  line  of  distribution  lists  from
earlier  versions  contained  "FidoNet  Nodelist   for. . ."
FidoNet is a  registered trademark of  Tom Jennings.   While
Tom hasn't  complained about  this  behaviour, he  has  every
right to, so  now the default  is "  Nodelist for.  . ." (no
name at all).

* Since version 2.00, the /N command line switch would  insert
a name other than "FidoNet" in the first line, and it  still
does.  A new configuration verb, NAMe, has been added to  do
the same.  The difference is that now, if you really want it
to say "FidoNet  Nodelist for. . ." you have  to explicitly
tell MakeNL the name is "FidoNet" by one of these methods.

* I am told that some  mailers require all out-going  messages
to contain the ^AINTL extended address  kludge.  With a  lot
of grimacing and head-shaking,  I have added the  capability
to force  MakeNL  to  do  this.   See  the  SUBmit  verb  in
MAKENL.PRN.


### V2.41 -- January 25, 1991 -- Code bug, documentation bug

* There are only two  ways MakeNL can  know your zone  number.
You explicitly tell it in your NETADDR statement, or, if you
are making a zone list, MakeNL can guess it.  Failing either
of these methods, your zone number is set to zero.   Usually
this doesn't matter, but. . .

  If MakeNL generates a  message to a zone-qualified  address,
and he doesn't know your zone  (i.e. your address is set  to
zone 0), MakeNL failed to notice and both the message header
and the INTL kludge were generated malformed.  This has  now
been fixed.  Under the stated condition, MakeNL will issue a
warning and NOT generate the message.

* Last time I added the PUBlish verb to the code.  This time I
added it to the documentation!


### V2.40 -- December 1, 1990 -- New features.

* Version 7 of ARC uses considerably more memory than  earlier
versions.  MakeNL now  checks free memory,  and if there  is
less that 256K, it swaps itself out of memory before execut-
ing ARC.  If there is an expanded memory driver loaded,  and
enough expanded  memory available,  MakeNL will  use it  for
swap space.  Otherwise  it swaps to  a temporary disk  file.
When swapped out, only about one kilobyte plus your environ-
ment space remain in memory.   This behaviour is  transparent
to users, and requires no action on their part.

* A new  configuration verb,  BAUdrate,  was added  to  permit
redefinition of  MakeNL's baud  rate validity  table.   This
allows for  future growth  without continual  recompilation.
See section  6.1.6  on  page 24  of  the  documentation  for
complete details.

* Added a  second configuration  verb,  PUBlish, to  define  a
publication day other than Friday.


### V2.31 -- August 8, 1990 -- New feature, bug Corrected.

* Added ARC <n> control verb, where <n> may be either 5 or  6.
It is used to force ARC version 6 or 7 to create level-5  or
level-6 compatible archives.  Note that it will cause errors
if <n> is greater than or equal to your ARC version number.

* MakeNL had  another string  length  error.   A  12-character
array was having  a string of  up to  12 characters  scanned
into it.  That doesn't leave  much room for the null  termi-
nator.  The size of the array was increased.


### V2.30 -- June 3, 1990 -- New feature, bug Corrected.

* Added MINPHONE control  verb.   If causes  MakeNL to  reject
phone numbers with  fewer than a  defined minimum number  of
parts. (See documentation.)

* A file entry in the FILES section of a control file may  in-
clude an alternate notification address.  MakeNL was  limit-
ing the length of this address to seven characters -- a  bit
confining.  The string has now  been extended to 17  charac-
ters.  This will permit an alternate address with a  5-digit
zone, a 5-digit net, and a 5-digit node number.

* FTS-0005.TXT was added to the distribution.   It is the  re-
placement document for FSC002-3.

* The documentation was updated to reflect these changes, how-
ever the sample  control files were  not changed to  include
any instances of MINPHONE.

* Recompiled the entire program using MSC 6.00, which resulted
in about 10% reduction in size of .EXE file.


### V2.20 -- 10/06/89 -- Significant Bug Corrected.

* Why this one wasn't reported before now is beyond me!   When
MakeNL received more than one new submittal difference  file
at once, all but the first  would fail to apply because  the
CRC was not  being reinitialized.   RCs  probably don't  get
many difference files, but how could DD go a year and a half
without noticing this problem?

* The program salutation and the documentation title page were
changed to reflect the new release level.

* The obsolete "FSC002-3"  was dropped  from the  distribution
archive.

* No other changes were made.


### V2.12 -- 05/31/88 -- Bug(?) Corrected.

* For some reason (as yet unidentified), memory allocation  in
the difference  analyzer (changed  in V2.11)  behaves  badly
when MakeNL is executed  on an 8088  machine.  This  release
modifies the routine to get  around the problem, and  should
work in the most adverse conditions (I hope).


### V2.11 -- 05/15/88 -- New behaviour.

* The FidoNet nodelist has become larger than earlier versions
could produce difference files  for.  The current  version's
difference analyzer has been modified to segment long  files
and analyze the segments.  After a segment is analyzed,  the
differences are output, then processing proceeds to the next
segment.

* Some time ago, Tom Marshall, IFNA Council, pointed out  that
any copyright contained in the original document should also
be contained in any published difference file.  The  program
at that time was forcing about four lines of the notice into
the difference file.  Since I was modifying the routine that
does it anyway, the difference analyzer was also modified to
force any copyright notice in full into the difference file.

* The changes made do not affect documentation, and it was not
changed.


### V2.10 -- 04/12/88 -- New feature, docs updated, minor bug fixed.

* A new control  file statement, "PRIvate,"  was added to  the
repertoire to  control the  processing of  private nodes  in
networks which discourage or do not permit them.

* The documentation received many  cosmetic changes.  Some  of
the change bars  merely reflect the  movement of figures  to
produce better pagination.  The substantive changes are doc-
umentation of the /NAMe command line switch and the  PRIvate
control file statement.

* The /NAMe command line switch was added in version 2.00, but
seems to have been overlooked in the documentation,  includ-
ing this file.  It provides  the capability of changing  the
network name placed on the first line of a composite list.

* One minor bug was discovered which could cause problems only
with a rare combination of events.  Since I have had no  re-
ports on it, I guess no one ever noticed it, but I fixed  it
anyway!


### V2.03 -- 03/12/88 -- Bug fixed

* A bug was introduced in 2.02 which caused the PVT and  OGATE
keywords to be exchanged on output.   This, of course,  pro-
duces a very confused nodelist!

* IT IS IMPERATIVE that if you ran V2.02 in process mode, that
you upgrade to 2.03 IMMEDIATELY.   Errors generated by  2.02
will "stick" until 2.03 corrects them.  Note that this  cor-
rection should be automatic  and require no intervention  on
your part.

* No other changes were made in  this version.  There were  no
documentation changes.


### V2.02 -- 03/05/88 -- Bug fixed, added feature

* If the first data file is not processed (happens  frequently
in test mode) MakeNL returned an erroneous result code.  The
problem was caused by an uninitialized variable.  You  would
not notice this one until you  tried to use MakeNL's  result
codes to automate weekly processing in a daily event in your
batch file.

* One minor feature  was added.   MakeNL will  now accept  the
keyword "OGATE"  in a  net list.   (OGATE  is invalid  in  a
Region.)  In order to use OGATE, you and all coordinators in
the chain above you must be  using V2.02!  Earlier  versions
of MakeNL will reject the OGATE  keyword.  For this  reason,
the documentation  has  not  been updated  to  reflect  it's
support at this time.

* No other changes were made in  this version.  There were  no
documentation changes.


### V2.01 -- 01/16/88 -- Bug fixed

* Found a bug that's been there  since the beginning of  time.
Wonder why nobody ever reported it???  In the FILES  section
of the .CTL and  explicit file name might  be as many as  12
characters (file name (8) plus "." (1) plus extension  (3)),
but the field width was set to 11!

* The only   other changes made  in this version  is that  the
entire package has been  recompiled with MS  C 5.0.   Please
advise me immediately of any problems encountered!

* Documentation has not  changed, however, the  order of  this
file was reversed (newest version first).


### V2.00 -- 12/28/87 -- Distribution restrictions removed

* Ken Kaplan expressed concern over  the logistics of the  re-
quirement for IFNA  control of the  distribution of  MakeNL.
Therefore, with this  release distribution restrictions  are
removed and MakeNL will be distributed as "shareware" in the
future.  See section 9 in MAKENL.PRN for details.

* New feature:

  This release extends the level of NL maintenance all the way
  to individual nodes.  Note that Coordinators may still fully
  maintain their segments if they wish, but now they may  also
  require each node to maintain its own entry in the list.

  If you wish to make use of this feature, I suggest you  cre-
  ate a file for each node in your segment and mail it to  the
  applicable node with a message  text reading "Enclosed is  a
  file containing  you  nodelist  entry.   If  there  are  any
  changes, edit this  file and  mail it back  to me.   DO  NOT
  CHANGE THE FILE NAME."  Replace each NL entry with an appro-
  priate NODE entry in the FILES section of your control file,
  set up an external event to execute MakeNL daily, and forget
  about 'em.

* Old feature:

  The "indentation" feature of the  files section of the  con-
  trol file  was designed  as "cut-over"  aid, and  should  no
  longer be needed.  MakeNL still supports it, but all  refer-
  ences to it have been removed  from the documentation.   The
  next release will remove it from the program.

* Several minor bugs were found and fixed.

* DOCs with change bars:

  MAKENL.PRN received  more  updates  than  the  program  did!
  Change bars have been added in  the right margin to  reflect
  changes since  V1.21.   Figures 2  through 7  were moved  to
  avoid several nearly  blank pages.   Since  the change  bars
  were added  automatically, this  action caused  several  in-
  stances of  false change  bars.   Oh  well, you  can't  have
  everything.  Nothing that WAS changed escaped being  marked.
  It's just  that  some  marked  passages  were  not  actually
  changed.


### V1.21 -- 11/14/87 -- MERGE clarified

* The requirements for  the MERGE option  were not clear,  and
code did not test that necessary network address information
was present. So:

  1. Added ability for MakeNL to select a default  net-
   work address based on  MAKE command if  NETaddress
   data is missing.

  2. Changed MERGE to  use the first  zone it finds  in
   the  distribution  file  if  no  zone  address  is
   available.

  3. Changed MERGE to  cancel if no  region or net  ad-
   dress is available.  This only happens when MAKing
   a HUB, and no NETaddress command is given.

* Added "change bars" to right margin of documentation.   This
edition reflects changes since V1.10.


### V1.20 -- 11/08/87 -- Bug fixes, one added feature

* Bug fixes:

  1. This wasn't really a bug. The MS library date/time
   handling routines seem  to make automatic  adjust-
   ments for DST (at  the wrong cross-over points,  I
   might add).  According to their docs, setting  the
   environment variable "SET TZ=CST+6" should turn it
   off, but they lie!  They recognize the minus sign,
   but not the plus sign.

   "SET TZ=CST6" will turn  it off, but MakeNL  can't
   rely on that being set right.

   The problem occurs just after the date MS thinks a
   change to or from DST should have happened and you
   run MakeNL within an hour  the wrong side of  mid-
   night.  Day numbers for backward dates get screwed
   up because the time  gets shifted across the  mid-
   night boundary, and MakeNL  can't find its  master
   files!

   Well, it's fixed, and  I'm sorry for any  inconve-
   nience it may have caused.

  2. Fixed a problem in renaming master files two weeks
   old.  (They were being deleted.)

  3. Fixed a  similar  problem renaming  output  master
   files when no changes occur.

  4. Fixed some minor problems surrounding the creation
   of archives and difference files.  If you  specify
   an OUTDIFF parameter,  you should  force the  cre-
   ation of  archives as  well as  difference  files,
   EVEN IF NO CHANGES  OCCUR AND YOU WOULD  OTHERWISE
   GENERATE NEITHER!  Now you do.

* New Feature:

  If you use a generic OUTFILE name, and if you create differ-
  ence and/or archive files, they  tend to accumulate in  your
  OUTPATH directory  over  time.   By default,  MakeNL ignores
  them completely.  This is done so that OUTPATH can point  to
  a "download" directory where several editions might be  made
  available to users.

  I had some complaints about this behaviour.  Some users don't
  appreciate having to "clean up after  MakeNL!"  So, I  added
  the CLEanup command to MakeNL's repertoire in this version.

  When MakeNL operates in process  mode and CLEanup (no  argu-
  ments) appears in the control file, after all other process-
  ing is done, it searches the OUTPATH directory for old  dif-
  ference and/or archive files and deletes any it finds.

  CLEanup has no effect  on current files  or the handling  of
  output master files.  It has no effect whatever when  MakeNL
  is operated in test mode.


### V1.10 -- 08/19/87 -- Bug fixes

* This release is mostly bug fixes, but a new feature has been
added, and behavior has been modified in one respect.

* Bug fixes:

  1. A check was added to insure that all defined  data
directories are distinct.   V1.00  loses files  if
   two are defined for the same directory.

  2. Corrected a bug in the  parsing of the /M  command
   line switch.

  3. Added a  check for  and  removal of  IBM  extended
   characters in text fields.

  4. Corrected file move operation  to account for  the
   use of the DOS JOIN command.

  5. Corrected bugs in  directory (path)  verification.
   In fact  the function  was  rewritten, and  is  (I
   hope) close to bullet proof.

  6. Removed the use of  "DOS COPY" to eliminate  false
   warning messages.

* Added feature:  Added the "NOTIFY SELF" statement to control
file.

* Modified behavior:  ALL output except merge file goes in the
defined OUTPath directory, or if not defined, in the  MASter
directory.  V1.00 places submittal and distribution files in
the current directory.


### V1.00 -- 08/11/87 -- Initial Release
