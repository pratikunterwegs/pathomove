#!/bin/bash
#SBATCH --time=00:30:00
#SBATCH --partition=regular
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=snevo_simulation
#SBATCH --array=1-n_array
#SBATCH --mem=500

pwd
ml load R/4.0.0-foss-2020a
cd snevo
echo "now in" 
pwd
Rscript some rscript here ${SLURM_ARRAY_TASK_ID}
