#include <iostream>
using namespace std;
#include <string>
#include <unistd.h> //library for gethostname and getlogin
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pwd.h>
#include <fcntl.h>
#include <boost/tokenizer.hpp>
using namespace boost;

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
		execvp(argv[0],argv);
			perror("execvp failed");
		for(j=0,i=token.begin(); i != token.end(); ++j,++i)
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
		execvp(argv[0],argv);
                        perror("execvp failed");
                for(j=0,i=token.begin(); i != token.end(); ++j,++i)
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
	int location = command.find(">");
        string front = command.substr(0,location);
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
               
                execvp(argv[0],argv);
                        perror("execvp failed");
                for(j=0,i=token.begin(); i != token.end(); ++j,++i)
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
        int location = command.find(">>");
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
	string front = command.substr(0,location);
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

                execvp(argv[0],argv);
                        perror("execvp failed");
                for(j=0,i=token.begin(); i != token.end(); ++j,++i)
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


int main()
{
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
		cout << " $ ";
		getline(cin,command);
		if(command.find("#") != string::npos)
		{
			command.erase(command.find("#"));
		}
		if(command == "exit")
		{
			exit(0);
		}
		if(command.find("<<<") != string::npos)
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
	}
	return 0;
}
