#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
//#define COUNT 100

int pwd_main() {
    // Write your code here
    // Do not write a main() function. Instead, deal with pwd_main() as the main function of your program.
 char cwd[PATH_MAX]; // in the manual we can use the macro path max but i used count dos not matter
    //  from the manual we check if any arguments was passed by checkin on 1
  
    // if we are at no where
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Ohh no i am at a black hole can not know where");
        exit(-2);
    }


    // Print the current working directory
    printf("%s\n", cwd);

    return 0;

}









/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#define COUNT 100

int pwd_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with pwd_main() as the main function of your program.
 char cwd[COUNT]; // in the manual we can use the macro path max but i used count dos not matter
    //  from the manual we check if any arguments was passed by checkin on 1
    if (argc != 1) {
        printf("stop passing arguments to my pwd %s\n", argv[0]);
        exit(-1);
    }
    // if we are at no where
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Ohh no i am at a black hole can not know where");
        exit(-2);
    }


    // Print the current working directory
    printf("%s\n", cwd);

    return 0;

}

  */
