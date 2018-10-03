# pg2

Abigail Shirey & Rita Shultz | ashirey & rshultz
==
CSE 30264: Computer Networks
3 October 2018
Programming Assignment 2 README
==

The following is the list of all of the included files for this project:
- in pg2 directory:
  - in server directory:
    - server.c
    - Makefile
    - tempdirectory (this is a directory)
    - SmallFile.txt
  - in client directory:
    - client.c
    - Makefile
  - fake.txt

The project can be compiled and executed by utilizing the following instructions:

The server implementation and the client implementation are held in two separate
directories, named "server" and "client". Each directory has its own Makefile.
To make the executable for the server and client, simply go into each directory
and type "make". The executable for the server is called "myftpd", and the
executable for the client is called "myftp". When actually running and testing
this code, the server and client should be run on two different student
machines. For example, the client executable can run on student00, while the
server executable runs on student02. They should be executed initially in the
following way:
- server (on student02):
  - $ ./myftpd 41044
  - (Here, the port number is specified as the second command line argument.)
- client (on student00):
  - $ ./myftp student02.cse.nd.edu 41044
  - (Here, the host name and port number are specified as the second and third
    command line arguments.)


The following is a list of example commands to run this code from the client-
side in the archive:
- LS
  - This will print a list of the directory of the server in the client's
  terminal window.
  - Specific example: $ LS
- DL <FILENAME>
  - This will download a file from the server's directory into the client's
  directory.
  - Specific example: $ DL MediumFile.pdf
- UP <FILENAME>
  - This will upload a file from the client's directory into the server's
  directory.
  - Specific example: $ UP client.c
- RM <FILENAME>
  - This will remove a file from the server's directory, as specified by the
  file name in the argument.
  - Specific example: $ RM fake.txt
- MKDIR <DIRNAME>
  - This will create a new directory in the server's directory.
  - Specific example: $ MKDIR testdir
- RMDIR <DIRNAME>
  - This will remove a directory from the server's directory.
  - Specific example: $ RMDIR tempdirectory
- CD <PATH/DIRNAME>
  - This will cause the server to operate out of the newly changed directory,
  as specified by the path in the argument.
  - Specific example: $ CD ../client
- EXIT
  - This causes the client to close its connection with the server, and end the
  executable's process.
  - Specific example: $ EXIT


Any questions regarding the functionality of this code repository can be
directed to Abigail Shirey (ashirey) or Rita Shultz (rshultz).
