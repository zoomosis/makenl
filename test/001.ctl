; MakeNL test control file
;
; From: Roy Witt, 1:387/0
; Date: 2012-10-14

logfile 001.log
master .
update ./update
make Network 387
outfile 001.
submit 1:19/0
netaddress 1:387/0
messages ./netmail
outpath ./outfile

data
Host,387,San_Antonio_TX_SATX_Area_Net,San_Antonio_TX,Roy_Witt,000-00-00-00-00,300,CM,MO,LO,INA:lone-star.dyndns.org,IBN
Hub,1,LoneStar_Echomail_Hub,San_Antonio_TX,Roy_Witt,000-00-00-00-00,300,CM,MO,XX,INA:lone-star.dyndns.org,IBN,U,NEC
,6,Lone_Star_Internet_Gate,San_Antonio_TX,Roy_Witt,000-00-00-00-00,300,CM,MO,XX,INA:lone-star.dyndns.org,IBN
PVT,10,CompTA_Radical,San_Antonio_TX,Roy_Witt,-Unpublished-,300,CM,MO,XX,INA:z28krazy.dyndns.org,IBN
,20,Bedrock_BBS,Helotes_TX,Rick_Waller,000-0-0-0-0,300,CM,XA,INA:bbs.kristyandrick.com,ITN,IBN
,22,Flying_\A/_Ranch,Las_Brisas_TX,Roy_Witt,000-00-00-00-00,300,CM,MO,XX,INA:lone-star.dyndns.org,IBN,IEM:autoham22@yahoo.com
Hold,99,Fidonet_Applicant_Test_Node,San_Antonio_TX,New_Dude,-Unpublished-,2400
Hold,999,New_Fidonet_Applicant,San_Antonio_TX,New_Sysop,-Unpublished-,2400,
