#include <iostream>
using namespace std;
#include <string>
#include <unistd.h> //library for gethostname and getlogin
#include <cstdlib>
#include <cstdio>
#include <cstring>

int main()
{
	string command;
	//int size = 30;
	//char *argv[30];
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

		char str[30] = command;
		char * pch;
		pch = strtok(str, " ");
		while(pch != NULL)
		{
			printf ("%s\n",pch);
			pch = strtok(NULL, " ");
		}
/*
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
*/
	}
	return 0;
}
