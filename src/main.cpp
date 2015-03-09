#include <iostream>
#include <string>
#include <unistd.h> //library for gethostname and getlogin
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <boost/tokenizer.hpp>
#include <signal.h>
#include <dirent.h>
pid_t prid = 0;
int pfd[2];
using namespace boost;
using namespace std;

int new_exec(const string &one, char *const in[])
{
	string final = one;
	char *p;
	string s;
	if((p=getenv("PATH")) == NULL)
    	{
        	perror("getenv failed");
        	exit(1);
    	}
	s=p;
    	char_separator<char> sep(":");
    	tokenizer<char_separator<char> > token(s,sep);
    	tokenizer<char_separator<char> >::iterator it = token.begin();
    	for(;it!=token.end();++it)
    	{
		string path = *it + "/" + one;
    	        struct stat buffer;
    	        if(-1==stat(path.c_str(),&buffer))
   	        {
       	    		continue;
       	    		perror("stat failed");
       	        }
       	        else
			final = path;
        }
	return execv(final.c_str(),in);
}
void colon_connector(const string &command)
{
        int id;
	char_separator<char> sep(";");
        tokenizer<char_separator<char> > tokens(command,sep);
        tokenizer<char_separator<char> >::iterator it = tokens.begin();
        for(; it != tokens.end(); ++it)
        {
         	if(-1 == pipe(pfd))
		{
			perror("fork failed");
			exit(1);
		}
		id=fork();	
                prid=id;
		if(-1 == id)
                {
                    perror("fork failed");
                    exit(1);
                }
                else if(id == 0)
                {
			int j=0;
                        char *input[50];
                        char_separator<char> sep(" ");
                        tokenizer<char_separator<char> > token(*it,sep);
                        tokenizer<char_separator<char> >::iterator i = token.begin();
                        if(i != token.end())
                        {
                                if(*i == "exit")
                                        exit(8);
                        }
                        for(; i != token.end(); ++i,++j)
                        {
                                input[j] =new char[(*i).size()];
                                strcpy(input[j],(*i).c_str());
                        }
			input[j] = 0;
			if(-1 == new_exec(input[0],input))
			{
		                perror(input[0]);
                                j=0;
                                for(i = token.begin(); i != token.end(); ++i,++j)
                                        delete [] input[j];
                                exit(1);
			}
		}
                else
                {
                        int stat_val = 0;
                        if(-1 == wait(&stat_val))
                        {
                                perror("wait failed");
                                exit(1);
                        }

                        int exitChild;
                        if(WIFEXITED(stat_val))
                        {
                                exitChild = WEXITSTATUS(stat_val);
                                if(exitChild == 8)
                                        exit(0);
                        }

                        
	        }
	}
}


void and_connector(const string &command)
{
	int id;
	char_separator<char> sep("&&");
        tokenizer<char_separator<char> > tokens(command,sep);
        tokenizer<char_separator<char> >::iterator it = tokens.begin();
    	for(; it != tokens.end(); ++it)
	{
        	if(-1 == pipe(pfd))
                {
                        perror("fork failed");
                        exit(1);
                }
		id = fork();
                prid=id;
        	if(-1 == id)
		{
        	    perror("fork failed");
        	    exit(1);
        	}
		else if(id == 0)
		{
       			char *input[50];
			char_separator<char> sep(" ");
        		tokenizer<char_separator<char> > token(*it,sep);
        		tokenizer<char_separator<char> >::iterator i = token.begin();
    			if(i != token.end())
			{
				if(*i == "exit")
        				exit(8);
				//
    			}
    			int j = 0;
    			for(; i != token.end(); ++i,++j)
			{
        			input[j] =new char[(*i).size()];
        			strcpy(input[j],(*i).c_str());
    			}
    			input[j] = 0;
		        if(-1 == new_exec(input[0],input))
                        {
                                perror(input[0]);
                                j=0;
                                for(i = token.begin(); i != token.end(); ++i,++j)
                                        delete [] input[j];
                                exit(1);
                        }
		}
		else
		{
            		int stat_val = 0;
            		if(-1 == wait(&stat_val))
			{
                		perror("wait failed");
                		exit(1);
            		}

			int exitChild;
                        if(WIFEXITED(stat_val))
                        {
                                exitChild = WEXITSTATUS(stat_val);
                                if(exitChild == 1)
                                        return;
                                else if(exitChild == 8)
                                        exit(0);

			}
	      	}	
    	}
}

