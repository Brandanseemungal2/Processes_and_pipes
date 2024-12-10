// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100]; 
    pid_t p; 
  
    if (pipe(fd1) == -1 || pipe(fd2) == -1) { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 

    p = fork();

    if (p < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    else if (p > 0) {
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);
  
        printf("Enter a string to concatenate: ");
        scanf("%s", input_str);
        
        write(fd[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        wait(NULL); 

        char received_str[100];
        read(fd2[0], received_str, 100);
        close(fd2[0]);

        strcat(received_str, fixed_str2);
        printf("Concatenated string: %s\n", received_str);
    }
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); 
        close(fd1[0]);

        strcat(concat_str, fixed_str1);
        printf("Concatenated string: %s\n", concat_str);

        printf("Enter another string: ");
        char input_str2[100];
        scanf("%s", input_str2);

        write(fd2[1], input_str2, strlen(input_str2) + 1);
        close(fd2[1]);

        exit(0);
    }
}
