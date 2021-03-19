#' Run snevo and the cluster.
#'
#' @param ssh_con SSH connection.
#' @param password Password.
#' @param script Which R script to run.
#' @param template_job Which shell script to use as a template.
#' @param parameter_file Where to get parameters.
#'
#' @return Runs a job which makes more jobs.
#' @export
#'
use_cluster <- function(
  ssh_con = "some_server",
  password = "your_password",
  script = "which_script.R",
  template_job = "some_template.sh",
  parameter_file = "which_parameters.csv"
) {
  
  # connect to server
  s <- ssh::ssh_connect(ssh_con, passwd = password)

  # check for folder and if not clone
  check_prepare_cluster(
    ssh_con = ssh_con,
    password = password
  )
  
  # modify template job to use correct R script
  job_shell_script = readLines(template_job)
  
  # check that last line is Rscript
  assertthat::assert_that(
    grep(pattern = "Rscript", x = job_shell_script) == length(job_shell_script),
    msg = "use_cluster: last line of job script is not Rscript"
  )
  
  # last line is R script command, replace this
  last_line = length(job_shell_script)
  job_shell_script[last_line] = glue::glue(
    'Rscript --slave -e {script} {parameter_file} ${{SLURM_ARRAY_TASK_ID}}'
  )
  
  # write to full job
  job_name = glue::glue('bash/job_script_\\
                        {gsub(pattern = " |:", replacement = "_", Sys.time())}.sh')
  writeLines(
    job_shell_script,
    con = job_name
  )
  
  # send job array to cluster
  ssh::scp_upload(
    s, 
    job_name, 
    to = "snevo"
  )
  
  # run on cluster
  ssh::ssh_exec_wait(
    s,
    command = glue::glue("sbatch {job_name}")
  )
  ssh::ssh_disconnect(s)
  
}
