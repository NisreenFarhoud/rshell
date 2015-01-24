all:
	test -d bin || mkdir bin
	g++ -Wall -Werror -ansi -pedantic ./src/main.cpp -o ./bin/rshell
rshell:
	test -d bin || mkdir bin
	g++ -Wall -Werror -ansi -pedantic ./src/main.cpp -o ./bin/rshell
clean:
	rm -rf bin
