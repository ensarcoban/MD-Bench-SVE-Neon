#!/bin/bash --login                   
#SBATCH --nodes=1                  
#SBATCH --time=02:00:00
#SBATCH -w gracesup1            
#SBATCH --job-name=job123 
#SBATCH --export=NONE              
                                   
# unset SLURM_EXPORT_ENV           

# module load <modules>            

cd /home/hpc/muco/muco117h/MD-Bench-SVE-Neon/grace
echo ProcessNumber, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce  > logs/neon_single_strong.log
echo ProcessNumber, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce  > logs/neon_double_strong.log
echo ProcessNumber, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce  > logs/sve_single_strong.log
echo ProcessNumber, Total, Force, Neighbor, Rest, PerformanceTotal, PerformanceForce  > logs/sve_double_strong.log

for process_number in {0..71..1}; do
    RESULT= $(srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/neon_single_clang_grace  -nx 200 -ny 100 -nz 100 -freq --freq= 3.4 )
    echo $process_number, $RESULT>> logs/neon_single_strong.log
done

for process_number in {0..71..1}; do
    RESULT= $(srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/neon_single_clang_grace  -nx 200 -ny 100 -nz 100 -freq --freq= 3.4 )
    echo $process_number, $RESULT>> logs/neon_double_strong.log
done

for process_number in {0..71..1}; do
    RESULT= $(srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/sve_single_clang_grace  -nx 200 -ny 100 -nz 100 -freq --freq= 3.4 )
    echo $process_number, $RESULT>> logs/sve_single_strong.log
done

for process_number in {0..71..1}; do
    RESULT= $(srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/sve_double_clang_grace  -nx 200 -ny 100 -nz 100 -freq --freq= 3.4 )
    echo $process_number, $RESULT>> logs/sve_double_strong.log
done
