; This is a sample control for a the coordinator of a Hub or small Net.
; Edit fields within angle brackets as necessary (removing the brackets).
; This variation uses a "master data file" for input and outputs to the
; same file name.  See NET-S.CTL for another approach.

make hub <100> <hub.100>  ; note explicit name of file simplifies processing

outfile        <hub.100>  ; master file and output file have SAME name

submit   <host's address> ; where you send updates, CRASH and/or HOLD optional
netaddress <your address> ; your network address here
messages   <directory>    ; path name to mail server's network mail area

; No other options are needed.  Master directory and output directory will
; default to the current directory.
