; This is a sample control for a the coordinator of a Hub or small Net.
; Edit fields within angle brackets as necessary (removing the brackets).

make network <nbr>        ; No "master data file"  input data is below.

outfile <explicit name>   ; output file 

submit     <RC's address> ; where you send updates, CRASH and/or HOLD optional
netaddress <your address> ; your network address here
messages   <directory>    ; path name to mail server's FidoNet mail area

; No other options are needed.  Master directory and output directory will
; default to the current directory.

data

; Your source data goes here.  The advantage of this over "HUB.CTL" is that
; MAKENL will not reformat or change your source data, even if you have errors.
