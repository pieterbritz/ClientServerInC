# ClientServerInC
 Client and Server Written in C for Virscient

Problem Statement:
==================

This is a “coding challenge”, which is outlined below:

The result can be sent via Github or equivalent, as a Zipped package, or any other suitable means.
 We'd like to timebox this to one week elapsed (though you are welcome to do it quicker if you would like).

Your task is to write a client and server in C for transferring files over a network from the server to the client.
 
1.  The client should connect to the server, specify the file to transfer,
     and receive the file to the local filesystem.
2.  The server needs to support multiple concurrent connections.
3.  The content is not sensitive, there is no need to restrict access or encrypt content.
     The files available to be transferred will all be in a single directory on the server.
4.  The network link may have limited bandwidth and some latency.
5.  The code needs to minimize external dependencies, please use only normal system libraries.

Extra credit:  Files will be transferred repeatedly with only minor changes to the content,
 it is useful to make efficient use of the network when the client has an old copy of the file
 and the copy of the file on the server has only minor modifications.

The submitted code will be peer reviewed and should be representative of your coding style.
 Your submission should also include comments describing your design.

  
 If you can please aim to have this challenge completed by COB 19/09/2022.
 
 Solution:
 =========
 1. Build the server by running "make server" from the project root folder.
    You will find the executable inside the build_server folder.
    Remember to supply the server IP and port, e.g. ./server 127.0.0.1 8080

 2. Build the client by running "make client" from the project root folder.
    You will find the executable inside the build_client folder.
    Remember to supply the server IP and port, e.g. ./server 127.0.0.1 8080

 3. Start the server by pressing "s".