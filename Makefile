CXX = g++
CFLAGS = -Wall -Werror -ggdb3
OBJ = test.o

main: $(OBJ) physics.hpp
	$(CXX) $(CFLAGS) $(OBJ) -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

clean:
	rm *.o main

run: main
	./main

debug: main
	valgrind --leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--verbose \
			--log-file=valgrind-out.txt \
			./main