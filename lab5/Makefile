all:
	g++ -fPIC -c func1.cpp -o d1.o
	g++ -fPIC -c func2.cpp -o d2.o

	g++ -shared -o libd1.so d1.o
	g++ -shared -o libd2.so d2.o

	g++ prog2.cpp -L. -ldl -o main2.out -Wl,-rpath -Wl,.
	g++ prog1.cpp -L. -ld1 -o main1.out -Wl,-rpath -Wl,.