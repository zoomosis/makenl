; This is a sample control for a the coordinator of a Zone.
; Edit fields within angle brackets as necessary (removing the brackets)

make zone <nbr>         ; No "master data file"  input data is below.

outfile <explicit name>   ; use generic name for output file if very large

process    Thursday       ; Set this day to meet deadline ITC gives you or
                          ; Friday after cut-over to zone based nodelist

arc z zip -o              ; Use InfoZip instead of the default Arc. Comment
                          ; out the ARC keyword to use Arc by default.  

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
