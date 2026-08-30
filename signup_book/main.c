#include <string.h>
#include <stdio.h>
#include "process.h"
#include <stdlib.h>

int main(){

    char student_name[50] = "";
    int  student_age = 0;
    printf("Welcome to X school\n");
    printf("Please enter the name of student: ");
    fgets(student_name,sizeof(student_name),stdin);
    student_name[strcspn(student_name, "\n")] = '\0';
    printf("\nWelcome %s",student_name);
    printf("\nWhat is your age?");
    printf("\n");
    scanf("%d",&student_age);
    
    const int pass_signup = signup(student_name,student_age);
    
    if(pass_signup == 1){
        printf("Welcome to school %s,",student_name);
    }else{
        printf("Sorry, you have to be older.");
    }
    
    free(signup);

    return 0;

}