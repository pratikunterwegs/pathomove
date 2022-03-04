#' Prepare the cluster for pathomove.
#'
#' @param ssh_con Cluster connection.
#' @param password Password.
#'
#' @return None, should check and prepare cluster.
#' @export
#'
check_prepare_cluster <- function(ssh_con = "some_server",
                                  password = "your_password") {
  message("checking cluster for pathomove")
  # connect to server
  s <- ssh::ssh_connect(ssh_con, passwd = password)

  # prepare check and fail case clone
  cluster_check <- glue::glue(
    'if [[ -d "pathomove" ]]; then
        echo "pathomove exists, updating";
        cd pathomove;
        git branch
        git checkout -- .;
        rm data/output/*.log;
        rm install_log.log;
        git remote update;
        if [[ ! `git status --porcelain` ]]; then
            git pull;
            chmod +x bash/install_pathomove.sh;
            ./bash/install_pathomove.sh;
        fi
    else
       echo "pathomove does not exist, cloning";
       git clone https://github.com/pratikunterwegs/pathomove.git pathomove;
       cd pathomove
       chmod +x bash/install_pathomove.sh;
       ./bash/install_pathomove.sh;
       cd ..
    fi
    git checkout -- .'
  )
  # check for folder pathomove
  ssh::ssh_exec_wait(s, command = cluster_check)

  ssh::ssh_disconnect(s)

  message("cluster prepared for pathomove")
}
