; $Id: net-l.ctl,v 1.4 2004/09/21 18:57:18 mbroek Exp $
;
; This is a sample control for a the coordinator of a large Net, whose
; Hub coordinators maintain their own segments.
; Edit fields within angle brackets as necessary (removing the brackets).

make network <nbr>        ; No "master data file"  input data is below.

outfile <explicit name>   ; use generic name if output file is very large

process    Wednesday      ; Set this day to meet deadline RC gives you

master     <directory>    ; where master files reside (default - current)
update     <directory>    ; where to save received files 'till processing
mailfiles  <directory>    ; where mail server places received files
uploads    <directory>    ; where BBS places uploaded files (if needed)
badfiles   <directory>    ; optional "waste can" for files with fatal errors

threshold 0 -1            ; send submitted file compressed (new)
arccopy z zip -jo         ; infozip
arcmove z zip -jmo

arcopen a arc ew          ; accept arced segments
arcopen z unzip -oj       ; accept zipped segments, unpack with infozip

notify  receipt    ; notify both receipt and errors CRASH and/or HOLD optional
submit     <RC's address> ; where you send updates, CRASH and/or HOLD optional
netaddress <your address> ; your network address here
messages   <directory>    ; path name to mail server's network mail area

; No other options are needed.  

data

; Your source data goes here.  

files

Node <nbr> <explicit file name> ; node files and hub files may be intermixed
Node <nbr> <explicit file name> ; if some of a Hub's nodes send you updates

Hub <nbr> <file name>     ; either generic or explicit file name, but must
Hub <nbr> <file name>     ; match convention used by Hub.
Hub <nbr> <file name> <notification address> ; if not "net/hub"
Hub <nbr> <file name>
;      etc.
