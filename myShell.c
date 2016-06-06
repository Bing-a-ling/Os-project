#include <stdio.h>
#include <unistd.h>
#include<signal.h>
#include<errno.h>

#define HISNUM 10
#define MAX_LINE 80 /* The maximum length command */

typedef struct Queue /* A struct to save historical commands */
{
    int start;
    int end;
    int start_number; 
    char his_cmd[HISNUM][80];
}Queue;

typedef struct Node        /* A struct to save jobs*/
{   pid_t pid;
    char cmd[80];
    char stat[10];
    struct node * link;
}Node;

/*****  Global variables declaration *****/

Node *head, *tail;
Queue his_info;

/***** Function declaration *****/
void show_his_cmd(void);
void init_envi();
void exec_cd_cmd(char * route);
void add_history(char * buffer);
void setup(char inputBuffer[], char *args[], int *background);




int main(void)
{
    
    int background;   /* equals 1 if a command is followed by '&' */
    char * args[MAX_LINE/2 + 1];
    char inputBuffer[MAX_LINE]; /* buffer to hold command entered */

    init_envi();
    int run_flag = 1;

    

    while(run_flag) 
    {   
/***** Set Signals *****/
        struct sigaction action;
        action.sa_sigaction = show_his_cmd;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGINT, &action, NULL);
        signal(SIGCHLD, SIG_IGN);

        background = 0;
/****** Print the current directory and the hint *****/
        if (printf ("[Liao Xueying@%s] COMMAND-> ", get_current_dir_name()) == EOF)
        {
            fflush(stdout);
            continue;
        }
        fflush(stdout);
/*****  Get the input and change it into useful information *****/
        fgets(inputBuffer, 80, stdin);
        
/*****  Inbuild Commands *****/
        if(inputBuffer[0]=='r'&& (inputBuffer[1]==' '||inputBuffer[1]=='\n'))
        {   int cmd_num;
            if(inputBuffer[1] == '\n')
            {   
                add_history(his_info.his_cmd[his_info.end]);
                cmd_num = his_info.end;
            }
            else
            {   int i;
                if(his_info.end >= his_info.start)
                {   for(i = his_info.end; i != his_info.start - 1; i--)
                    {
                        if(his_info.his_cmd[i][0] == inputBuffer[2])
                            break;
                    }
                    if (i == his_info.start - 1)
                    {    printf("Can't find command with '%c'!\n", inputBuffer[2]);
                         continue;
                    }
                    else
                        cmd_num = i;
                }
                else
                {   for(i = his_info.end + HISNUM; i != his_info.start - 1; i--)
                    {
                        if(his_info.his_cmd[i % HISNUM][0] == inputBuffer[2])
                            break;
                    }
                    if (i == his_info.start - 1)
                    {    printf("Can't find command with '%c'!\n", inputBuffer[2]);
                         continue;
                    }
                    else
                        cmd_num = i;
                }
                add_history(his_info.his_cmd[cmd_num]);

            }
            setup(his_info.his_cmd[cmd_num], args, &background);
            exec(args,his_info.his_cmd[cmd_num],&run_flag, background);
            continue;
        }
/*****  execute *****/
        add_history(inputBuffer);
        setup(inputBuffer, args, &background);
        exec(args, inputBuffer, &run_flag, background);
        
    }
   // printf("\nwhy am i here???\n");
    return 0;
}

