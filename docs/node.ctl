; $Id: node.ctl,v 1.2 2004/08/02 20:16:26 mbroek Exp $
;
; Sample control file for an individual node

make node <76>            ; No "master data file"  input data
                          ; is below.
outfile <node.076>        ; output file

submit     <100/512>      ; where you send updates, CRASH
                          ; and/or HOLD optional
netaddress <1:100/76>     ; your network address here
messages   <directory>    ; path name to mail server's network
                          ; mail area

; No other options are needed.  Master directory and output
; directory will default to the current directory.

data

; Your source data goes here.
,76, Baker's Arce, So. Roxana IL,. . . etc.
