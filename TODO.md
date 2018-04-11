1. How to keep track of global values
    * Name of filesystem
    * Superblock structure
    * Root directory structure  /* can be included in the
                                   max number of open files */
    * For each connected user a global table is needed to keep track of their
      current directory, on the server side. All the operation requested from
      from the client will work with resopect with this saved current directory
    * 
