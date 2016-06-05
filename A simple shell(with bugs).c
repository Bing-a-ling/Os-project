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
//int sig_flag = 0;
Node *head, *tail;
Queue his_info;

void show_his_cmd(void);
void init_envi();
void exec_cd_cmd(char * route);
void add_history(char * buffer);
void setup(char inputBuffer[], char *args[], int *background);
//void add_node();
//void del_node();
//void set_flag();




int main(void)
{
    
    int background;   /* equals 1 if a command is followed by '&' */
    char * args[MAX_LINE/2 + 1];
    int should_run = 1, status = 0;
    char inputBuffer[MAX_LINE]; /* buffer to hold command entered */

    init_envi();
    

    

    while(1) 
    {   
         /***** Set Signals *****/
     /*   struct sigaction action;
        action.sa_sigaction = show_his_cmd;
        sigfillset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;
        sigaction(SIGINT, &action, NULL);*/
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
        setup(inputBuffer, args, &background);

/*****  Inbuild Commands *****/
        
        if(strcmp(args[0], "exit") == 0)/*exit*/
        {
            add_history(inputBuffer);
            printf("Goodbye~\n");
            exit(0);
        }
        
        if(strcmp(args[0], "cd") == 0 ) /* cd */
        {   
            exec_cd_cmd(args[1]);

            add_history(inputBuffer);
            continue;
        }
        if(strcmp(args[0], "history") == 0 ) /* history */
        {   printf("%d %d %d %s", his_info.start, his_info.end, his_info.start_number, inputBuffer);
            show_his_cmd();
   //         add_history(inputBuffer);
            continue;
        }

        add_history(inputBuffer);

        pid_t pid = fork();
        if(pid == 0)  /* child process */
        {   
            printf("I'm child\n");
            execvp(args[0], args);
            printf("Wrong command!\n");
            break;
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
    printf("\nwhy am i here???\n");
    return 0;
}

void init_envi()
{   printf("init\n");
    his_info.start = his_info.end = 0;
    his_info.start_number = 1;
    head = tail = NULL;
   
}


void show_his_cmd(void)
{   printf("history\n %d", getpid());
    int i ,j,k= 0;
    printf("%d, %d, %s",his_info.start,his_info.start,his_info.his_cmd[0]);
    printf("\n    Num        Command\n");
    if(his_info.start < his_info.end)
    {   printf("start < end\n");
        for(i = his_info.start; i< his_info.end; i++)
        {
            printf("    %d        ", his_info.start_number + i - his_info.start);
            for(j = 0; his_info.his_cmd[i][j] != '\n'; j++)
            {   if(his_info.his_cmd[i][j] == '\0')
                    printf(" ");
                else
                    printf("%s ", his_info.his_cmd[i][j]);
            }
        }        
    }
    else 
    {   printf("start > end\n");
        {   for(i = his_info.start; (i% HISNUM) != his_info.end; i++)
            {
                printf("    %d        ", his_info.start_number + i - his_info.start);
                for(j = 0; his_info.his_cmd[i%HISNUM][j] != '\n'; j++)
                {   if(his_info.his_cmd[i%HISNUM][j] == '\0')
                        printf(" ");
                    else
                        printf("%s ", his_info.his_cmd[i][j]);
                }
            }
        }
    }   
}

void add_history(char *input)
{   
    int i;
    printf("added to his.");
    his_info.end = (his_info.end + 1)% HISNUM;
    if(his_info.end == his_info.start){
        his_info.start = (his_info.start+1) % HISNUM;
        his_info.start_number++;
    }
    for(i = 0; input[i] !='\n'; i++)
       his_info.his_cmd[his_info.end][i] = input[i];
    input[i] = '\n';
    printf("%d %d\n", his_info.start, his_info.end);

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
