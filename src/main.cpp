#include <iostream>
using namespace std;
#include <string>
#include <unistd.h> //library for gethostname and getlogin
#include <cstdlib>
int main()
{
	string command;
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
			exit(1);
		}
	}
	return 0;
}
