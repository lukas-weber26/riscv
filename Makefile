main: main.c
	gcc -g ./main.c -o test 

fast: main.c
	gcc -Ofast ./main.c -o test 

clang: main.c
	clang -Ofast ./main.c -o test 
