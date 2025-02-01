// The MIT License (MIT)
// 
// Copyright (c) 2024 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>


#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32     

int main(int argc, char *argv[])
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
	char error_message[30] = "An error has occurred\n";
	FILE *input_stream = stdin;
  if (argc > 1)
  {
  	input_stream = fopen(argv[1], "r");
  	if (input_stream == NULL)
  	{  		
  		write(STDERR_FILENO, error_message, strlen(error_message));
  		exit(1);
  	}
  
  fseek(input_stream, 0, SEEK_END);
  
  int file_size = ftell(input_stream);
  if (file_size == 0) {
      write(STDERR_FILENO, error_message, strlen(error_message));
      fclose(input_stream);
      exit(1);
  }
  
  else
  {
  	rewind(input_stream); 	
  }
 }
  
  while(1)
  {
    // Print out the msh prompt
    if (argc == 1)
    {
    	printf ("msh> ");
    }
    while( !fgets (command_string, MAX_COMMAND_SIZE, input_stream) );

    // Read the command from the commandi line.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something.
    

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_pointer;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    
    char *head_ptr = working_string;
    
    // Tokenize the input with whitespace used as the delimiter
    while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
    	if (strlen(argument_pointer) != 0)
    	{
    		token[token_count] = strndup(argument_pointer, MAX_COMMAND_SIZE );
    		token_count++;
    	}   
    }
    token[token_count] = NULL;
    token_count++;

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
	
    int token_index  = 0;

    //EXIT FUNCTION
    if (token[token_index]!=NULL)
    {
      if (strcmp(token[token_index],"exit")==0)
      {
      	if (token_count!=2)
      	{
      		write(STDERR_FILENO, error_message, strlen(error_message));
      		continue;
      	}
      	else
      	{
					free(head_ptr);
					exit(0);      	
      	}
      }
      
    //CD FUNCTION
		  else if (strcmp(token[token_index],"cd")==0)
		  {
				token_index++;
				if (chdir(token[token_index])!=0)
				{
					write(STDERR_FILENO, error_message, strlen(error_message));
					free(head_ptr);
					continue;  			
				}
				else
				{
					continue;
				}     		
		  }
		  
		  else
		  {
				pid_t pid = fork();
				if (pid < 0)
				{
						write(STDERR_FILENO, error_message, strlen(error_message));
						continue;
				}
				else if (pid == 0)
				{
					int i;
					for (i = 0; i < token_count-1; i++)
					{
						if ((strcmp(token[i],">") == 0) && i != 0)
						{
							token[i] = NULL;
							int fd = open(token[i+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
							if (fd < 0 || token[i+2]!=NULL)
							{
								write(STDERR_FILENO, error_message, strlen(error_message));
								free(head_ptr);
								exit(0);
							} 
							dup2(fd,1);
							close(fd);					  
						}
					}				
					
					if (execvp(token[0], token) < 0)
					{
					    write(STDERR_FILENO, error_message, strlen(error_message));
							
					}
				}
				
				else
				{
						wait(NULL);				  
				}			
		  }

		}
		
		if (input_stream != stdin && token[token_count]==NULL)
		{
				exit(0);
		} 

	}

  return 1;
  // e2520ca2-76f3-90d6-0242ac1210022
}