void or_connector(const string &command)
{
        int id;
	char_separator<char> sep("||");
        tokenizer<char_separator<char> > tokens(command,sep);
        tokenizer<char_separator<char> >::iterator it = tokens.begin();
        for(; it != tokens.end(); ++it)
        {
		if(-1 == pipe(pfd))
                {
                        perror("fork failed");
                        exit(1);
                }
                id =fork();
                prid = id;
		if(-1 == id)
                {
                    perror("fork failed");
                    exit(1);
                }
                else if(id == 0)
                {
                        char *input[50];
                        char_separator<char> sep(" ");
                        tokenizer<char_separator<char> > token(*it,sep);
                        tokenizer<char_separator<char> >::iterator i = token.begin();
                        if(i != token.end())
                        {
                                if(*i == "exit")
                                        exit(8);
                        }
                        int j = 0;
                        for(; i != token.end(); ++i,++j)
                        {
                                input[j] =new char[(*i).size()];
                                strcpy(input[j],(*i).c_str());
                        }
                        input[j] = 0;
                        if(-1 == new_exec(input[0],input))
                        {
                                perror(input[0]);
                                j=0;
                                for(i = token.begin(); i != token.end(); ++i,++j)
                                        delete [] input[j];
                                exit(1);
                        }
                }
                else
                {
                        int stat_val = 0;
                        if(-1 == wait(&stat_val))
                        {
                                perror("wait failed");
                                exit(1);
                        }
			int exitChild;
                        if(WIFEXITED(stat_val))
			{
				exitChild = WEXITSTATUS(stat_val);
				if(exitChild == 0)
					return;
				else if(exitChild == 8)
					exit(0);
			}
                }
        }
}

void input_redirection(const string &command)
{
	string back = command.substr(command.find("<")+1);
	int fd;
	char_separator<char> sep(" ");
	tokenizer<char_separator<char> > tokens(back,sep);
	tokenizer<char_separator<char> >::iterator it = tokens.begin();
        if(it == tokens.end())
	{
		cout << "file error" << endl;
		return; 
	}
	if(-1==(fd = open((*it).c_str(),O_RDONLY,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)))
	{
		perror("open failed");
		return;
	}
	string front = command.substr(0,command.find("<"));
	int pid = fork();
	if(pid == -1)
		perror("fork failed");
	else if (pid == 0)
        {
		int j=0;
		char *argv[50];
		tokenizer<char_separator<char> > token(front,sep);
		tokenizer<char_separator<char> >::iterator i = token.begin();
		if(i == token.end())
		{
			cout << "no command" << endl;
			exit(1);
		}
		for(; i != token.end(); ++i,++j)
		{
			argv[j] = new char[(*i).size()];
			strcpy(argv[j],(*i).c_str());
		}
		argv[j] = NULL;
		if(fd >= 0)
		{
			if(close(0) == -1)
                        {
                                perror("close failed");
                                exit(1);
                        }
			if(dup(fd) == -1)
                        {
                                perror("dup failed");
                                exit(1);
                        }
		}
		argv[j] = 0;
                if(-1 == new_exec(argv[0],argv))
                        perror(argv[0]);
                j=0;
                for(i = token.begin(); i != token.end(); ++i,++j)
                        delete [] argv[j];
                exit(1);
	}
	else
	{
		if(wait(NULL) == -1)
		{
			perror("wait failed");
			exit(1);
		}
		if(fd != -1)
		{
			if(close(fd) == -1)
			{
				perror("close");
				exit(1);
			}
		}
	}
}

void input_redirectionS(const string &command)
{
	string front = command.substr(0,command.find("<<<"));
        string back = command.substr(command.find("<<<")+3);
        string inp_string;
	int fd[2];
	if(command.find("\"") == string::npos)
	{
		cout << "string not found" << endl;
		return;
	}
	else
	{
		inp_string = command.substr(command.find("\"")+1);
		inp_string = inp_string.substr(0,inp_string.find("\""));
		if(pipe(fd) == -1)
		{
			perror("pipe failed");
			exit(1);
		}
		int size = inp_string.size();
		char *buf = new char[size];
		strcpy(buf,inp_string.c_str());
		if(-1 == write(fd[1],buf,strlen(buf)))
		{
			perror("write failed");
			exit(1);
		}
		if(-1 == close(fd[1]))
			perror("close failed");
		delete [] buf;
	}
        int pid = fork();
        if(pid == -1)
                perror("fork failed");
        else if (pid == 0)
        {
                int j=0;
		char *argv[50];
		char_separator<char> sep(" ");
                tokenizer<char_separator<char> > token(front,sep);
                tokenizer<char_separator<char> >::iterator i = token.begin();
                if(i == token.end())
                {
                        cout << "no command" << endl;
                        exit(1);
                }
                for(; i != token.end(); ++i,++j)
                {
                        argv[j] = new char[(*i).size()];
                        strcpy(argv[j],(*i).c_str());
                }
                argv[j] = NULL;
                if(fd[0] >= 0)
		{
			if(close(0) == -1)
                        {
                                perror("close failed");
                                exit(1);
                        }
			if(dup(fd[0]) == -1)
			{
				perror("dup failed");
				exit(1);
			}
		}
		argv[j] = 0;
                if(-1 == new_exec(argv[0],argv))
                	perror(argv[0]);
                j=0;
                for(i = token.begin(); i != token.end(); ++i,++j)
                	delete [] argv[j];
                exit(1);
	}
        else
        {
                if(wait(NULL) == -1)
                {
                        perror("wait failed");
                        exit(1);
                }
                if(fd[0] != -1)
                {
                        if(close(fd[0]) == -1)
                        {
                                perror("close");
                                exit(1);
                        }
                }
        }
}


