# Benchmarks
Geekbench 5: https://browser.geekbench.com/v5/cpu/17873912
## blake512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|    38.75   |    40.19   |   103.71   |        21.31       |       2.83       |
+------+------------+------------+------------+--------------------+------------------+
| synth|   280.83   |   462.61   |   164.73   |        12.5        |       1.85       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   500.61   |   1107.22  |   221.17   |        19.7        |       1.48       |
+------+------------+------------+------------+--------------------+------------------+
| total|   820.19   |   1610.02  |    196.3   |        17.82       |       1.62       |
+-------------------------------------------------------------------------------------+
```
## bmw512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|    35.89   |    37.4    |   104.21   |        22.18       |       2.66       |
+------+------------+------------+------------+--------------------+------------------+
| synth|   379.88   |   639.06   |   168.22   |        13.32       |       1.76       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   1332.91  |   2780.02  |   208.57   |        23.29       |       1.46       |
+------+------------+------------+------------+--------------------+------------------+
| total|   1748.69  |   3456.48  |   197.66   |        21.04       |       1.53       |
+-------------------------------------------------------------------------------------+
```
## cubehash512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|    96.83   |    98.47   |   101.69   |        19.8        |       3.09       |
+------+------------+------------+------------+--------------------+------------------+
| synth|    554.3   |   744.14   |   134.25   |        11.52       |       1.56       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   1784.21  |   4558.16  |   255.47   |        21.05       |       1.29       |
+------+------------+------------+------------+--------------------+------------------+
| total|   2435.33  |   5400.78  |   221.77   |        19.78       |       1.36       |
+-------------------------------------------------------------------------------------+
```
## echo512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|   385.52   |   386.82   |   100.34   |        22.28       |       3.25       |
+------+------------+------------+------------+--------------------+------------------+
| synth|   661.62   |   815.34   |   123.23   |        15.66       |       1.65       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   683.08   |   1373.49  |   201.07   |        17.3        |       1.42       |
+------+------------+------------+------------+--------------------+------------------+
| total|   1730.22  |   2575.65  |   148.86   |        17.37       |       1.78       |
+-------------------------------------------------------------------------------------+
```
## groestl512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|   796.13   |    797.3   |   100.15   |        22.09       |       3.15       |
+------+------------+------------+------------+--------------------+------------------+
| synth|   377.44   |    730.7   |    193.6   |        13.87       |       2.11       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   1064.38  |   2274.71  |   213.71   |        19.79       |       1.33       |
+------+------------+------------+------------+--------------------+------------------+
| total|   2237.95  |   3802.71  |   169.92   |        19.31       |       1.87       |
+-------------------------------------------------------------------------------------+
```
## hamsi512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|   119.91   |   121.38   |   101.23   |        27.79       |       2.46       |
+------+------------+------------+------------+--------------------+------------------+
| synth|   358.75   |   528.89   |   147.43   |        12.81       |       1.69       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   784.24   |   1640.19  |   209.14   |        21.09       |       1.29       |
+------+------------+------------+------------+--------------------+------------------+
| total|   1262.89  |   2290.45  |   181.37   |        19.73       |       1.45       |
+-------------------------------------------------------------------------------------+
```
## jh512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|   1921.53  |   1922.03  |   100.03   |        34.42       |       2.41       |
+------+------------+------------+------------+--------------------+------------------+
| synth|    299.3   |   488.32   |   163.15   |        15.09       |       1.75       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   486.97   |   936.36   |   192.29   |        18.07       |       1.42       |
+------+------------+------------+------------+--------------------+------------------+
| total|   2707.8   |   3346.71  |   123.59   |        28.44       |       2.04       |
+-------------------------------------------------------------------------------------+
```
## keccak512_f450
```
+-------------------------------------------------------------------------------------+
| Stage|Run Time (s)|CPU Time (s)|CPU Load (%)|LLC Cache Misses (%)|IPC (instrs/clock)|
+------+------------+------------+------------+--------------------+------------------+
|csynth|    52.86   |    54.46   |   103.03   |        21.37       |       2.93       |
+------+------------+------------+------------+--------------------+------------------+
| synth|   288.81   |   596.79   |   206.64   |        23.46       |       1.77       |
+------+------------+------------+------------+--------------------+------------------+
| impl |   494.44   |   1123.05  |   227.13   |        21.42       |        1.5       |
+------+------------+------------+------------+--------------------+------------------+
| total|   836.11   |   1774.31  |   212.21   |        22.1        |       1.64       |
+-------------------------------------------------------------------------------------+
```
