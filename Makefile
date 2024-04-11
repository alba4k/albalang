CC := clang
CFLAGS := -Wall -Wextra -O2

OBJ := obj/main.o obj/utils.o obj/stdlib.o obj/variables.o

build/albalang: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o build/albalang

obj/main.o: src/main.c
	$(CC) $(CLAGS) -c src/main.c -o obj/main.o

obj/utils.o: src/utils.c
	$(CC) $(CLAGS) -c src/utils.c -o obj/utils.o

obj/stdlib.o: src/stdlib.c
	$(CC) $(CLAGS) -c src/stdlib.c -o obj/stdlib.o

obj/variables.o: src/variables.c
	$(CC) $(CLAGS) -c src/variables.c -o obj/variables.o

clean:
	rm -rf obj/* build/*