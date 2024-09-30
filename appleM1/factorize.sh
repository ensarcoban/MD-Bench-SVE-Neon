#!/bin/bash
particle_size=1000

for i in {1..14}; do
    size=$(echo "scale=10; e(l($particle_size)/3)" | bc -l)
    size=$(echo "($size+0.999)/1" | bc)  # This rounds up to the nearest whole number
    size_cube=$((size * size * size))
    echo "$size $size_cube"
    particle_size=$(echo "$particle_size * 1.5" | bc)
done