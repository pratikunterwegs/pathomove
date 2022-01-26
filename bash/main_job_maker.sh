#!/bin/bash
#SBATCH --time=00:10:00
#SBATCH --partition=regular
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=snevo_simulation
#SBATCH --array=1-n_array
#SBATCH --output=/home/p284074/snevo/data/output/snevo_output_%a.log
#SBATCH --mem=5000

pwd
ml load R/4.1.0-foss-2021a
ml load Boost/1.76.0-GCC-10.3.0
cd snevo
echo "now in" 
pwd
Rscript some rscript here ${SLURM_ARRAY_TASK_ID}
