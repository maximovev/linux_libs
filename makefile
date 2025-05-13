all:
	make typedef_test
	make current_time_test
libs:

typedef_test:
	gcc test/typedef_test.cpp -o out/typedef_test.elf
	./out/typedef_test.elf
current_time_test:
	gcc test/current_time_test.cpp -o out/current_time_test.elf
	./out/current_time_test.elf