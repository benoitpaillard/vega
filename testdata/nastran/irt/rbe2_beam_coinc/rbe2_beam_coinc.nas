SOL 101
APP   DISP
TIME  10000
SET 1 = 2
AUTOSPC = NO
SUBCASE 1
  LOAD=1
  SPC=1
  DISP=1
BEGIN BULK
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
GRID           1              0.   -5.+0    1.+1
GRID           2            3.+2   -5.+0    1.+1
GRID           3            1.+1   -5.+0    1.+1
GRID           4            2.+1   -5.+0    1.+1
GRID           5            3.+1   -5.+0    1.+1
GRID           6            4.+1   -5.+0    1.+1
GRID           7            5.+1   -5.+0    1.+1
GRID           8            6.+1   -5.+0    1.+1
GRID           9            7.+1   -5.+0    1.+1
GRID          10            8.+1   -5.+0    1.+1
GRID          11            9.+1   -5.+0    1.+1
GRID          12            1.+2   -5.+0    1.+1
GRID          13           1.1+2   -5.+0    1.+1
GRID          14           1.2+2   -5.+0    1.+1
GRID          15           1.3+2   -5.+0    1.+1
GRID          16           1.4+2   -5.+0    1.+1
GRID          17           1.5+2   -5.+0    1.+1
GRID          18           1.6+2   -5.+0    1.+1
GRID          19           1.7+2   -5.+0    1.+1
GRID          20           1.8+2   -5.+0    1.+1
GRID          21           1.9+2   -5.+0    1.+1
GRID          22            2.+2   -5.+0    1.+1
GRID          23           2.1+2   -5.+0    1.+1
GRID          24           2.2+2   -5.+0    1.+1
GRID          25           2.3+2   -5.+0    1.+1
GRID          26           2.4+2   -5.+0    1.+1
GRID          27           2.5+2   -5.+0    1.+1
GRID          28           2.6+2   -5.+0    1.+1
GRID          29           2.7+2   -5.+0    1.+1
GRID          30           2.8+2   -5.+0    1.+1
GRID          31           2.9+2   -5.+0    1.+1
GRID          32           1.3+2   -5.+0    1.+1
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
CBEAM          1       9       1       3      0.      0.    1.+0
CBEAM          2       9       3       4      0.      0.    1.+0
CBEAM          3       9       4       5      0.      0.    1.+0
CBEAM          4       9       5       6      0.      0.    1.+0
CBEAM          5       9       6       7      0.      0.    1.+0
CBEAM          6       9       7       8      0.      0.    1.+0
CBEAM          7       9       8       9      0.      0.    1.+0
CBEAM          8       9       9      10      0.      0.    1.+0
CBEAM          9       9      10      11      0.      0.    1.+0
CBEAM         10       9      11      12      0.      0.    1.+0
CBEAM         11       9      12      13      0.      0.    1.+0
CBEAM         12       9      13      14      0.      0.    1.+0
CBEAM         13       9      14      15      0.      0.    1.+0
CBEAM         14       9      32      16      0.      0.    1.+0
CBEAM         15       9      16      17      0.      0.    1.+0
CBEAM         16       9      17      18      0.      0.    1.+0
CBEAM         17       9      18      19      0.      0.    1.+0
CBEAM         18       9      19      20      0.      0.    1.+0
CBEAM         19       9      20      21      0.      0.    1.+0
CBEAM         20       9      21      22      0.      0.    1.+0
CBEAM         21       9      22      23      0.      0.    1.+0
CBEAM         22       9      23      24      0.      0.    1.+0
CBEAM         23       9      24      25      0.      0.    1.+0
CBEAM         24       9      25      26      0.      0.    1.+0
CBEAM         25       9      26      27      0.      0.    1.+0
CBEAM         26       9      27      28      0.      0.    1.+0
CBEAM         27       9      28      29      0.      0.    1.+0
CBEAM         28       9      29      30      0.      0.    1.+0
CBEAM         29       9      30      31      0.      0.    1.+0
CBEAM         30       9      31       2      0.      0.    1.+0
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
RBE2    10000000      15  123456      32
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
MAT1           1    2.+57.6923+4    3.-1      0.
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
PBEAM          9       1    2.+26.6667+31.6667+3      0.3.7867+4
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
SPC1           1  123456       1
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
FORCE          1       4       0    1.+0      0.      0.   -1.+2
ENDDATA
