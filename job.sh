#!/usr/bin/env bash
#SBATCH --job-name=thomas-mijieux
#SBATCH --output=job_test.out
#SBATCH --error=job_test.err
#SBATCH -p mistral
#SBATCH --time=00:20:00
#SBATCH --exclusive
#SBATCH --nodes=3 --ntasks-per-node=10

module load intel/mkl/64/11.2/2016.0.0
module load compiler/gcc/5.1.0
module load slurm/14.11.11
module load hardware/hwloc/1.11.0
module load mpi/openmpi/gcc/1.10.1-tm

cd /home/prcd2016-mijieux/gravmpi

exec_task() {
    mpiexec -n 30 --output-filename output/output ./star input/autogen1_$1.data
    pushd output 
#    ./meta-gplot.py 20 $1 | gnuplot
#    cp output.png output_$1.png

    popd
}

exec_task 507
#exec_task 531
#exec_task 539
