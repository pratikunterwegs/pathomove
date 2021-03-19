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
                        parameter_file = "which_parameters.csv") {

  # connect to server
  s <- ssh::ssh_connect(ssh_con, passwd = password)

  # transfer parameter file
  ssh::scp_upload(
    s,
    files = parameter_file,
    to = "snevo"
  )

  # modify template job to use correct R script
  job_shell_script <- readLines(template_job)

  # check flexible array creation
  assertthat::assert_that(
    length(grep(pattern = "n_array", x = job_shell_script)) > 0,
    msg = "use_cluster: n_array cannot be flexibly created"
  )

  # replace n array with length of parameter file
  n_array <- length(readLines(parameter_file)) - 1
  job_shell_script = gsub(pattern = "n_array", n_array, job_shell_script)

  # check that last line is Rscript
  assertthat::assert_that(
    grep(pattern = "Rscript", x = job_shell_script) == length(job_shell_script),
    msg = "use_cluster: last line of job script is not Rscript"
  )

  # last line is R script command, replace this
  last_line <- length(job_shell_script)
  job_shell_script[last_line] <- glue::glue(
    "Rscript /scripts/{script} {parameter_file} ${{SLURM_ARRAY_TASK_ID}}"
  )

  # write to full job
  job_name <- glue::glue(
    'job_script_\\
    {gsub(pattern = " |:", replacement = "_", Sys.time())}.sh'
  )
  writeLines(
    job_shell_script,
    con = glue::glue(
      'bash/{job_name}'
    )
  )

  # send job array to cluster
  ssh::scp_upload(
    s,
    glue::glue(
      "bash/{job_name}"
    ),
    to = "snevo/bash"
  )

  # run on cluster
  ssh::ssh_exec_wait(
    s,
    command = c(
      glue::glue("sbatch snevo/bash/{job_name}"),
      glue::glue("rm snevo/bash/{job_name}")
    )
  )
  ssh::ssh_disconnect(s)
}
