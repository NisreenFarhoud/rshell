##Building RShell

This program will run like a shell using commands through execvp.

##Run program:	cd rshell
		make
		bin/rshell

##Bugs:
1. commands not added
2. connectors not working
3. piping not working

##Built ls Command
Bugs:

1. Running "bin/ls l" will run the command as if input was "bin/ls" instead of giving error for invalid flag

2. Formatting gives random new line at end of some commands in -R

3. Every file is on its own line

4. Alignment needs improvement to account for every case
