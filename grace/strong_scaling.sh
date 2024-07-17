#!/bin/bash --login                   
#SBATCH --nodes=1                  
#SBATCH --time=02:00:00
#SBATCH -w gracesup1            
#SBATCH --job-name=job123 
#SBATCH --export=NONE              
                                   
# unset SLURM_EXPORT_ENV           

# module load <modules>            

cd /home/hpc/muco/muco117h/MD-Bench-SVE-Neon/grace
echo "BEGIN LOG" > logs/neon_single_strong.log
echo "BEGIN LOG" > logs/neon_double_strong.log
echo "BEGIN LOG" > logs/sve_single_strong.log
echo "BEGIN LOG" > logs/sve_double_strong.log

for process_number in {0..71..2}; do
    echo PROCESS NUMBER $process_number >> logs/neon_single_strong.log
    srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/neon_single_grace_clang  -nx 40 -ny 32 -nz 16 -freq --freq= 3.4 >> logs/neon_single_strong.log
done

for process_number in {0..71..2}; do
    echo PROCESS NUMBER $process_number >> logs/neon_double_strong.log
    srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/neon_double_grace_clang  -nx 40 -ny 32 -nz 16 -freq --freq= 3.4 >> logs/neon_double_strong.log
done

for process_number in {0..71..2}; do
    echo PROCESS NUMBER $process_number >> logs/sve_single_strong.log
    srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/grace_single_grace_clang -nx 40 -ny 32 -nz 16 -freq --freq= 3.4 >> logs/sve_single_strong.log
done

for process_number in {0..71..2}; do
    echo PROCESS NUMBER $process_number >> logs/sve_double_strong.log
    srun --cpu-bind=none likwid-pin -q -c N:0-$process_number ./no_likwid/grace_double_grace_clang -nx 40 -ny 32 -nz 16 -freq --freq= 3.4 >> logs/sve_double_strong.log
done
