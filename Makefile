multi:
	python3 ./src/make_graph.py
	g++ -O3 ./src/ld0.cpp -o ./bin/ld0
	g++ -O3 ./src/ld1.cpp -o ./bin/ld1
	g++ -O3 ./src/ld2.cpp -o ./bin/ld2
	g++ -O3 ./src/ld3.cpp -o ./bin/ld3
	g++ -O3 ./src/ld4.cpp -o ./bin/ld4
	g++ -O3 ./src/ld5.cpp -o ./bin/ld5
	g++ -O3 ./src/ld6.cpp -o ./bin/ld6
	g++ -O3 ./src/ld7.cpp -o ./bin/ld7
