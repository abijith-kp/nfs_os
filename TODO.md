1. How to keep track of global values
    * Name of filesystem
    * Superblock structure
    * Root directory structure  /* can be included in the
                                   max number of open files */
    * For each connected user a global table is needed to keep track of their
      current directory, on the server side. All the operation requested from
      from the client will work with resopect with this saved current directory
    * 

2. Client-Server archetecture:
    * Multithreaded server which can serve to multiple clients
    * Client will be single threaded
    * Client:
        - runs a shell
        - infinite while loop with read-eval-print procedure
        - can read commands sends it to server
        - reads result from server prints to the user
        - client keeps track of its current directory or should the server keep
          track of it for individual clients
        - client connects to server using server-ip and port number
    * Server:
        - each thread of server will read commands from client process it and
          sends back the result
        - if client keeps track of cur directory
            : client will send cur directory along with command and args
        - server starts with a local ip address, port and db file name
        - one server will provide service to only one db
