#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <boost/format.hpp>
#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

bool stringCompare(const string &first, const string &second)
{
	for(string::const_iterator it_left = first.begin(), it_right = second.begin();
		it_left != first.end() && it_right != second.end();
	 	++it_left, ++it_right)
		if(tolower(*it_left) < tolower(*it_right))
			return true;
		else if(tolower(*it_left) > tolower(*it_right))
			return false;
		if(first.size() < second.size())
			return true;
	return false;
}

void L_flag(const string path, const string &file)
{
	struct stat s;
	if(lstat(path.c_str(), &s) == -1)
	{
		perror("lstat failed");
		exit(1);
	}
	bool blueDir = false;
	if(S_ISREG(s.st_mode))
		cout << "-";
	else if(S_ISFIFO(s.st_mode))
                cout << "p";
	else if(S_ISCHR(s.st_mode))
                cout << "c";
	else if(S_ISBLK(s.st_mode))
                cout << "b";
	else if(S_ISLNK(s.st_mode))
                cout << "l";
        else if(S_ISSOCK(s.st_mode))
                cout << "s";
	else if(S_ISDIR(s.st_mode))
	{
		cout << "d";
		blueDir = true;
	}
	else
		cout << "?";

	cout << ((s.st_mode & S_IRUSR)?"r":"-");
        cout << ((s.st_mode & S_IWUSR)?"w":"-");
        
	bool greenExec = false;
	if(s.st_mode & S_IXUSR)
	{
		cout << "x";
		greenExec = true;
	}
	else
		cout << "-";

        cout << ((s.st_mode & S_IRGRP)?"r":"-");
        cout << ((s.st_mode & S_IWGRP)?"w":"-");
        cout << ((s.st_mode & S_IXGRP)?"x":"-");
        cout << ((s.st_mode & S_IROTH)?"r":"-");
        cout << ((s.st_mode & S_IWOTH)?"w":"-");
        cout << ((s.st_mode & S_IXOTH)?"x":"-");
		
	struct group *group_name = getgrgid(s.st_gid);
	if(group_name == NULL)
	{
		perror("getGroup failed");
		exit(1);
	}
        struct passwd *user_name = getpwuid(s.st_uid);
	if(user_name == NULL)
	{
		perror("getUSR failed");
		exit(1);
	}
	cout << "  " << left << setw(4) << s.st_nlink 
	<< ' ' << setw(12) 
	<< user_name->pw_name << setw(12) 
	<< group_name->gr_name << setw(9)
	<< s.st_size; 
	
	string time = ctime(&s.st_mtime);
        time = time.substr(3, time.size()-12) + " ";
	cout << setw(15) << time;
	
	if(file.at(0) == '.')
		cout << "\x1b[100m";
	if(greenExec)
		cout << "\x1b[92m";
	if(blueDir)
                cout << "\x1b[94m";
	
	cout << file << "\x1b[0m" <<  endl;
	
}

void R_flag(vector<string> &file, string path, bool flagA, bool flagL)
{
	unsigned total = 1;
        unsigned align = 85;
	cout << endl;
	cout << path << ":" << endl;
	vector<string> directory;

	if(flagL)
	{
		//print total
		long ret_total = 0;
		for(unsigned i=0; i <file.size(); ++i)
		{
			struct stat s;
			string str_path = path + "/" + file.at(i);
			if(lstat(str_path.c_str(), &s) == -1)
			{
				perror("lstat failed");
				exit(1);
			}
			if(file.at(i).at(0) != '.' || flagA)
				ret_total += s.st_blocks/2;
		}
		cout << "total " << ret_total << endl;
	}
	else 
	{
		unsigned i=0;
		for(; i < file.size(); ++i)
		{
			unsigned file_sz = file.at(i).size();
			if(flagA && total < file_sz)
				total = file_sz;
			else if (file.at(i).at(0) != '.' && total < file_sz)
				total = file_sz;
		}
	
		total = total + 2;
		align = align / total;
	}
	int p = align;

	for(unsigned i=0; i < file.size(); ++i)
	{
		struct stat s;
		string name = path + "/" + file.at(i);
		if(lstat(name.c_str(), &s) == -1){
			perror("lstat failed");
			exit(1);
		}
	
		if(flagL)
		{
			if(file.at(i).at(0) != '.' || flagA)
				L_flag(name,file.at(i));
		}
		else
		{
			if(s.st_mode & S_IXUSR)
				cout << "\x1b[92m";
			if(S_ISDIR(s.st_mode))
				cout << "\x1b[94m";
			if(file.at(i).at(0) == '.')
				cout << "\x1b[100m";
	
			if(file.at(i).at(0) != '.' || flagA)
				cout << left << setw(total) << file.at(i);
			cout << "\x1b[0m";
			align = align - 1;
			if(align <= 0)
			{
				cout << endl;
				align = p;
			}
		}
		if(S_ISDIR(s.st_mode))
		{
			if(file.at(i).at(0) != '.' || flagA)
			{
				directory.push_back(file.at(i));
			}
		}
	}
	
	for(unsigned i=0; i < directory.size(); ++i)
	{
		file.clear();
	
		if(directory.at(i) == ".." || directory.at(i) == ".")
			continue;
		string name = path + "/" + directory.at(i);
		DIR *dirp = opendir(name.c_str());
		dirent *direntp;
		if(dirp == NULL)
		{
			perror("opendir failed");
			exit(1);
		}
		while((direntp = readdir(dirp)))
		{
			if(direntp == NULL)
			{
				perror("readdir failed");
				exit(1);
			}
			string s = direntp->d_name;
			file.push_back(s);
		}
		if(closedir(dirp) == -1)
		{
			perror("closedir failed");
			exit(1);
		}
		sort(file.begin(),file.end(),stringCompare);
		cout << endl;
		R_flag(file, path + "/" + directory.at(i), flagA, flagL);
	}
}

