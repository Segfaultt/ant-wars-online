CC = g++ -g

C_FLAGS = -w -std=c++14

L_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -pthread

OBJ_NAME = ant_wars_online

OBJS = obj/sdl.o obj/main.o obj/texture_wrapper.o obj/network.o obj/entity.o

all: $(OBJS)
	$(CC) $(L_FLAGS) $(OBJS) -o $(OBJ_NAME)

obj/sdl.o: src/sdl.cpp
	$(CC) $(C_FLAGS) -c src/sdl.cpp -o obj/sdl.o

obj/main.o: src/main.cpp
	$(CC) $(C_FLAGS) -c src/main.cpp -o obj/main.o

obj/texture_wrapper.o: src/texture_wrapper.cpp
	$(CC) $(C_FLAGS) -c src/texture_wrapper.cpp -o obj/texture_wrapper.o

obj/network.o: src/network.cpp
	$(CC) $(C_FLAGS) -c src/network.cpp -o obj/network.o

obj/entity.o: src/entity.cpp
	$(CC) $(C_FLAGS) -c src/entity.cpp -o obj/entity.o

clean:
	rm obj/* 
