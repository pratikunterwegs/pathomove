#! /bin/bash
#SBATCH --job-name=run_infomove
#SBATCH --time=00:30:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=300
#SBATCH --partition=regular

pwd
pwd
module load R/3.6.1-foss-2018a
ml list
