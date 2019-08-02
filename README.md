# Assembler
A program in C++ that assembles a SCRAM program from an assembler level description.  
The extended SCRAM instruction set is decoded: 

![Extended SCRAM Instruction Set](https://github.com/Kunalmighty/Simulator/blob/master/instructionSet.png)

Input format:
-------------
The input consists of lines with the following format:
label: opcode argument ; comment
The label is any string starting with a letter (a-z, A-Z) and composed of letters, numbers, and underscores. 
The label is optional, but if present, must be followed by a colon.
The opcode is any of the ones listed, or an assembler directive. It is optional.

The argument is only present on those opcodes that need it. 
It can be either a number (decimal, octal or hexadecimal.
The comment is also optional, but if present, must be preceded by a semicolon.
Note that empty lines, as well as lines with only a label or only a comment, or other such combinations are allowed. 
Also, there can be any number of spaces or tab characters between the label, opcode, argument and comment.

Output format:
--------------
The output is a binary file, which does not have a particular format. 
