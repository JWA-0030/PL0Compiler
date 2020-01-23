Project: scanner.c
Date Completed: 13 October 2017
Completed by: Chandler Douglas & Jesse Alsing

This program is a scanner to tokenize a program written in PL/0 
and catch errors in an event that the program is not written 
syntactically correct. The output file for this program contains
the source program repeated, as well as a symbolic version of the
lexeme list and a tokenized version of the lexeme list.

In order to compile the program and run it, first connect to Eustis
or another linux server, and make sure the file (scanner.c located in
Source of the Submission folder) as well as the correct input 
(scannerin.txt) are located in the proper directory. 

Once all of the files are put in the proper directory, go to Eustis 
and issue the following command: 

gcc scanner.c

This will compile the source c file into an executable a.out file.
To run the a.out file simply issue the following command: 

./a.out

After running the program (hopefully no errors occur) a file named
scannerout.txt can be found in your directory. To compare with the 
proper output issue the command:

diff scannerout.txt correctout.txt

This will print any difference to the screen, nothing otherwise.

To print the output file to the screen issue the command: 

cat scannerout.txt