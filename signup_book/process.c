#include "process.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int signup(char student[100], int student_age){


    size_t student_memory = strlen(student);

    char *new_student = malloc(student_memory + 1);
    
    if (new_student == NULL){
        return 3;
    }

    char *is_old_enough = check_age(student_age);

    if (strcmp(is_old_enough,"OK") == 0)
    {
        return 1;
    }
    else{
        return 2;
    }
    
    // strcpy(new_student, student);


    // return new_student;

}

char *check_age(int student_age){

    if(student_age < 10){
        char *not_old = "Sorry you are too young to be enrolled!";
        return not_old;
    }
    else{
        char *old_enough = "OK";
        return old_enough;
    }


}