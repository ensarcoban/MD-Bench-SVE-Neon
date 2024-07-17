#!/bin/bash -l

x_init=16
y_init=16
z_init=16

echo "Initial size: x=$x_init | y=$y_init |z=$z_init"
cd /home/hpc/muco/muco117h/MD-Bench-SVE-Neon/grace
echo "ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce"  > logs/neon_single_weak.log
echo "ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce"  > logs/neon_double_weak.log
echo "ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce"  > logs/sve_single_weak.log
echo "ProcessNumber, x, y, z, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce"  > logs/sve_double_weak.log

for ((j=1; j<=144; j+=1)); do 
    # Initial problem size
    x=$x_init
    y=$y_init
    z=$z_init
    count=0
    n_proc=$j
    declare -a factor_array

    i=2
    for ((i;i<=$n_proc;)); do
        if [ `expr $n_proc % $i` -eq 0 ];then
            factor=$i
            factor_array[count]=$factor
            count=`expr $count + 1`
            n_proc=$((n_proc / factor))
        else 
            i=`expr $i + 1`
        fi
    done
    echo ${factor_array[@]}

    direction=1
    index=$((count-1))
    for ((index;index>=0;)); do
        if [ $direction -eq 1 ]; then
            x=$((x*${factor_array[index]}))
            direction=2
        elif [ $direction -eq 2 ]; then
            y=$((y*${factor_array[index]}))
            direction=3
        elif [ $direction -eq 3 ]; then
            z=$((z*${factor_array[index]}))
            direction=1
        else
            echo "FATAL ERROR"
        fi
        index=`expr $index - 1`
    done
    RESULT=$(srun --cpu-bind=none likwid-pin -q -c N:0-$(($j-1)) ./no_likwid/neon_single_clang_grace  -nx $x -ny $y -nz $z -freq --freq= 3.4)
    echo $(($(($j-1)) + 1)), $x, $y, $z, $RESULT>> logs/neon_single_weak.log
    RESULT=$(srun --cpu-bind=none likwid-pin -q -c N:0-$(($j-1)) ./no_likwid/neon_single_clang_grace  -nx $x -ny $y -nz $z -freq --freq= 3.4)
    echo $(($(($j-1)) + 1)), $x, $y, $z, $RESULT>> logs/neon_double_weak.log
    RESULT=$(srun --cpu-bind=none likwid-pin -q -c N:0-$(($j-1)) ./no_likwid/sve_single_clang_grace   -nx $x -ny $y -nz $z -freq --freq= 3.4)
    echo $(($(($j-1)) + 1)), $x, $y, $z, $RESULT>> logs/sve_single_weak.log
    RESULT=$(srun --cpu-bind=none likwid-pin -q -c N:0-$(($j-1)) ./no_likwid/sve_double_clang_grace   -nx $x -ny $y -nz $z -freq --freq= 3.4)
    echo $(($(($j-1)) + 1)), $x, $y, $z, $RESULT>> logs/sve_double_weak.log
done
