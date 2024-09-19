#!/bin/bash -l

cd /home/hpc/muco/muco117h/MD-Bench-SVE-Neon/grace
echo "ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce"  > logs/neon_double_stabilizer_numa.log
#!/bin/bash
particle_size=10000

for i in {1..20}; do
    size=$(echo "scale=10; e(l($particle_size)/3)" | bc -l)
    size=$(echo "($size+0.999)/1" | bc)  # This rounds up to the nearest whole number
    size_cube=$((size * size * size))
    particle_size=$(echo "$particle_size * 1.5" | bc)
    RESULT=$(likwid-pin -q -c M0:0-71 ./bins/MDBench-CP-ARMCLANG-ARM-NEONT-DP   -nx $size -ny $size -nz $size -freq --freq= 3.4)
    echo 2, $size, $size, $size, $RESULT>> logs/neon_double_stabilizer_numa.log

done