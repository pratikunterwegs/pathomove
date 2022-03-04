#!/bin/bash
#SBATCH --time=05:00:00
#SBATCH --partition=regular
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=pathomove_default
#SBATCH --array=1-n_array
#SBATCH --output=/home/p284074/patho-move-evol/data/output/pathomove_default_%a.log
#SBATCH --mem=8000

pwd
ml load R/4.1.0-foss-2021a
ml load Boost/1.76.0-GCC-10.3.0
cd patho-move-evol
echo "now in" 
pwd
Rscript some rscript here ${SLURM_ARRAY_TASK_ID}
