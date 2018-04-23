#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>

int x=0;

char commend[30][20];

int running_state=1;//1 前台 0 后台

int exev(char* cmd,char* cmd2)
{
     if(!strcmp(cmd,"history"))
        {
            if(strlen(cmd)>1 && (cmd+1)!=NULL)
            {
                int n=*(cmd+1);
                for(int q=x-n;q<x;q++)
                    printf("%s\n",(commend[q]));
            }
            else
            {
                for(int q=0;q<x;q++)
                    printf("%s\n",(commend[q]));
            }
            return 0;
        }
    execvp(cmd,cmd2);

}

int exe_fun_no_pipe(char* cmd,int sign)
{
       //用指针数组接收命令
        char *_argv[32];
        _argv[0] = cmd;
        if(*cmd==' ')
        {
            cmd++;
            _argv[0]=cmd;
        }
        int len=strlen(cmd);

        if(*(cmd+len-1)==' ')
            *(cmd+len-1)='\0';

        int i = 1;

        char *start = cmd;
        while(*start)
        {
            if((*start)==' ')   //每个命令以空格结束
            {
                *start = '\0';
                start++;
                _argv[i] = start;
                i++;
                continue;
            }
            start++;
        }

        _argv[i] = NULL;


        //特殊指令(前缀部分)
        if(!strcmp(_argv[0],"exit"))
            return 1;

        if(!strcmp(_argv[0],"cd"))
        {
            chdir(_argv[1]);
            return 0;
        }



        //特殊指令(后缀部分)
        int running_state=1;//running_state=1 前台运行  running_state=0  后台运行
        if(!strcmp(_argv[i-1],"&"))
        {
            running_state=0;
            _argv[i-1]=NULL;
            i--;
        }


        if(sign==0)  //无pipe
        {
                    pid_t id = fork();   //创建子进程
                    if(id < 0)
                    {
                        perror("fork");
                    }
                    else if(id == 0)
                    {
                        if(i>2)
                        {
                            if(!strcmp(_argv[i-2],">"))
                        {
                            char* output=_argv[i-1];
                            _argv[i-2]=NULL;
                            int fd=dup(1);
                            close(1);
                            int q=open(output, O_WRONLY);
                            ftruncate(q,0);

                            if(q==-1) printf("File open error.");
                            exev(_argv[0],_argv);

                            close(1);
                            int n=dup(fd);
                            close(fd);
                        }
                            else if(!strcmp(_argv[i-2],">>"))
                        {
                            char* output=_argv[i-1];
                            _argv[i-2]=NULL;
                            int fd=dup(1);
                            close(1);
                            int q=open(output,O_WRONLY|O_APPEND);
                            if(q==-1) printf("File open error.");
                            exev(_argv[0],_argv);

                            close(1);
                            int n=dup(fd);
                            close(fd);
                        }
                            else if(!strcmp(_argv[i-2],"<"))
                        {
                            char* input=_argv[i-1];
                            _argv[i-2]=NULL;
                            int fd=dup(0);
                            close(0);
                            int q=open(input,O_RDONLY);
                            if(q==-1) printf("File open error.");
                            exev(_argv[0],_argv);

                            close(0);
                            int n=dup(fd);
                            close(fd);
                        }

                        }
                        else{
                            exev(_argv[0],_argv);   //将接收到的命令传给系统
                        }


                        exit(1);
                    }
                    else
                    {
                        if(running_state==1)
                        {
                            int status = 0;
                            pid_t ret = waitpid(id,&status,0);
                            if(ret > 0 && WIFEXITED(status))
                            {}
                            else
                            {
                                perror("waitpid");
                            }

                        }
                        else
                        {
                             int status = 0;
                            waitpid(id,&status,WNOHANG);
                        }

                    }


        }
        else if(sign==1) //有pipe
        {
                    if(i>2)
                        {
                            if(!strcmp(_argv[i-2],">"))
                        {
                            char* output=_argv[i-1];
                            _argv[i-2]=NULL;
                            int fd=dup(1);
                            close(1);
                            int q=open(output, O_WRONLY);
                            ftruncate(q,0);

                            if(q==-1) printf("File open error.");
                            exev(_argv[0],_argv);

                            close(1);
                            int n=dup(fd);
                            close(fd);
                        }
                            else if(!strcmp(_argv[i-2],">>"))
                        {
                            char* output=_argv[i-1];
                            _argv[i-2]=NULL;
                            int fd=dup(1);
                            close(1);
                            int q=open(output,O_WRONLY|O_APPEND);
                            if(q==-1) printf("File open error.");
                            exev(_argv[0],_argv);

                            close(1);
                            int n=dup(fd);
                            close(fd);
                        }
                            else if(!strcmp(_argv[i-2],"<"))
                        {
                            char* input=_argv[i-1];
                            _argv[i-2]=NULL;
                            int fd=dup(0);
                            close(0);
                            int q=open(input,O_RDONLY);
                            if(q==-1) printf("File open error.");
                            exev(_argv[0],_argv);

                            close(0);
                            int n=dup(fd);
                            close(fd);
                        }

                        }
                        else{
                            exev(_argv[0],_argv);   //将接收到的命令传给系统
                        }


                        exit(1);


        }

    return 0;
}