void exec(char * args[], char inputBuffer[], int *run_flag, int background)
{   int status=0;
    if(strcmp(args[0], "exit") == 0)/*exit*/
        {
            printf("Goodbye~\n");
            *run_flag = 0;
            return;
        }
        
        if(strcmp(args[0], "cd") == 0 ) /* cd */
        {   
            exec_cd_cmd(args[1]);
             
            return;
        }
        if(strcmp(args[0], "good") == 0 ) /* good night */
        {   printf("Good night, dear.\n");
            *run_flag = 0;
            return;
        }
        if(strcmp(args[0], "history") == 0 ) /* history */
        {
            show_his_cmd();

            return;
        }
 
        pid_t pid = fork();
        if(pid == 0)  /* child process */
        {   
            //printf("I'm child\n");
            execvp(args[0], args);
            printf("Wrong command!\n");
            *run_flag = 0;
            return;
        }
        else
        {
            if(pid < 0)  /* error */
                printf("fork error!\n");
            else        /* parent process */
            {  // printf("Parent. \n");  
                if(background == 0) /* wait till the child process ends */
                 //   printf("Wait..."); 
                    waitpid(pid, &status, 0);
            }
        }
}

void init_envi()
{  // printf("init\n");
    his_info.start = his_info.end = 0;
    his_info.start_number = 1;
    head = tail = NULL;
   
}


void show_his_cmd(void)
{  // printf("history\n %d", getpid());
    int i ,j,k= 0;
    printf("\n    Num        Command\n");
    if(his_info.start < his_info.end)
    {
        for(i = his_info.start; i<= his_info.end; i++)
        {
            printf("    %d        ", (his_info.start_number + i - his_info.start));
            for(j = 0; his_info.his_cmd[i][j] != '\n'; j++)
            {   
                    printf("%c", his_info.his_cmd[i][j]);
            }
            printf("\n");
        }        
    }
    else 
    {   //printf("start > end\n");
         if(his_info.start > his_info.end)
         {   for(i = his_info.start; i!= his_info.start +  HISNUM; i++)
             {
                 printf("    %d        ", his_info.start_number + i - his_info.start);
                 for(j = 0; his_info.his_cmd[i%HISNUM][j] != '\n'; j++)
                 {   
                         printf("%c", his_info.his_cmd[i%HISNUM][j]);
                 }
                 printf("\n");
             }
         }
    }  
}



void add_history(char *input)
{   int i,j;
    static int empty_flag = 1;
   // printf("added to his.");
    if(empty_flag)
    {   for(i = 0; input[i] !='\0'; i++)
           his_info.his_cmd[his_info.end][i] = input[i];
        empty_flag = 0;
    }
    else
    {
        his_info.end = (his_info.end + 1)% HISNUM;
        for(i = 0; input[i] !='\0'; i++)
            his_info.his_cmd[his_info.end][i] = input[i];
        if(his_info.end == his_info.start)
        {
            his_info.start = (his_info.start+1) % HISNUM;
            his_info.start_number++;
        }
    }
    
    his_info.his_cmd[his_info.end][i] = '\0';
  //  printf("%d %d\n", his_info.start, his_info.end);

}

void setup(char inputBuffer[], char *args[], int *background)
{       static int num = 0;
        int i = 0;
        char *p;
        for(p = inputBuffer; ;p++)
        {    
            args[i++] = p;
            while(*p != ' ' && *p != '\0' && *p != '&' && *p != '\n' )
            {
                p++;
            }
            if(*(p-1) == '\0')
                i--;
            if(*p != ' ')
            {   
                if(*p == '&') 
                {  
                    *background = 1;
                }
                *p = '\0';
                *(p+1) = '\n';
                break;
            }
            else
                *p = '\0';
        }
        args[i] = NULL; 

       
}

void exec_cd_cmd(char * route)
{
    if(route != NULL)
    {
        if(chdir(route) < 0)
            switch(errno)
            {
                case ENOENT:
                    fprintf(stderr, "DIRECTORY NOT FOUND\n");
                    break;
                case ENOTDIR:
                    fprintf(stderr, "NOT A DIRECTORY NAME\n");
                    break;
                case EACCES:
                    fprintf(stderr, "YOU DO NOT HAVE RIGHT TO ACCESS\n");
                    break;
                default:
                    fprintf(stderr, "SOME ERROR HAPPENED IN CHFIR\n");
            
            }
    }
} 
