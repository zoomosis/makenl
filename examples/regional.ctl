; $Id: regional.ctl,v 1.1 2009/01/08 20:07:46 mbroek Exp $
;
; This is a sample control for a the coordinator of a Region.
; Edit fields within angle brackets as necessary (removing the brackets)

make region <nbr>         ; No "master data file"  input data is below.
outfile <generic name>    ; use generic name for output file 
process    Thursday       ; Set this day to meet deadline ZC gives you

;arc 5		          ; obsolete, see whatsnew.txt
                          ; don't define arccopy, arcmove, arcopen, this will
                          ; let makenl use the build-in defaults:
                          ; arc a, arc m, arc ew

master     <directory>    ; where master files reside (default - current)
update     <directory>    ; where to save received files 'till processing
mailfiles  <directory>    ; where mail server places received files
uploads    <directory>    ; where BBS places uploaded files (if needed)
badfiles   <directory>    ; optional "waste can" for files with fatal errors

notify  errors CRASH      ; notify of errors (CRASH optional) and/or
notify  receipt           ; use either of these or both with different 
                          ; attributes.

submit     <ZC's address> ; where you send updates, CRASH and/or HOLD optional
netaddress <your address> ; your network address here
messages   <directory>    ; path name to mail server's network mail area

data

; Your region source data goes here.  

files

Node <nbr> <explicit file name> ; node files must precede any network files
Node <nbr> <explicit file name> ; (unless it is your intent to move an
                                ;  independant node into someone's net on

                                ;  on the sly)

Net <nbr> <file name>     ; either generic or explicit file name, but must
Net <nbr> <file name>     ; match convention used by Net.
Net <nbr> <file name> <notification address> ; if not "net/0"
Net <nbr> <file name>
;      etc.

