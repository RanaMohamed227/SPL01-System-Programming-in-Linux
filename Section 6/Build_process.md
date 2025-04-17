1- mkdir build_process
2- nano hello.c
3- write a simple c program 
![image](https://github.com/user-attachments/assets/32957660-e1bc-4282-a621-66f3f9a279f0)


4- build the file using gcc compiler

![image](https://github.com/user-attachments/assets/5cccebe2-0749-41f6-97d0-15f38bf2a310)


5- run the file 

![image](https://github.com/user-attachments/assets/95d5835b-6ed1-4610-bb6b-215b62ed5f6a)



- i can use any other compiler but at the end we will have a program and we want the same format for any compiler so we want a standard to be able to run the program without any trouble i can run any code using different compilers and still be able to run the program the same way 

- so we need standard or specification for the interference of the compiler and running of the progrom
### first edition was a.out the output of the file but after that we got the most popular the elf file .


---------------------------------------------------


## ELF file 
### Build process

![image](https://github.com/user-attachments/assets/42f280cb-e6a2-400b-a091-c0204bb8031b)




1- preprocessor --> macros and libraries replacment 

2- compilation --> syntaX checking 

3- assembly --> change it to assembly code --> can have optmization steps --> architecture dependant 
assembler then takes the assembly file to machine code 
4- object code files --> machine code --> zeros & ones 
the linker takes all object files and libraries -c library - and connect all of them and manages the memeory allocation --> linker script describes the format of the output files 


-------------------------------------------



### Lets check it

1- we define the macros 
![image](https://github.com/user-attachments/assets/e6552200-7775-4e8d-b3c9-810c976fd9a2)


gcc is not the compiler its the fornetend the calls the compilers by it self

2- preprocessor

![image](https://github.com/user-attachments/assets/08d2d74a-d5ad-416b-b153-aed38373e350)



### Replacment 
![image](https://github.com/user-attachments/assets/53fe181c-da56-45d3-9ca2-a71fc1fe06c6)

3- Assembly
![image](https://github.com/user-attachments/assets/e04ae040-3a50-4e77-bed4-ff3c5f75d534)


4- object code 

![image](https://github.com/user-attachments/assets/027061cd-5fb4-45ab-a46d-f7bc0448dcea)




types of files:
![image](https://github.com/user-attachments/assets/514e61c2-ce06-4631-ace1-88f29f415478)



Relocatable : can be relocated --> the addresses needs to be resolved --> the linker does it 


5- final outputfile 

![image](https://github.com/user-attachments/assets/65572a8c-50c8-43bd-a2f8-fef3e431b841)


------------------------------------------



## Doing that all in one step 

1- remove previous files

![image](https://github.com/user-attachments/assets/563be987-f9fa-4a22-adc8-075cb8d83e9a)


![image](https://github.com/user-attachments/assets/7059bc20-3483-42dc-a82a-9157cce61b1a)




2- compile with saving temp filrs



![image](https://github.com/user-attachments/assets/81028070-42c8-4ab5-beb5-4773dcf098db)



## if we did not use libc we will have error in the output file

![image](https://github.com/user-attachments/assets/7fce07d5-4b15-4bc7-ab8c-ab1bdf94bdaa)



- the creation of elf file


![image](https://github.com/user-attachments/assets/400ccc1c-7161-4530-b34e-2ec257b2a8ad)







