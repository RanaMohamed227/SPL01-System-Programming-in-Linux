ELF : EXcutable and link format --. to satndarize the format of the output files from the compilers 


ELF format: the way data is organizaed in the file 

![image](https://github.com/user-attachments/assets/7c9d507b-9c80-4c71-ae41-ed38f5d82a2e)

ELF Header: type of the file --> elf file --> no.of sections 
data section --> global variables
bss section --> unintialized global variables & static variables
symtab --> symbols
debug --> optional




------------------------------------------
1- Read ELF header

Magic number : identification to the files 

Entry point: start address


![image](https://github.com/user-attachments/assets/05cb0f7c-7bef-400b-bddf-524aa797dbf0)



2- Read elf sections 

![image](https://github.com/user-attachments/assets/48bb136a-b28d-4cab-9a0c-d023456c760a)





![image](https://github.com/user-attachments/assets/cf4491b2-8555-4d59-a22d-9d74835baad9)



3- Read symbols

![image](https://github.com/user-attachments/assets/37701429-cb45-4348-b939-3d23578537aa)




----------------------------------------

1- static compilation 

Include all the required and needed files in the final output eXecutable file --> static linking
![image](https://github.com/user-attachments/assets/b83e0b4f-70d1-4009-b6a1-71afb00f231b)






Now its defined 

![image](https://github.com/user-attachments/assets/5fa9ffe2-fdcd-4cf9-bc78-9c22c7897bc0)






-----------------------------------


2- Dyamic linking : the default on linuX

What is dynamic linking : the os runs tons of programs so instead of each program has its own files that implemt each function so we will have many many redandant files so we make a shared library that all function can use instead of making tons of files for each function 

--> shared library here is --> libc 




-----------------------------



### Statically linked files is larger than the dynamically linked files 


![image](https://github.com/user-attachments/assets/d4a9ce52-9cb2-4f97-9259-e9359f1aa28f)



--------------


### Deassembly


![image](https://github.com/user-attachments/assets/e6063e0d-d86c-41fe-b643-ed0957022322)






