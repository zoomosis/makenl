# MakeNL NODELIST Generator Program

## Support

* Web:      <http://makenl.sourceforge.net/>
* FidoNet:  The echomail area MAKENL_NG, available to most systems
* Trackers: Besides discussions in the echo, Bugs and Feature Requests
can be registered and tracked at:

  * <https://sourceforge.net/p/makenl/bugs/>
  * <https://sourceforge.net/p/makenl/feature-requests/>


## Important Changes since MakeNL 2.5x:

### FILENAMES

If you use a case sensitive OS (Unix, Linux etc), then change
all output filenames to lowercase. MakeNL writes the filenames
to disk in lowercase on these OS's. On other OS's this doesn't
matter.


## NETADDRESS

To allow better zone aware netmails, the netaddress must now
include the zone number, 28/0 should now become 2:28/0


## ARC

The ARC keyword no longer works as documented in the old MakeNL
documentation and is obsolete. MakeNL 2.5x only had support for
Arc compression, but 3.1.8 now supports multiple compression
utilities, through configurable archiver entries following the
ARCOpen and ARCMove keywords. These new keywords need between
three and five parameters. To provide compatibility with MakeNL
2.5x, the ARC keyword must be commented out of the MAKENL.CTL
file, which will cause MakeNL to default to Arc compression as
before.

Currently, ARCCopy and ARCMove work for this feature. The difference
is that ARCCopy creates an archive but leaves the original file,
ARCMove creates and archive and removes the original file.
Since version 3.2.1 MakeNL can now send hub and host networks segments
compressed to. Add the following lines to makenl.ctl to use it:

    threshold   0 -1
    arccopy     z zip -jo      ; Infozip
    arcmove     z zip -jmo

If you use the arccopy keyword you must also use the arcmove keyword
so that both the nodelist and nodediff will use the same archiver
such as zip. If you onmit one of these, MakeNL refuses to start.
If you don't use both keywords, the default arc archiver will be
used.

The new syntax of the ARCXxxxx keywords are as follows:

    ARCXxxx [x] [archiver exe name] [archiver command] [command switches]

The [x] tells MakeNL what the first character of the final archive
of the stub will be. For zip, you would list z, resulting in *.znn
output files.

Here are some examples of commonly used archivers, and the commands
that work with them:

    PKZip:   ARCCopy z pkzip -ex (or: ARC z pkzip -a)
    InfoZip: ARCCopy z zip -jo
    Rar:     ARCCopy r rar -a -ep
    Arj:     ARCCopy j arj -a -e
    Lha:     ARCCopy l lha -a -m1

To allow received segment files in different formats maximum 10
ARCOpen commands may be defined in the MAKENL.CTL file. If in
MAKENL.CTL in the FILES section a file is defined as hub.50 then
in the mailfiles and uploads directories a zipped segment with the
name hub.z0 or hub.zip will be unpacked and the unpacked file will
be processed. The syntax form the ARCOpen command is the same as
for ARCCopy, but 10 lines are allowed and 10 different archive
formats are recognized. The following archives are recognized:

    Pkzip or Infozip, Arc, Lha, Zoo, Arj, Rar, Ha, Bzip2, Gzip and
    Unix compress.

The following are examples of entries in MAKENL.CTL:

    Arc:      ARCOpen a arc ew
    Infozip:  ARCOpen z unzip -oj
    Lha:      ARCOpen l lha ef
    Rar:      ARCOpen r rar e -o+ -y
    Gzip:     ARCOpen g tar xfz
    Bzip2:    ARCOpen b tar xfj
    Compress: ARCOpen c tar xfz
    Arj:      ARCOpen j arj -y e
    Zoo:      ARCOpen o zoo eq:0
    Ha:       ARCOpen h ha ey


### ALLOW8bit

ALLOW8bit is a new keyword that will cause MakeNL to allow char-
acters above 0x7F in nodelist entries.  Normally, such characters
are replaced with question marks (?) when the file is processed.
This keyword is disabled by default.  To enable it, add ALLOW8bit 1
to your MAKENL.CTL file.


### ALLOWUnpub

ALLOWUnpub is a new keyword that will cause MakeNL to allow the
-Unpublished- string to be present in the sixth (phone number)
field of node listings, without requiring the Pvt keyword to be
present in the listing. This keyword is disabled by default. To
enable it, add ALLOWUnpub 1 to your MAKENL.CTL file.


### ALPHAphone

ALPHAphone is a new keyword that will cause MakeNL to allow alpha
characters to be present in the sixth (phone number) field of node
listings. This keyword is disabled by default. To enable it, add
ALPHAphone 1 to your MAKENL.CTL file.


### LOGFile

The full path and filename to the logfile to use. It is best to
place this keyword as first keyword in the .ctl file, logging
starts as soon as this keyword is seen.

If not defined then nothing is logged. This is the old behaviour.

### LOGLevel

The level of logging, must be between 1 and 4. Level 1 is the
default. The meaning of all levels is:

    1 - Normal and error messages
    2 - Verbose messages
    3 - Debug messages
    4 - Severe debug messages


### FORcesubmit

FORcesubmit is a new keyword that will cause MakeNL to submit
unchanged segments. Normally unchanged segments are not submitted.
This keyword will only work if MakeNL is started with -P on the
commandline. This keyword is disabled by default.

To enable it add FORcesubmit 1 to your MAKENL.CTL file.


### REMoveBOM

REMoveBOM is a new keyword that will cause MakeNL to remove any UTF-8
Byte Order Marks (BOMs) found in incoming nodelist segments.  This
keyword is disabled by default.  To enable it add REMoveBOM 1 to your
MAKENL.CTL file.

---
