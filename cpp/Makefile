all: main

clean:
	rm main

main: src/main.cpp
	g++ -std=c++17 -O3 -o main src/main.cpp

main_prof: src/main.cpp
	g++ -std=c++17 -O2 -pg -o main_prof src/main.cpp