int close_pipe_conn(int num,int sum,int fd[])
{
    for(int j=0;j<sum-1;j++)
    {
        if(j!=num && j!=num-1)
        {
            close(fd[2*j]);
            close(fd[2*j+1]);
        }
    }
    return 0;
}

int fun_exe_pipe(char* start)
{

    char *_argv[32];    //用指针数组接收命令
    _argv[0] = start;
    int i = 1;

    while(*start)
    {
        if((*start)==' ')   //每个命令以空格结束
        {
            *start = '\0';
            start++;
            _argv[i] = start;
            i++;
            continue;
        }
        start++;
    }

    _argv[i] = NULL;

    execvp(_argv[0],_argv);   //将接收到的命令传给系统

    return 1;

}

char last_one(char* str)
{
    int len=strlen(str);
    return *(str+len-1);
}



int main()
{
    char cmd[128];

    while(1)
    {
        printf("[user@localhost myshell]$ ");
        fflush(stdout);

        ssize_t _s = read(0,cmd,sizeof(cmd)-1);   //读取命令
        if(_s > 0)
        {
            cmd[_s - 1] = '\0';
        }
        else
        {
            perror("read");
            return 1;
        }

        strcpy(commend[x],cmd);
        x++;


        char *cmd_part[10];
        cmd_part[0]=cmd;
        int part_num=1;
        char* part_start = cmd;
        while(*part_start)
        {
            if(*part_start=='|')   //每个命令以空格结束
            {
                *part_start = '\0';
                part_start++;
                cmd_part[part_num]= part_start;
                part_num++;
                continue;
            }
            part_start++;
        }
        cmd_part[part_num]=NULL;

        int length=strlen(cmd);
        running_state=1;
        if(*(cmd+length-1)=='&')
        {
            running_state=0;
            *(cmd+length-1)='\0';
        }


        //有无pipe
        if(part_num<=1)
        {
            int n=exe_fun_no_pipe(cmd,0);
            if(n==1) exit(1);
            continue;
        }

        //创建pipe
        int fd_num=2*part_num;
        int fd[fd_num];
        for(int j=0;j<part_num-1;j++)
        {
            pipe(&fd[2*j]);
        }


        //创建子进程
        int j=0;
        pid_t id = fork();



        //first program
        if(last_one(cmd_part[0])=='&')
            running_state=0;

        if(id==0)
        {
            //关闭与其他pipe的联系
            close_pipe_conn(0,part_num,fd);
            //管理输入
            close(fd[0]);
            //重置输出
            close(1);
            dup(fd[1]);
            close(fd[1]);

            //执行
            exe_fun_no_pipe(cmd_part[0],1);
            exit(1);
        }
        else
        {
           if(running_state==1)
            {
                int status = 0;
                pid_t ret = waitpid(id,&status,0);
                if(ret > 0 && WIFEXITED(status))
                {}
                else
                {
                    perror("waitpid");
                }
            }
             else
            {
                int status = 0;
                waitpid(id,&status,WNOHANG);
            }

        }


        for(j=1;j<part_num-1;j++)
        {
            pid_t id = fork();   //创建子进程
            if(last_one(cmd_part[j])=='&')
            running_state=0;
            if(id!=0)
            {
                close(fd[2*j-2]);
                close(fd[2*j-1]);

                if(running_state==1)
            {
                int status = 0;
                pid_t ret = waitpid(id,&status,0);
                if(ret > 0 && WIFEXITED(status))
                {}
                else
                {
                    perror("waitpid");
                }
            }
             else
            {
                int status = 0;
                waitpid(id,&status,WNOHANG);
            }

            }
            else if(id==0)
            {
                //关闭与其他pipe的联系
                close_pipe_conn(j,part_num,fd);
                //管理输出
                close(fd[2*j-1]);
                dup2(fd[2*j+1],1);
                close(fd[2*j+1]);

                //管理输入
                dup2(fd[2*j-2],0);
                close(fd[2*j-2]);
                close(fd[2*j]);

                //执行

                exe_fun_no_pipe(cmd_part[j],1);

                exit(1);
            }

        }


        //last program
        id = fork();   //创建子进程
        if(last_one(cmd_part[j])=='&')
            running_state=0;
        if(id!=0)
            {
                close(fd[2*j-2]);
                close(fd[2*j-1]);
                if(running_state==1)
            {
                int status = 0;
                pid_t ret = waitpid(id,&status,0);
                if(ret > 0 && WIFEXITED(status))
                {}
                else
                {
                    perror("waitpid");
                }
            }
             else
            {
                int status = 0;
                waitpid(id,&status,WNOHANG);
            }

            }
        else if(id==0)
            {
                //关闭与其他pipe的联系
                close_pipe_conn(j,part_num,fd);
                //管理输出
                close(fd[2*j-1]);

                //管理输入
                dup2(fd[2*j-2],0);
                close(fd[2*j-2]);

                //执行

               exe_fun_no_pipe(cmd_part[j],1);

                exit(1);
            }

    }
    return 0;
}
