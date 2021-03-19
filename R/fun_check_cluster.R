#' Prepare the cluster for snevo.
#'
#' @param ssh_con Cluster connection.
#' @param password Password.
#'
#' @return None, should check and prepare cluster.
#' @export
#'
check_prepare_cluster <- function(
                                  ssh_con = "some_server",
                                  password = "your_password") {
  # connect to server
  s <- ssh::ssh_connect(ssh_con, passwd = password)

  # prepare check and fail case clone
  cluster_check <- glue::glue(
   'if [[ -d "snevo" ]];
        then
        echo "snevo exists, updating"
        cd snevo
        git remote update
        if git status --porcelain;
          then
          git pull
          chmod +x bash/install_snevo.sh
          ./bash/install_snevo.sh
        fi
    else 
       echo "snevo does not exist, cloning"
       git clone https://github.com/pratikunterwegs/snevo.git snevo
       chmod +x bash/install_snevo.sh
       ./bash/install_snevo.sh 
    fi
    git checkout -- bash/install_snevo.sh'
  )
  # check for folder snevo
  ssh::ssh_exec_wait(s, command = cluster_check)

  ssh::ssh_disconnect(s)

  message("cluster prepared")
}
