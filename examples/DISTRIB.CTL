; This is a sample control for a coordinator who creates distribution files.
; On process day, MakeNL is run a second time using this control file.
; Edit fields within angle brackets as necessary (removing the brackets)

make composite
name <network name>   	; eg. FidoNet or AlterNet or EGGnet
threshold  0 -1		; force ARC -- no submittal difference file

arc z zip -o            ; Use InfoZip instead of the default Arc. Comment
                        ; out the ARC keyword to use Arc by default.  

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
