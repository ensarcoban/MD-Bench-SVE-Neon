#!/bin/bash -l

cd /home/hpc/muco/muco117h/MD-Bench-SVE-Neon/grace
echo "ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce"  > logs/neon_double_stabilizer.log
#!/bin/bash
particle_size=1000

for i in {10..20..1}; do
    # size=$(echo "scale=10; e(l($particle_size)/3)" | bc -l)
    # size=$(echo "($size+0.999)/1" | bc)  # This rounds up to the nearest whole number
    # size_cube=$((size * size * size))
    echo "$i"
    RESULT=$(likwid-pin -q -c S1:0-3@S2:0-3@S4:0-3@S5:0-3 ./bins/MDBench-CP-CLANG-ARM-NEON-SP   -nx $i -ny $i -nz $i )
    echo 16, $i, $i, $i, $RESULT>> logs/neon_double_stabilizer.log
done