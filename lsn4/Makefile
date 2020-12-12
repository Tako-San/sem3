all: fork fork_c sleep time

fork: myfork.c
		gcc -g -Wall -Wextra myfork.c -o fork
fork_c: myfork_chain.c
		gcc -g -Wall -Wextra myfork_chain.c -o fork_c
sleep: slp.c
		gcc -g -Wall -Wextra slp.c -o slp
time: mytime.c
		gcc -g -Wall -Wextra -lrt mytime.c -o tm
