## cont. System call 

Manual

![image](https://github.com/user-attachments/assets/163a9c3e-cc9d-44e2-9ef5-44e8a06fc879)


## Man 2 intro 

![image](https://github.com/user-attachments/assets/98e947e5-8c0b-403d-855c-329a3073eb50)


open/read/ write --> fundamentals in Unix --> we can find it in version 1



Unix --> system calls --> interfaces of the system 


## How to use system calls 

![image](https://github.com/user-attachments/assets/7bef3458-ed61-4cb8-92f2-5c363fa42efc)


for example in the above screenshot we indirctily used system calls --> c-library contains many function that does various jobs some of the functions are done in the application or userspace no need for the kernel for exampple the string length 

but for some like printf --> used system call ---> formatted string %d replcment --. hgappens in the kerenl not constant 
libc called the kernel or the system call to talk to   the kerenl so its called implicitly


## any process in linux (usually)
1- standard input
2- standard output 
3- standard error


by default libc assumes those files are open --> libc does not open it 
write system call


 

## use system call without libc 

1- ![image](https://github.com/user-attachments/assets/32ac88cb-2b50-4200-85b8-4fc3d117c4cc)


fd--> file descriptor --> ID of the file

write up to the number of count from the buffer 


2- ![image](https://github.com/user-attachments/assets/40040902-15cb-44a5-8dc3-931b8f5194f9)


3- compile
![image](https://github.com/user-attachments/assets/f53b56d5-a1f0-44cf-b191-afe77b3ca563)



# OUCHHHH WRITE IS NOT A SYSTEM CALL!!! 
SYSTEM CALL IS NOT A FUNCTION CALL 
when we call the system to does something we do not call a function but we want to switch the system mode we switch to the privilage mode



![image](https://github.com/user-attachments/assets/df5aa762-8894-43cf-b652-80c3ace949d7)


