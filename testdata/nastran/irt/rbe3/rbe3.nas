$ FILENAME - RBE3.DAT
ID     LINEAR,RBE3
SOL    101  
TIME   5
CEND
TITLE = SINGLE ELEMENT WITH RBE3
SPC  = 1
LOAD = 1
OLOAD     = ALL
GPFORCE   = ALL
SPCFORCES = ALL
BEGIN BULK
$
RBE3    10              99      3       1.0     123     1       2
        3       4   
FORCE   1       99              100.    0.      0.      1.
$
PARAM   POST    0
$
GRID    1               0.      0.      0.
GRID    2               100.    0.      0.
GRID    3               100.    100.    0. 
GRID    4               0.      100.    0. 
GRID    99              60.     50.     0.
$
PSHELL  1       4       10.     4
$
MAT1    4       4.E6            0.
$
CQUAD4  1       1       1       2       3       4
$
SPC1    1       123456  1       4
ENDDATA
