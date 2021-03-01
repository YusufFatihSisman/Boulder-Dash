OBJS = BoulderDash.c

CC = gcc

COMPILER_FLAGS = -w

LINKER_FLAGS = SDL2.dll SDL2_image.dll SDL2_mixer.dll

OBJ_NAME = BoulderDash

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) 
	./BoulderDash.exe

