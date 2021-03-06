$ MeshTriaLin.bdf
ID MeshTriaLin.bdf,NASTRAN
SOL 101
APP   DISP
TIME  10000
CEND
SET 157 = 1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,
56,57,58,59,60 $ Display nodegroup for : ConstraintSet<SPC>{original_id=1}
SET 158 = 3,4 $ DISP
SET 156 = 1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31,
32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,
48,49,50,51,52,53,54,55,
56,57,58,59,60 $ Display nodegroup for : LoadSet<LOAD>{original_id=1}
SET 151 = 1 $ Orientation of coordinate system: 121
SET 153 = 2 $ Orientation of coordinate system: 122
SET 154 = 2 $ PROP_6
SET 152 = 1 $ PROP_8
SET 155 = 61,62,63,64,65,66,67,
68,69,70,71,72,73,74,75,
76,77,78,79,80,81,82,83,
84,85,86,87,88,89,90,91,
92,93,94,95,96,97,98,99,
100,101,102,103,104,105,106,107,
108,109,110,111,112,113,114,115,
116,117,118 $ PROP_9
  STRESS(VONMISES) = 154
SUBCASE 1
  LOAD=1
  SPC=1
  DISP = 158
SUBCASE 2
  LOAD=2
  SPC=1
  DISP = 158
SUBCASE 3
  LOAD=3
  SPC=1
  DISP = 158  
SUBCASE 4
  LOAD=4
  SPC=1
  DISP = 158    
