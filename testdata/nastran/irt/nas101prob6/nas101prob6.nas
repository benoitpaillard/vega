ID SEMINAR,PROB6
TIME 5
SOL 103
CEND
TITLE = FIXED CIRCULAR PLATE
SUBTITLE = NORMAL MODES
 SPC = 20
 METHOD = 1
 DISP = ALL
BEGIN BULK
CORD2C  100             0.0     0.0     0.0     0.0     0.0     1.0      +000001
++0000011.0     0.0     0.0
GRID    1       100     0.0     0.0     0.0     100
GRID    2       100     .5      0.0     0.0     100
GRID    3       100     1.0     0.0     0.0     100
GRID    4       100     1.5     0.0     0.0     100
GRID    5       100     2.      0.0     0.0     100
GRID    6       100     2.5     0.0     0.0     100
GRID    7       100     3.      0.0     0.0     100
GRID    8       100     3.5     0.0     0.0     100
GRID    9       100     0.5     30.0    0.0     100
GRID    10      100     1.0     30.0    0.0     100
GRID    11      100     1.5     30.0    0.0     100
GRID    12      100     2.      30.0    0.0     100
GRID    13      100     2.5     30.0    0.0     100
GRID    14      100     3.      30.0    0.0     100
GRID    15      100     3.5     30.0    0.0     100
GRID    16      100     0.26    0.97    0.0     100
GRID    17      100     .76     0.97    0.0     100
GRID    18      100     1.26    0.97    0.0     100
GRID    19      100     1.76    0.97    0.0     100
GRID    20      100     2.26    0.97    0.0     100
GRID    21      100     2.76    0.97    0.0     100
GRID    22      100     3.26    0.97    0.0     100
CQUAD4  2       20      2       3       17      16
CQUAD4  3       20      3       4       18      17
CQUAD4  4       20      4       5       19      18
CQUAD4  5       20      5       6       20      19
CQUAD4  6       20      6       7       21      20
CQUAD4  7       20      7       8       22      21
CQUAD4  9       20      16      17      10      9
CQUAD4  10      20      17      18      11      10
CQUAD4  11      20      18      19      12      11
CQUAD4  12      20      19      20      13      12
CQUAD4  13      20      20      21      14      13
CQUAD4  14      20      21      22      15      14
CTRIA3  1       20      1       2       16
CTRIA3  8       20      1       16      9
EIGRL   1                       3
MAT1    30      30.+6           .3      .3
PARAM   COUPMASS1
PARAM   WTMASS  2.59-3
PSHELL  20      30      0.125   30              30
SPC     20      1       12456   0.0
SPC     20      8       123456  0.0
SPC     20      15      123456  0.0
SPC1    20      2       2       THRU    8
SPC1    20      2       9       THRU    15
ENDDATA