void output_redirection(const string& command)
{
	string back = command.substr(command.find(">")+1);
	char_separator<char> sep(" ");
        tokenizer<char_separator<char> > tokens(back,sep);
        tokenizer<char_separator<char> >::iterator it = tokens.begin();
        if(it == tokens.end())
        {
                cout << "file error" << endl;
                return;
        }
        int fd;
        if(-1==(fd = open((*it).c_str(),((O_RDWR|O_CREAT)| O_TRUNC),S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)))
        {
                perror("open failed");
                return;
        }
        string front = command.substr(0,command.find(">"));
	int pid = fork();
	if(pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else if(pid == 0)
	{
		int j=0;
		char *argv[50];
                tokenizer<char_separator<char> > token(front,sep);
                tokenizer<char_separator<char> >::iterator i = token.begin();
                if(i == token.end())
                {
                        cout << "no command" << endl;
                        exit(1);
                }
                for(; i != token.end(); ++i,++j)
                {
                        argv[j] = new char[(*i).size()];
                        strcpy(argv[j],(*i).c_str());
                }
                argv[j] = NULL;
                if(close(0) == -1)
                {
                	perror("close failed");
                       	exit(1);
                }
                if(dup(1) == -1)
                {
			perror("dup failed");
                        exit(1);
                }
                if(close(1) == -1)
                {
                	perror("close failed");
                        exit(1);
                }
                if(dup(fd) == -1)
                {
                	perror("dup failed");
                        exit(1);
                }
		argv[j] = 0;
                if(-1 == new_exec(argv[0],argv))
                        perror(argv[0]);
                j=0;
                for(i = token.begin(); i != token.end(); ++i,++j)
                        delete [] argv[j];
                exit(1);
	}
	else
	{
		if(wait(NULL) == -1)
		{
			perror("wait failed");
			exit(1);
		}
		if(close(fd) == -1)
		{
			perror("close failed");
			exit(1);
		}
	}
}

void output_redirectionD(const string& command)
{
        string back = command.substr(command.find(">>")+2);
        char_separator<char> sep(" ");
        tokenizer<char_separator<char> > tokens(back,sep);
        tokenizer<char_separator<char> >::iterator it = tokens.begin();
        if(it == tokens.end())
        {
                cout << "file error" << endl;
                return;
        }
        int fd;
        if(-1==(fd = open((*it).c_str(),((O_RDWR|O_CREAT)| O_APPEND),S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)))
        {
                perror("open failed");
                return;
        }
	string front = command.substr(0,command.find(">>"));
        int pid = fork();
        if(pid == -1)
        {
                perror("fork");
                exit(1);
        }
        else if(pid == 0)
        {
                char *argv[50];
                tokenizer<char_separator<char> > token(front,sep);
                tokenizer<char_separator<char> >::iterator i = token.begin();
                if(i == token.end())
                {
                        cout << "no command" << endl;
                        exit(1);
                }
                int j=0;
                for(; i != token.end(); ++i,++j)
                {
                        argv[j] = new char[(*i).size()];
                        strcpy(argv[j],(*i).c_str());
                }
                argv[j] = NULL;
                if(close(0) == -1)
                {
                        perror("close failed");
                        exit(1);
                }
                if(dup(1) == -1)
                {
                        perror("dup failed");
                        exit(1);
		}
		if(close(1) == -1)
                {
                        perror("close failed");
                        exit(1);
                }
                if(dup(fd) == -1)
                {
                        perror("dup failed");
                        exit(1);
                }
		argv[j] = 0;
                if(-1 == new_exec(argv[0],argv))
                        perror(argv[0]);
                j=0;
                for(i = token.begin(); i != token.end(); ++i,++j)
                        delete [] argv[j];
                exit(1);
        }
        else
        {
                if(wait(NULL) == -1)
                {
                        perror("wait failed");
                        exit(1);
                }
                if(close(fd) == -1)
                {
                        perror("close failed");
                        exit(1);
                }
        }
}

void command_cd(const string &command)
{
	if(pipe(pfd) == -1)
	{
		perror("exit failed");
		exit(1);
	}

	int id = fork();
	prid = id;
	if(id == -1)
	{
		perror("fork failed");
		exit(1);
	}
	else if(id == 0)
	{
		char_separator<char> sep(" ");
	       	tokenizer<char_separator<char> > tokens(command,sep);
	       	tokenizer<char_separator<char> >::iterator it = tokens.begin();
		++it;
		string new_path = *it;
		if(it == tokens.end())
		{
			cout << "No second arguement!" << endl;
			return;
		}
		if(-1 == write(pfd[1],new_path.c_str(),new_path.size()))
		{
			perror("write failed");
			exit(1);
		}
		exit(4);
	}
	else
	{
		int stat_val = 0;
		if(waitpid(id,&stat_val,WUNTRACED)== -1)
		{
                	perror("WAIT");
                	exit(1);
		}
		if(WIFEXITED(stat_val))
		{
			int childExit = WEXITSTATUS(stat_val);
			if(childExit == 4)
			{
                		char end[BUFSIZ];
                		memset(end,0,BUFSIZ);
                		if(-1==close(pfd[1]))
                		{
                		    perror("close failed");
                		    exit(1);
                		}
                		if(-1==read(pfd[0],end,BUFSIZ))
                		{
       	        		    perror("read failed");
       	        		    exit(1);
        	       		}
       	        		if(-1==close(pfd[0]))
                		{
                		    perror("close failes");
                		    exit(1);
                		}
                		if(-1==chdir(end))
        	       		{
                	    		perror("chdir failed");
        	       	    		return;
        	       		}	
       			}
		}			
	}
	return;

}

void C_handler(int sig)
{
	if(0 != prid)
	{
		kill(prid,SIGKILL);
		prid = 0;
	}
}

void Z_handler(int sig)
{
	if(0 != prid)
	{
		kill(prid,SIGSTOP);
		//raise(SIGSTOP);
		cout << endl << "Paused foreground" << endl;
	}
}

void command_fg()
{
	if(prid != 0)
        	kill(prid, SIGCONT);
        else
                cout << "fg failed" << endl;
	return;
}

void command_bg()
{
	if(prid != 0)
	{
        	kill(prid, SIGCONT);
                prid = 0;
        }
        else
        	cout << "bg failed" << endl;
	return;
}


int main()
{
	if(SIG_ERR == signal(SIGINT,C_handler))
	{
		perror("signal failed");
		exit(1);
	}
	if(SIG_ERR == signal(SIGTSTP,Z_handler))
	{
		perror("signal failed");
		exit(1);
	}
	//setup for print username/hostname
	string command;
	char *login_name = getlogin();
	if(login_name == NULL)
		perror("login failed");
	char hostname[100];
	gethostname(hostname,100);
	if(hostname == NULL)
		perror("hostname failed");
	while(true)
	{
		if(login_name)
		{
			cout << login_name << "@" << hostname;
		}

		char currentD[BUFSIZ];
		if(NULL == getcwd(currentD,BUFSIZ))
		{
			perror("getcwd failed");
			exit(1);
		}

		cout << ":" << currentD << " $ ";
		getline(cin,command);
		if(command.find("#") != string::npos)
		{
			command = command.substr(0,command.find("#"));
		}
		if(command == "exit")
		{
			exit(0);
		}
		if(command.find("&&") != string::npos)
			and_connector(command);
		else if(command.find("||") != string::npos)
			or_connector(command);
		else if(command.find("<<<") != string::npos)
		{
			input_redirectionS(command);
			cout << endl;
		}
		else if(command.find("<") != string::npos)
			input_redirection(command);
		else if(command.find(">>") != string::npos)
			output_redirectionD(command);
		else if(command.find(">") != string::npos)
			output_redirection(command);
		else if(command.find("cd") != string::npos)
			command_cd(command);
		else if(command.find("fg") != string::npos)
			command_fg();
        	else if(command.find("bg") != string::npos)
			command_bg();
		else
			colon_connector(command);
		char currD[BUFSIZ];
        	if(NULL == getcwd(currD,BUFSIZ))
        	{
                	perror("getcwd failed");
                	exit(1);
        	}
        	cout << currD << endl;
	}
	return 0;
}
