/*	Author: Willard Laxton			*/
/*	Project: CS 4760 Operating Systems P1	*/
/*	Date: 02-06-2019			*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>


//Header Files
#include "stack.h"

int main(int argc, char* argv[])
{
//Local Vars
	const int PID_ARRAY_SIZE = 20;
	int number_Of_Arguments = argc, option, o_Flag = 0, i_Flag = 0, argv_Counter = 1;
	int store_Pid[PID_ARRAY_SIZE], store_Pid_Position = 0, first_From_Input = 0;
	int number_Of_Iterations = 0, input_File_Data = 0, i = 0, o_Used_Flag = 0, i_Used_Flag = 0, size = 0;
	
	FILE *input_Fp, *output_Fp;
	pid_t forked_Process;
	struct Stack* stack = createStack(PID_ARRAY_SIZE); 

//Shared Memory Vars
	key_t key = 100;
	int shm_Input_File_Ptr_Id = shmget(key, 1024, 0777| IPC_CREAT);
	int *input_File_Position = (int*) shmat(shm_Input_File_Ptr_Id,(void*)0,0);

/****************************************
 * 	Shared Memory Initializations	*
 * 	Shared Memory Error Check	*
 ****************************************/
	if(shm_Input_File_Ptr_Id == -1)
        {
                fprintf(stderr, "%s: ", argv[0] );
       		perror("Error");
           	exit(EXIT_FAILURE);
        }
	
	if(*input_File_Position == -1)
	{
		fprintf(stderr, "%s: ", argv[0] );
                perror("Error");
                exit(EXIT_FAILURE);
	}

	if(number_Of_Arguments > 6)
        {       
       		fprintf(stderr, "%s: %s\n", argv[0], "Error: Too many arguments were given");
  		exit(EXIT_FAILURE);
        } 
	
//Initialize Array For Storing Child Pids With Default Values
	for(i = 0; i < PID_ARRAY_SIZE; i++)
	{
		store_Pid[i] = 0;
	}

//GetOpt Loop That Continues Until There Are No More Matching Options (h, i, o)
	while( (option = getopt(number_Of_Arguments, argv, "hio")) != -1)
	{
		switch(option)
		{
//Option h
			case 'h':
				printf("%s\n", "Options:");
				printf("\n%s\n", "-h");
				printf("%s\n\n", "List all of the legal command line options");
				printf("%s\n", "-i");
				printf("%s\n\n", "Reads from default input file named input.dat");
				printf("%s\n", "-i inputfilename");
				printf("%s\n\n", "Reads from given input file");
				printf("%s\n", "-o");
				printf("%s\n\n", "Writes to a default output file called output.dat");
				printf("%s\n", "-o outputfile name");
				printf("%s\n\n", "Writes to a specified output file");

//Exits The Program After Options Are Displayed If h Was The Only Option That Was Given
				if(number_Of_Arguments == 2)
				{
					exit(EXIT_SUCCESS);
				}

				argv_Counter++;
			break;

//Option i	
			case 'i':
				if( (argv[argv_Counter + 1] != NULL) && (strcmp(argv[argv_Counter + 1], "-h") != 0) && (strcmp(argv[argv_Counter + 1], "-i") != 0) && (strcmp(argv[argv_Counter + 1], "-o") != 0) )
				{
					input_Fp = fopen(argv[argv_Counter + 1], "r");

					if(input_Fp == NULL)
					{
						fprintf(stderr, "%s: ", argv[0] );		
						perror("Error");
						exit(EXIT_FAILURE);
					}
			
//Checks To See If The File Is Empty And Exits If It Is
					fseek (input_Fp, 0, SEEK_END);
    					size = ftell(input_Fp);

    					if(size == 0) 
					{
        					fprintf(stderr, "%s: %s\n", argv[0], "Error: Input file empty");
                				exit(EXIT_FAILURE);
  					}
	
					fseek(input_Fp, 0, SEEK_SET);

					i_Flag++;	
					argv_Counter++;
				}

//Prevents The i Option From Being Used More Than Once
				if(i_Used_Flag > 0)
                                {
                                        fprintf(stderr, "%s: %s\n", argv[0], "Error: You can't use the i option more than once");
                                        exit(EXIT_FAILURE);
                                }

				argv_Counter++;
				i_Used_Flag++;
			break;

//Option o
			case 'o':
                                if( (argv[argv_Counter + 1] != NULL) && (strcmp(argv[argv_Counter + 1], "-h") != 0) && (strcmp(argv[argv_Counter + 1], "-i") != 0) && (strcmp(argv[argv_Counter + 1], "-o") != 0) )
                                {
                                        output_Fp = fopen(argv[argv_Counter + 1], "w+");
                                        o_Flag++;
                                        argv_Counter++;
                                }

//Prevents The o Option From Being Used More Than Once
				if(o_Used_Flag > 0)
				{
					fprintf(stderr, "%s: %s\n", argv[0], "Error: You can't use the o option more than once");
					exit(EXIT_FAILURE);
				}

                                argv_Counter++;
				o_Used_Flag++;
			break;
		}
	}	

