CC := clang
CFLAGS := -Wall -Wextra -O2

OBJ := obj/main.o obj/stdlib.o obj/utils.o obj/variables.o

ifeq ($(DEBUG),1)
	OBJ := obj/debug.o obj/main.o obj/stdlib.o obj/utils.o obj/variables.o
	CLEAN := clean
	CFLAGS := -Wall -Wextra -O2 -DDEBUG
endif

build/albalang: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o build/albalang -lm

obj/debug.o: src/debug.c
	$(CC) $(CFLAGS) -c src/debug.c -o obj/debug.o

obj/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o obj/main.o

obj/stdlib.o: src/stdlib.c
	$(CC) $(CFLAGS) -c src/stdlib.c -o obj/stdlib.o

obj/utils.o: src/utils.c
	$(CC) $(CFLAGS) -c src/utils.c -o obj/utils.o

obj/variables.o: src/variables.c
	$(CC) $(CFLAGS) -c src/variables.c -o obj/variables.o

clean:
	rm -rf obj/* build/*