/**
 * argv[1] -> server ip
 * argv[2] -> port number
 *
 * 1. Start and bind socket to interface
 * 2. Run infinite loop
 * 3. When a new client connection is made start a new thread to process the
 *    client
 * 4. Print log message from the server to the terminal
 * 5. Process can be like:
 *      Read command from client
 *      Evaluate the command
 *      Send the result back
 *
 **/
