#!/bin/bash
header="ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce, Nthreads"
size=58

# echo $header > logs/strong/neon_double_strong.log
# echo $header > logs/strong/neon_single_strong.log
# echo $header > logs/strong/sve_double_strong.log
# echo $header > logs/strong/sve_single_strong.log
echo $header > logs/strong/reference_double_strong.log

for i in {0..143}; do
    # RESULT=$(likwid-pin -q -c N:0-$i ./bins/MDBench-CP-ARMCLANG-ARM-NEONT-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    # echo $i, $size, $size, $size, $RESULT>>logs/strong/neon_double_strong.log
    # RESULT=$(likwid-pin -q -c N:0-$i ./bins/MDBench-CP-ARMCLANG-ARM-NEON-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    # echo $i, $size, $size, $size, $RESULT>>logs/strong/neon_single_strong.log
    # RESULT=$(likwid-pin -q -c N:0-$i ./bins/MDBench-CP-ARMCLANG-ARM-SVET-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    # echo $i, $size, $size, $size, $RESULT>>logs/strong/sve_double_strong.log
    # RESULT=$(likwid-pin -q -c N:0-$i ./bins/MDBench-CP-ARMCLANG-ARM-SVE-SP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    # echo $i, $size, $size, $size, $RESULT>>logs/strong/sve_single_strong.log
    RESULT=$(likwid-pin -q -c N:0-$i ./bins/MDBench-CP-ARMCLANG-ARM-NEON-DP-REF   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo $i, $size, $size, $size, $RESULT>>logs/strong/reference_double_strong.log
done

