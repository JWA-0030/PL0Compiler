In this folder you will find the source files for the Code Generation
program done by Chandler Douglas & Jesse Alsing. 

To compile the program, please drop all 4 files in a common directory
on Eustis or whatever other program of your choice using Filezilla.

Once all the files are moved, issue the following command: 

gcc main.c parser.c scanner.c vm.c

This will create an a.out file that can be run using the following
command:

./a.out 

Note 1: The flags that were required to be implemented do work, use
-a -l -v to print out the corresponding output.

p.s. we couldnt get our jpc to work correctly, we didnt figure out where to 
jump to if the condition was met