Noted: Linux format files is ELF we can not open the executable files from windows on linux as its not supported by it 


### Processes and programs in linux :
programs contains multiple processes and not all programs are working at the same time 


program file : elf file: info about the prgram and how to convert it to process at the runtime 


![image](https://github.com/user-attachments/assets/4696025f-d708-4a85-9408-74d9f7227e23)

----------------------------------------------------------------------------------


Process flow

![image](https://github.com/user-attachments/assets/10a69bc7-b8fc-40d1-8549-85a825bc83d9)


what happend here when we entered ls , the shell took this command and goes to kernel and tell her ya kernel do this command ...

------------------------------------

## Some useful commands

command ps -e : print all excutin proccesses on the system 

tty : number of terminal  


kill : end a proccess --> send a signal to the kernel to end proccesses

![image](https://github.com/user-attachments/assets/5020e66a-1d2b-484b-9431-5dc40c3b049c)




![image](https://github.com/user-attachments/assets/eb0b63e7-8bf6-4365-9ba3-a5f69374afc9)




![image](https://github.com/user-attachments/assets/51c00e7f-d53a-4bf6-a0d7-47d856a8e554)



![image](https://github.com/user-attachments/assets/1ac5e248-a86b-447a-b623-8bdaa6938d28)

--------------------------------------------------------


### PID : Proccess ID

----------------------------------------------------------


Proccess consistes of two main parts :

1- user space : applications running 

code of the programs and their data 


2- kernel : allocate data structure for the proccess --> PCB : Proccess control block 

IDs of the proccesses , memory , time , limits , location of the files and what files , memory tables

## proccess address space:


![image](https://github.com/user-attachments/assets/7970d9f2-f53f-458c-83ce-a87ed73c4819)



-----------------------------------------------

the elf file before and while running it 

![image](https://github.com/user-attachments/assets/689d2390-f476-45dc-af1f-6ee7c9948454)