int main(int argc, char*argv[])
{
	unsigned total = 1;
        unsigned align = 85;
        unsigned p;
	bool print_total = true;
	int flag_type = 0;
	vector <string> inp;
	int i=1;	//starting at first option flag not ls command
	for(; i < argc;i++) 
	{
		if(argv[i][0] == '-') 
		{
			int j = 1;
			for(; argv[i][j] != 0 && argv[i][j] != ' ';j++)
			{
				char input = argv[i][j];
				if(input == 'a')
				{
					flag_type = flag_type|1;
				}
				else if(input == 'l')
				{
					flag_type = flag_type|2;
				}
				else if(input == 'R')
				{	
					flag_type = flag_type|4;
				}
				else
				{
					//a, l, or R was not passed 
					cout << "Unrecognized flag!" << endl;
					exit(1);
				}
			}
			if(j == 1)
			{
				//no input after -
				cout << "Unrecognized flag!" << endl;
				exit(1);
			}
		}
		else
		{
			inp.push_back(argv[i]);
		}
	}

	vector<string> files;
	string dirName = ".";
	DIR *dirp = opendir(dirName.c_str());
	if(dirp == NULL)
	{
		perror("opendir failed");
		exit(1);
	}
	dirent *direntp;
	while((direntp = readdir(dirp)))
	{
		if(direntp == NULL)
		{
			perror("readdir failed");
			exit(1);
		}
		files.push_back(direntp->d_name);
	}
	if(closedir(dirp) == -1)
	{	
		perror("closedir failed");
		exit(1);
	}
	sort(files.begin(),files.end(),stringCompare);
	
	if(!(flag_type&2))
	{
		for(unsigned i =0; i < files.size(); ++i)
		{
			unsigned size = files.at(i).size();
			if((flag_type&1) && total < size)
				total = size;
			else if(files.at(i).at(0) != '.' && total < size)
				total = size;
		}
		total = total + 2;
		align = align/total;
		p = align;
	}
	for(unsigned i=0; i < files.size(); ++i) //perform on every file in directory
	{
		struct stat s;
		string temp = dirName + "/" + files.at(i);
		if(lstat(temp.c_str(), &s) == -1)
		{
			perror("lstat failed");
			exit(1);
		}
		//check for flags -l -R -a
		if((flag_type&2) && (flag_type&1) && (flag_type&4))
		{
			R_flag(files, dirName, true, true);
			break;
		}
		//check for flags -l -a
		else if ((flag_type&2) && (flag_type&1))
                {
                        if(print_total)
                        {
                                long ret_total = 0;
                                for(unsigned i=0; i <files.size(); ++i)
                                {
                                        struct stat s1;
                                        string str_path = dirName + "/" + files.at(i);
                                        if(lstat(str_path.c_str(), &s1) == -1)
                                        {
                                                perror("lstat failed");
                                                exit(1);
                                        }
                                        ret_total += s1.st_blocks/2;
                                 }
                                cout << "total " << ret_total << endl;
                                print_total = false;
                        }
                        L_flag(dirName + "/" + files.at(i),files.at(i));
                }
		//check for flags -l -R
		else if ((flag_type&2) && (flag_type&4))
                {
                        R_flag(files, dirName, false, true);
                        break;
                }
		//check for flags -a -R
		else if ((flag_type&1) && (flag_type&4))
                {
                        R_flag(files, dirName, true, false);
                        break;
                }
		//check for flags -a
		else if (flag_type&1)
                {
			if(files.at(i).at(0) == '.')
                               	cout << "\x1b[100m";
                        if(s.st_mode & S_IXUSR)
                                cout << "\x1b[92m";
			if(S_ISDIR(s.st_mode))
                                cout << "\x1b[94m";
			cout << left << setw(total) << files.at(i) << "\x1b[0m";//10=total
			align = align-1;
                        if(align <= 0)
                        {
                                cout << endl;
                                align = p;
                        }
                }
		//check for flas -l
		else if(flag_type&2)
                {
                        if(print_total)
                        {
                                long ret_total = 0;
                                for(unsigned i=0; i <files.size(); ++i)
                                {
                                        struct stat s1;
                                        string str_path = dirName + "/" + files.at(i);
                                        if(lstat(str_path.c_str(), &s1) == -1)
                                        {
                                                perror("lstat failed");
                                                exit(1);
                                        }
                                        if(files.at(i).at(0) != '.')
                                                ret_total += s1.st_blocks/2;
                                 }
                                cout << "total " << ret_total << endl;
                        	print_total = false;
			}
                        if(files.at(i).at(0) != '.')
                        {
                                L_flag(dirName + "/" + files.at(i),files.at(i));
                        }
                }
		//check for flags -R
		else if (flag_type&4)
                {
                        R_flag(files, dirName, false, false);
                        break;
                }
		//no flags
		else
		{
			if(files.at(i).at(0) != '.')
			{
				if(s.st_mode & S_IXUSR)
					cout << "\x1b[92m";
				if(S_ISDIR(s.st_mode))
				{
					cout << "\x1b[94m";
					cout << left << files.at(i) << setw(total) << "/";
				}
				else
					cout << left << setw(total) << files.at(i);
				cout << "\x1b[0m";
				align = align - 1;
				if(align <= 0)
				{
					cout << endl;
					align = p;
				}
			}
		}
	}
	cout << endl;
	return 0;
}
