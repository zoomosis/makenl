; $Id: distrib.ctl,v 1.4 2007-04-09 18:20:27 fido Exp $
;
; This is a sample control for a coordinator who creates distribution files.
; On process day, MakeNL is run a second time using this control file.
; Edit fields within angle brackets as necessary (removing the brackets)

logfile <file name>     ; Logfile to use
loglevel <0..4>         ; Level of logging, default 0
make composite
name <network name>   	; eg. FidoNet or AlterNet or EGGnet
threshold  0 -1		; force compression -- no submittal difference file
;arc 5			; obsolete, see whatsnew.txt
arccopy z pkzip -ex	; compress with pkzip instead of default arc
arcmove z pkzip -mf	; compress with pkzip instead of default arc
arcopen a arc ew	; accept compressed arced segments
arcopen z pkunzip -eo	; accept compressed zipped segments
outfile <generic name>	; recommended names:
			;       NODELIST - complete composite list
			;       Zn-LIST  - zone distribution list
			;       Rnn-LIST - region distribution list
			;       NnnnLIST - network distribution list
outdiff <generic name>	; recommended names:
			;       NODEDIFF - complete composite list
			;       Zn-DIFF  - zone distribution list
			;       Rnn-DIFF - region distribution list
			;       NnnnDIFF - network distribution list
master     <directory>	; same as OUTPath or MASter in previous run
outpath    .		; put <name>.nnn in current directory
copyright <cpy-dist.txt>; if these three files are not path-qualified
prolog    <file name>	; they are located in the master directory
epilog    <file name>
files
Network <nbr> <explicit name>	; must match OUTFile name of previous run
;   or
Region  <nbr> <generic name>	; must match OUTFile name of previous run
;   or
Zone    <nbr> <generic name>	; must match OUTFile name of previous run
Zone    <nbr> <generic name>	; file(s) received from other ZC(s)

