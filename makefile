CCPP = g++

all:
	make typedef_test
	make current_time_test
	make xml_io_test
	make neural

typedef_test:
	gcc test/typedef_test.cpp -o out/typedef_test.elf
	./out/typedef_test.elf

current_time_test:
	gcc test/current_time_test.cpp -o out/current_time_test.elf
	./out/current_time_test.elf

lib_setup:
	apt-get install libtinyxml2-dev libboost-dev libboost-program-options-dev
	cd /usr/include/libxml2
	cp -R libxml/ ../

xml_io_test:
	g++ -std=c++11 -D __USE_XOPEN2K8 -c test/xml_io_test.cpp -o out/xml_io_test.elf -lxml2

neural:
	clear
	$(CCPP) test/neural_test.cpp -o out/neural_test.elf