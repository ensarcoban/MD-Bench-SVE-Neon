#!/bin/bash
header="ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce, Nthreads"
size=50

echo $header > logs/strong/neon_double_strong.log
echo $header > logs/strong/neon_single_strong.log
echo $header > logs/strong/sve_double_strong.log
echo $header > logs/strong/sve_single_strong.log
echo $header > logs/strong/reference_double_strong.log

for i in {0..3}; do
    RESULT=$(likwid-pin -q -c S1:0-$i ./bins/MDBench-CP-CLANG-ARM-NEONT-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $i, $size, $size, $size, $RESULT>>logs/strong/neon_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $i, $size, $size, $size, $RESULT>>logs/strong/neon_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-$i ./bins/MDBench-CP-CLANG-ARM-SVET-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $i, $size, $size, $size, $RESULT>>logs/strong/sve_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-$i ./bins/MDBench-CP-CLANG-ARM-SVE-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $i, $size, $size, $size, $RESULT>>logs/strong/sve_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-DP-REF   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $i, $size, $size, $size, $RESULT>>logs/strong/reference_double_strong.log
done

for i in {0..3}; do
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-$i ./bins/MDBench-CP-CLANG-ARM-NEONT-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+4), $size, $size, $size, $RESULT>>logs/strong/neon_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+4), $size, $size, $size, $RESULT>>logs/strong/neon_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-$i ./bins/MDBench-CP-CLANG-ARM-SVET-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+4), $size, $size, $size, $RESULT>>logs/strong/sve_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-$i ./bins/MDBench-CP-CLANG-ARM-SVE-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+4), $size, $size, $size, $RESULT>>logs/strong/sve_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-DP-REF   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+4), $size, $size, $size, $RESULT>>logs/strong/reference_double_strong.log
done


for i in {0..3}; do
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-$i ./bins/MDBench-CP-CLANG-ARM-NEONT-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+8), $size, $size, $size, $RESULT>>logs/strong/neon_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+8), $size, $size, $size, $RESULT>>logs/strong/neon_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-$i ./bins/MDBench-CP-CLANG-ARM-SVET-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+8), $size, $size, $size, $RESULT>>logs/strong/sve_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-$i ./bins/MDBench-CP-CLANG-ARM-SVE-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+8), $size, $size, $size, $RESULT>>logs/strong/sve_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-DP-REF   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+8), $size, $size, $size, $RESULT>>logs/strong/reference_double_strong.log
done


for i in {0..3}; do
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-3@S5:0-$i ./bins/MDBench-CP-CLANG-ARM-NEONT-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+12), $size, $size, $size, $RESULT>>logs/strong/neon_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-3@S5:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+12), $size, $size, $size, $RESULT>>logs/strong/neon_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-3@S5:0-$i ./bins/MDBench-CP-CLANG-ARM-SVET-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+12), $size, $size, $size, $RESULT>>logs/strong/sve_double_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-3@S5:0-$i ./bins/MDBench-CP-CLANG-ARM-SVE-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+12), $size, $size, $size, $RESULT>>logs/strong/sve_single_strong.log
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-3@S5:0-$i ./bins/MDBench-CP-CLANG-ARM-NEON-DP-REF   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $($i+12), $size, $size, $size, $RESULT>>logs/strong/reference_double_strong.log
done