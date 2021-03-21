#!/bin/bash
#SBATCH --time=00:30:00
#SBATCH --partition=regular
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=snevo_simulation
#SBATCH --array=1-n_array
#SBATCH --mem=500

pwd
module load R/3.6.1-foss-2018a
cd snevo
echo "now in" 
pwd
Rscript some rscript here ${SLURM_ARRAY_TASK_ID}
