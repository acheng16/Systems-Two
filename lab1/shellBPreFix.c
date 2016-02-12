#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */


void line_search(FILE *fo, int line_number){
  fclose(fo);
  fo = fopen("history.txt","r");
  

}

void print_history(FILE *fo) {
  char line[MAXLINE];
  fclose(fo);
  fo = fopen("history.txt","r");
  while(fgets(line,MAXLINE,fo)!=NULL){
    strtok(line,"\n");
    printf("%s\t",line);
    fgets(line,MAXLINE, fo);
    printf("%s",line);
  }
  fclose(fo);
  fo = fopen("history.txt","a");
}

void retrieve_from_history(char inBuffer[], FILE *fo, int line_number) {
  char line[MAXLINE];
  char *token;
  FILE *temp_file;

  int count = 0;
  fclose(fo);
  int i = 0;

  fo = fopen("history.txt","r");
  temp_file = fopen("temp.txt","w+");

  fscanf(fo, "%d", &i);
  while( ( fgets(line,MAXLINE,fo)!=NULL ) && (line_number-1!=(i)) ){
    fprintf(temp_file,"%d\n",i);
    fgets(line,MAXLINE, fo);
    fprintf(temp_file,"%s",line);
    fscanf(fo, "%d", &i);
  }
  fgets(inBuffer,MAXLINE, fo);
  fprintf(temp_file,"%i\n", line_number);
  fprintf(temp_file,"%s", inBuffer);
  fclose(fo);
  fclose(temp_file);
  rename("temp.txt","history.txt");
  fo = fopen("history.txt","a");
}


/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

int setup(char inBuffer[], char *args[],int *bkgrnd, FILE *fo, int line_number)
{
    int length,  /* #  characters in the command line */
        start,   /* Beginning of next command parameter           */
        i,       /* Index for inBuffer arrray          */
        j;       /* Where to place the next parameter into args[] */

	char *r_num_char;    
	int r_num;

    /* Read what the user enters */
    memset(inBuffer,0,80);
    length = read(STDIN_FILENO, inBuffer, MAXLINE);  
    if ( ( length != 0 ) && ( strcmp(inBuffer,"rr\n")!=0 ) ) {
      fprintf(fo,"%i\n",line_number);
      fprintf(fo,"%s", inBuffer);
    }
    
    start = -1;
    j = 0;

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
	exit(-1);           /* Terminate with error code of -1 */
    }

    if (strcmp(inBuffer,"rr\n") == 0) {
      if (line_number==1) {
        printf("No recent history!");
	return 1;
      }else {
		memset(inBuffer,0,80);
	        retrieve_from_history(inBuffer, fo, line_number);
		length = strlen(inBuffer);
		
	}
    }
	
     if (strcmp(inBuffer,"r\n")== 0)  {
		printf("Syntax Error: Must include history number\n");
		return 1;
	}else if (strcmp(inBuffer,"r ") ==0) {
		strtok(inBuffer," ");	
		r_num_char=strtok(inBuffer," ");
		r_num = atoi(r_num_char);
		retrieve_from_history(inBuffer, fo, r_num);
	}
	/*if ( (strcmp(inBuffer,"history\n") == 0) || (strcmp(inBuffer,"h\n") == 0) ){
	      print_history(fo);
	      return 1;
	    }else if (  (strcmp(inBuffer,"history &\n") == 0) || (strcmp(inBuffer,"h &\n") == 0) ) {
	       *bkgrnd=1;
	       print_history(fo);
	       return 1;
	    }*/
    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
 
        switch (inBuffer[i]){
        case ' ':
        case '\t' :          /* Argument separators */
            if(start != -1){
                args[j] = &inBuffer[start];    /* Set up pointer */
                j++;
            }

            inBuffer[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inBuffer[start];     
                j++;
            }

            inBuffer[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *bkgrnd = 1;
            inBuffer[i] = '\0';
            break;
            
        default :             /* Some other character */
	  if (start == -1) {
	    start = i;
	  }
	}
 
    }    
    args[j] = NULL; /* Just in case the input line was > 80 */
} 

int main(void)
{
    char inBuffer[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int bkgrnd;             /* Equals 1 if a command is followed by '&', else 0 */
    int exec;
    pid_t pid;
    FILE *file_history;
    int bool;
    int line_number;

    line_number=1;

    file_history = fopen("history.txt","w+");
    while (1){            /* Program terminates normally inside setup */

	bkgrnd = 0;

	printf("SysIIShell--> ");  /* Shell prompt */
        fflush(0);

        bool = setup(inBuffer, args, &bkgrnd, file_history, line_number);    /* Get next command */
	if ( strcmp ( args[0], "history") == 0 || strcmp ( args[0], "h") == 0 ) {
		if ( args[1] == NULL || args[1] == "&") {
		print_history(file_history);
		bool = 1;
		}
	}
	if ( strcmp (args[0], "rr") == 0 && args[1] = NULL ) {
		retrive_from_history(line_number);
	}
			
	
	line_number++;
	pid = fork();
	if (bkgrnd == 0){
		if(pid != 0) {
			wait();
		}
	}
	if (pid == 0){
	  if (bool != 1){
		exec = execvp(inBuffer,args);
		if (exec<0) {
		  printf("The command does not exist.\n");
		}
	  }
	  _exit(0);
	}
	/* Fill in the code for these steps:  
	 (1) Fork a child process using fork(),
	 (2) The child process will invoke execvp(),
	 (3) If bkgrnd == 0, the parent will wait, 
		o/w returns to the setup() function. */
    }
}