$
TITLE=Vega Exported Model
BEGIN BULK
PARAM,PRGPST,NO
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
GRID           1            3.+2      0.    1.+1
GRID           2            3.+2   -1.+1    1.+1
GRID           3              0.      0.    1.+1
GRID           4              0.   -1.+1    1.+1
GRID           5        9.9984+0   -1.+1    1.+1
GRID           6        1.9997+1   -1.+1    1.+1
GRID           7        2.9995+1   -1.+1    1.+1
GRID           8        4.0498+1   -1.+1    1.+1
GRID           9        5.1451+1   -1.+1    1.+1
GRID          10         6.145+1   -1.+1    1.+1
GRID          11        7.1448+1   -1.+1    1.+1
GRID          12        8.2654+1   -1.+1    1.+1
GRID          13        9.2904+1   -1.+1    1.+1
GRID          14         1.029+2   -1.+1    1.+1
GRID          15         1.129+2   -1.+1    1.+1
GRID          16        1.2301+2   -1.+1    1.+1
GRID          17         1.343+2   -1.+1    1.+1
GRID          18         1.443+2   -1.+1    1.+1
GRID          19         1.543+2   -1.+1    1.+1
GRID          20         1.651+2   -1.+1    1.+1
GRID          21         1.757+2   -1.+1    1.+1
GRID          22         1.857+2   -1.+1    1.+1
GRID          23        1.9569+2   -1.+1    1.+1
GRID          24        2.0569+2   -1.+1    1.+1
GRID          25        2.1715+2   -1.+1    1.+1
GRID          26        2.2715+2   -1.+1    1.+1
GRID          27        2.3715+2   -1.+1    1.+1
GRID          28        2.4762+2   -1.+1    1.+1
GRID          29        2.5855+2   -1.+1    1.+1
GRID          30        2.6855+2   -1.+1    1.+1
GRID          31        2.7854+2   -1.+1    1.+1
GRID          32        2.8854+2   -1.+1    1.+1
GRID          33        9.8977+0      0.    1.+1
GRID          34        1.9795+1      0.    1.+1
GRID          35        2.9693+1      0.    1.+1
GRID          36        4.0003+1      0.    1.+1
GRID          37        5.0948+1      0.    1.+1
GRID          38        6.0845+1      0.    1.+1
GRID          39        7.0743+1      0.    1.+1
GRID          40        8.2685+1      0.    1.+1
GRID          41        9.2997+1      0.    1.+1
GRID          42        1.0289+2      0.    1.+1
GRID          43        1.1279+2      0.    1.+1
GRID          44        1.2282+2      0.    1.+1
GRID          45        1.3495+2      0.    1.+1
GRID          46        1.4485+2      0.    1.+1
GRID          47        1.5475+2      0.    1.+1
GRID          48        1.6553+2      0.    1.+1
GRID          49        1.7595+2      0.    1.+1
GRID          50        1.8584+2      0.    1.+1
GRID          51        1.9574+2      0.    1.+1
GRID          52        2.0564+2      0.    1.+1
GRID          53          2.17+2      0.    1.+1
GRID          54        2.2689+2      0.    1.+1
GRID          55        2.3679+2      0.    1.+1
GRID          56        2.4742+2      0.    1.+1
GRID          57        2.5895+2      0.    1.+1
GRID          58        2.6885+2      0.    1.+1
GRID          59        2.7875+2      0.    1.+1
GRID          60        2.8864+2      0.    1.+1
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
CTRIA3        61       9       4      33       3        
CTRIA3        62       9       1      60       2        
CTRIA3        63       9       5      33       4        
CTRIA3        64       9       6      34       5        
CTRIA3        65       9       7      35       6        
CTRIA3        66       9       8      36       7        
CTRIA3        67       9       9      37       8        
CTRIA3        68       9      10      38       9        
CTRIA3        69       9      11      39      10        
CTRIA3        70       9      12      40      11        
CTRIA3        71       9      13      40      12        
CTRIA3        72       9      14      41      13        
CTRIA3        73       9      15      43      14        
CTRIA3        74       9      16      44      15        
CTRIA3        75       9      17      44      16        
CTRIA3        76       9      18      45      17        
CTRIA3        77       9      19      46      18        
CTRIA3        78       9      20      47      19        
CTRIA3        79       9      21      48      20        
CTRIA3        80       9      22      49      21        
CTRIA3        81       9      23      50      22        
CTRIA3        82       9      51      23      52        
CTRIA3        83       9      25      53      24        
CTRIA3        84       9      26      54      25        
CTRIA3        85       9      27      55      26        
CTRIA3        86       9      28      56      27        
CTRIA3        87       9      29      56      28        
CTRIA3        88       9      30      57      29        
CTRIA3        89       9      31      58      30        
CTRIA3        90       9      32      59      31        
CTRIA3        91       9       2      60      32        
CTRIA3        92       9      33       5      34        
CTRIA3        93       9      34       6      35        
CTRIA3        94       9      35       7      36        
CTRIA3        95       9      36       8      37        
CTRIA3        96       9      37       9      38        
CTRIA3        97       9      38      10      39        
CTRIA3        98       9      39      11      40        
CTRIA3        99       9      40      13      41        
CTRIA3       100       9      43      15      44        
CTRIA3       101       9      44      17      45        
CTRIA3       102       9      45      18      46        
CTRIA3       103       9      46      19      47        
CTRIA3       104       9      47      20      48        
CTRIA3       105       9      48      21      49        
CTRIA3       106       9      49      22      50        
CTRIA3       107       9      50      23      51        
CTRIA3       108       9      53      25      54        
CTRIA3       109       9      54      26      55        
CTRIA3       110       9      55      27      56        
CTRIA3       111       9      56      29      57        
CTRIA3       112       9      57      30      58        
CTRIA3       113       9      58      31      59        
CTRIA3       114       9      59      32      60        
CTRIA3       115       9      14      42      41        
CTRIA3       116       9      24      52      23        
CTRIA3       117       9      14      43      42        
CTRIA3       118       9      24      53      52        
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
MAT1           1    2.+57.6923+4    3.-1      0.
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
PSHELL         9       1    1.+1       1
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
SPC1           1  123456       1
SPC1           1  123456       2
SPC1           1       4       1
SPC1           1       4       2
SPC1           1       4       3
SPC1           1       4       4
SPC1           1       4       5
SPC1           1       4       6
SPC1           1       4       7
SPC1           1       4       8
SPC1           1       4       9
SPC1           1       4      10
SPC1           1       4      11
SPC1           1       4      12
SPC1           1       4      13
SPC1           1       4      14
SPC1           1       4      15
SPC1           1       4      16
SPC1           1       4      17
SPC1           1       4      18
SPC1           1       4      19
SPC1           1       4      20
SPC1           1       4      21
SPC1           1       4      22
SPC1           1       4      23
SPC1           1       4      24
SPC1           1       4      25
SPC1           1       4      26
SPC1           1       4      27
SPC1           1       4      28
SPC1           1       4      29
SPC1           1       4      30
SPC1           1       4      31
SPC1           1       4      32
SPC1           1       4      33
SPC1           1       4      34
SPC1           1       4      35
SPC1           1       4      36
SPC1           1       4      37
SPC1           1       4      38
SPC1           1       4      39
SPC1           1       4      40
SPC1           1       4      41
SPC1           1       4      42
SPC1           1       4      43
SPC1           1       4      44
SPC1           1       4      45
SPC1           1       4      46
SPC1           1       4      47
SPC1           1       4      48
SPC1           1       4      49
SPC1           1       4      50
SPC1           1       4      51
SPC1           1       4      52
SPC1           1       4      53
SPC1           1       4      54
SPC1           1       4      55
SPC1           1       4      56
SPC1           1       4      57
SPC1           1       4      58
SPC1           1       4      59
SPC1           1       4      60
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
PLOAD2         1    5.-1      61
PLOAD2         1    5.-1      62
PLOAD2         1    5.-1      63
PLOAD2         1    5.-1      64
PLOAD2         1    5.-1      65
PLOAD2         1    5.-1      66
PLOAD2         1    5.-1      67
PLOAD2         1    5.-1      68
PLOAD2         1    5.-1      69
PLOAD2         1    5.-1      70
PLOAD2         1    5.-1      71
PLOAD2         1    5.-1      72
PLOAD2         1    5.-1      73
PLOAD2         1    5.-1      74
PLOAD2         1    5.-1      75
PLOAD2         1    5.-1      76
PLOAD2         1    5.-1      77
PLOAD2         1    5.-1      78
PLOAD2         1    5.-1      79
PLOAD2         1    5.-1      80
PLOAD2         1    5.-1      81
PLOAD2         1    5.-1      82
PLOAD2         1    5.-1      83
PLOAD2         1    5.-1      84
PLOAD2         1    5.-1      85
PLOAD2         1    5.-1      86
PLOAD2         1    5.-1      87
PLOAD2         1    5.-1      88
PLOAD2         1    5.-1      89
PLOAD2         1    5.-1      90
PLOAD2         1    5.-1      91
PLOAD2         1    5.-1      92
PLOAD2         1    5.-1      93
PLOAD2         1    5.-1      94
PLOAD2         1    5.-1      95
PLOAD2         1    5.-1      96
PLOAD2         1    5.-1      97
PLOAD2         1    5.-1      98
PLOAD2         1    5.-1      99
PLOAD2         1    5.-1     100
PLOAD2         1    5.-1     101
PLOAD2         1    5.-1     102
PLOAD2         1    5.-1     103
PLOAD2         1    5.-1     104
PLOAD2         1    5.-1     105
PLOAD2         1    5.-1     106
PLOAD2         1    5.-1     107
PLOAD2         1    5.-1     108
PLOAD2         1    5.-1     109
PLOAD2         1    5.-1     110
PLOAD2         1    5.-1     111
PLOAD2         1    5.-1     112
PLOAD2         1    5.-1     113
PLOAD2         1    5.-1     114
PLOAD2         1    5.-1     115
PLOAD2         1    5.-1     116
PLOAD2         1    5.-1     117
PLOAD2         1    5.-1     118
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
PLOAD4         2      61    5.-1                            THRU     118
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
FORCE          3       1          .375+3     0.0     0.0     1.0
FORCE          3       2          .375+3     0.0     0.0     1.0
FORCE          3       3          .375+3     0.0     0.0     1.0
FORCE          3       4          .375+3     0.0     0.0     1.0
$---1--][---2--][---3--][---4--][---5--][---6--][---7--][---8--][---9--][--10--]
PLOAD          4    5.-1       1       3       4
PLOAD          4    5.-1       4       2       1
ENDDATA
