This is an implemented virtual machine for running PM/0 by Chandler Douglas and Jesse Alsing.

Inorder to run the code you MUST extract the zip folder first then you can run it

To extract the folder right click on the zip folder and click extract

to run the file in a Linux enviroment you must:
a) download vm.c  and vminput.txt from the zip file.
b) type gcc vm.c to make it an executable
c) type ./a.out to run the executable

to test the resulting vmoutput.txt file:
a) after compiling run cat vmoutput.txt to display the contents of the output file to the screen.
b) then run diff correctoutput.txt and  vmoutput.txt to visually find any differences.
	i. Because of the broad assignment details, differences in format are inevitable.