//Sets Default File For Input
	if(i_Flag == 0)
	{
		input_Fp = fopen("input", "r");

		if(input_Fp == NULL)
          	{
             		fprintf(stderr, "%s: %s\n", argv[0], "Error: File could not be opened");
                    	exit(EXIT_FAILURE);
            	}

//Checks To See If The File Is Empty And Exits If It Is
		fseek (input_Fp, 0, SEEK_END);
               	size = ftell(input_Fp);

            	if(size == 0)
             	{
                   	fprintf(stderr, "%s: %s\n", argv[0], "Error: Input file empty");
                 	exit(EXIT_FAILURE);
             	}

        	fseek(input_Fp, 0, SEEK_SET);
	}

//Sets Default File For Output
	if(o_Flag == 0)
        {
                output_Fp = fopen("output", "w+");

		if(output_Fp == NULL)
                {
                        fprintf(stderr, "%s: %s\n", argv[0], "Error: File could not be opened");
                        exit(EXIT_FAILURE);
                }
        }

//Get First Number From Input File Which Will Be Number Of Processes
	fscanf(input_Fp, "%d", &first_From_Input);

//Position Of File Pointer
	*input_File_Position = ftell(input_Fp);

//Start Of Forked Process
	for( i = 0; i < first_From_Input; i++)
	{
		forked_Process = fork();

		if(forked_Process == -1)
		{
			fprintf(stderr, "%s: ", argv[0] );
                       	perror("Error");
                     	exit(EXIT_FAILURE);
		}

		if(forked_Process == 0)
		{
			break;
		}
		else
		{	
			store_Pid[store_Pid_Position] = (int) forked_Process;
			store_Pid_Position++;
			wait();
		}
	}
	
//Child Process
	if(forked_Process == 0)
	{
		fseek(input_Fp, *input_File_Position, SEEK_SET);

		fscanf(input_Fp, "%d", &number_Of_Iterations);

//Reads From The Input File And Pushes The Numbers On To A Stack
                for(i = 0; i < number_Of_Iterations; i++)
                {
                        fscanf(input_Fp, "%d", &input_File_Data);
	
			if(!isFull(stack))
			{
				push(stack, input_File_Data);
                        }
			else
			{
				fprintf(stderr, "%s\n", "Error: Stack is full.");
				exit(EXIT_FAILURE);
			}
                }
		
		*input_File_Position = ftell(input_Fp);
		shmdt(input_File_Position);

		fprintf(output_Fp, "\n%d: ", (int) getpid());

		while(!isEmpty(stack))
		{
			fprintf(output_Fp, "%d ", pop(stack));
		}

		fclose(input_Fp);
                fclose(output_Fp);
		
		fprintf(output_Fp, "\n");
	}
//Parent Process
	else
	{	
		fprintf(output_Fp, "\nAll the children were: ");		

		store_Pid_Position = 0;
		while(store_Pid[store_Pid_Position] != 0)
		{
			fprintf(output_Fp, "%d ", store_Pid[store_Pid_Position]);
			store_Pid_Position++;
		}

		fprintf(output_Fp, "\nThe parent's pid was: %d\n", (int) getpid());		

//File Cleanup
		fclose(input_Fp);
		fclose(output_Fp);

//Destroy Shared Memory
		shmctl(shm_Input_File_Ptr_Id, IPC_RMID, NULL);
	}
}

