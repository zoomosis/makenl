; $Id: zone.ctl,v 1.1 2009/01/08 20:07:46 mbroek Exp $
;
; This is a sample control for a the coordinator of a Zone.
; Edit fields within angle brackets as necessary (removing the brackets)

make zone <nbr>         ; No "master data file"  input data is below.

outfile <explicit name>   ; use generic name for output file if very large

process    Thursday       ; Set this day to meet deadline ITC gives you or
                          ; Friday after cut-over to zone based nodelist

;arc 5		          ; obsolete, see whatsnew.txt
                          ; don't define arccopy, arcmove, arcopen, this will
                          ; let makenl use the build-in defaults:
                          ; arc a, arc m, arc ew

ARCOpen a arc ew          ; several un-compress commands for incoming segments
ARCOpen z unzip -oj
ARCOpen l lha ef
ARCOpen r rar e -o+ -y
ARCOpen g tar xfz
ARCOpen b tar xfj
ARCOpen c tar xfz
ARCOpen j arj -y e
ARCOpen o zoo eq:0
ARCOpen h ha ey

master     <directory>    ; where master files reside (default - current)
update     <directory>    ; where to save received files 'till processing
mailfiles  <directory>    ; where mail server places received files
uploads    <directory>    ; where BBS places uploaded files (if needed)
badfiles   <directory>    ; optional "waste can" for files with fatal errors

notify  errors CRASH      ; notify of errors (CRASH optional). Errors should
                          ; not occur at this level, but. . .

submit     1:1/0          ; where you send updates, CRASH and/or HOLD optional
netaddress <your address> ; your network address here
messages   <directory>    ; path name to mail server's FidoNet mail area

data

; Your zone source data goes here.  Note that first non-comment statement
; must begin with the keyword "Zone" not "Region"

files

Region <nbr> <generic name>     
Region <nbr> <generic name>     
Region <nbr> <generic name> <notification address> ; if not "region/0"
Region <nbr> <generic name>
;      etc.

