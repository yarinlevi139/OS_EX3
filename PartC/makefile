proactor_server: proactor_server.o
	gcc -o proactor_server proactor_server.o -L../PartB -lproactor


proactor_server.o : proactor_server.c
	gcc -c proactor_server.c


clean:
	rm -f proactor_server proactor_server.o
