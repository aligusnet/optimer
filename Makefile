INCLUDE = -Iinclude -Isrc
LIB = -Llib
SRC = src/main.cpp src/data_loader.cpp src/tsp.cpp src/stats.cpp
TESTS_SRC = tests/main.cpp tests/ap.cpp tests/atsp.cpp tests/teamcity_cppunit.cpp tests/teamcity_messages.cpp src/data_loader.cpp src/tsp.cpp src/stats.cpp

all:
	clang++ $(INCLUDE) --std=c++11 --stdlib=libc++ -g -o optimer $(SRC)

clang:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang $(SRC)

clang-33:
	clang++-mp-3.3 --std=c++11 --stdlib=libc++ -O2 -o optimer-clang++-mp-3.3 $(SRC)

gcc-47:
	g++-mp-4.7 --std=c++11 -O2 -o optimer-g++-mp-4.7 $(SRC)

tst:
	g++-mp-4.7 $(INCLUDE) $(LIB) -lcppunit --std=c++11 -O2 -o optimer-tests $(TESTS_SRC)

clean-all:
	rm -rf optimer* bin include lib share

cpplint:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" `find src` 2>&1
