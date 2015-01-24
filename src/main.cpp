#include <iostream>
using namespace std;
#include <string>
#include <unistd.h> //library for gethostname and getlogin
#include <cstdlib>
#include <cstdio>
int main()
{
	string command;
	int size = 30;
	char *argv[size];
	//setup for print username/hostname
	char *login_name = getlogin();
	char hostname[100];
	gethostname(hostname,100);
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
		int pid = fork();
		if(pid == -1)
		{
			perror("fork");
			exit(1);
		}
		else if(pid == 0)
		{
			if(-1 == execvp(argv[0], argv))
				perror("There was an error in execvp");
			exit(1);	
		}
		else if(-1 == wait(0))
		{
			perror("There was an error with wait().");
		}
	}
	return 0;
}
