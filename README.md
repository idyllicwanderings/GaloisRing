# Galois Ring

### Preliminary Libraries
Sagemath 10.3

Install the following packages for sagemath:
```bash
sage --pip install (--user) tqdm
```

### Run
To build the project, use
```bash
```

### Test
Our test structure looks like:

```bash
├── TestVectors
│   ├── GRToweringAddition.txt
│   ├── GRToweringInverse.txt
│   ├── GRToweringMultiplication.txt
│   └── GRToweringSubtraction.txt
└── UnitTests
    ├── GaloisRing
    ├── GaloisRingTowering
    │   ├── genGRTowering.sage
    │   ├── grmodtables.sage
    │   ├── testGRTowering.cc
    │   └── testGRTowering.h
    ├── UnitTest.cc
    ├── UnitTest.h
```



To run the tests, first run the corresponding bash script, then use 

```bash
cmake -B build 
cmake --build build
./build/test_target 
```
for example, to run the tests for towers of Galois ring, first run `lib/genreduction.sage` and then run `tests/UnitTests/GaloisRingTowering/genGRTowering.sage`, the modify the `k`, `d` and `GRT_HAS_K_TOWERS` in `tests/UnitTests/GaloisRingTowering/testGRTowering.h`, the run below

To have $55$ testcases of $GR(GR(GR(7,2),3),2)$, use
 ```bash
cd lib/
sage genreduction.sage
cd ../tests/UnitTests/GaloisRingTowering
sage genGRTowering.sage 55 7 2 3 2 mult add sub inv
 ```
then mofidy the corresponding `k, d0, d1, d2` and `define GRT_HAS_3_TOWERS` in `tests/UnitTests/GaloisRingTowering/testGRTowering.h`, then run 
```bash
cmake -B build 
cmake --build build
./build/grtowertest
```
### Performance
To benchmark the performance of arithemetic operations in Galois ring, run
```bash
cmake -B build 
cmake --build build
./build/grbenchmark
```