# --- Project Requirement: Makefile ---

# Compiler
CC = gcc

# Compiler flags:
# -Wall = Show all warnings
# -g    = Include debugging information (for GDB)
CFLAGS = -Wall -g

# Libraries to link:
# -lcurl = The libcurl networking library
# -lcjson = The cJSON parsing library
# -lm = The math library (required by cJSON and portfolio.c)
LIBS = -lcurl -lcjson -lm

# All .c files in the project
SRCS = main.c portfolio.c api_handler.c file_handler.c

# All .o (object) files to be created
OBJS = $(SRCS:.c=.o)

# The name of the final executable
TARGET = portfolio_manager

# Default rule: Build everything
all: $(TARGET)

# Rule to link all object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule to compile a .c file into a .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to run the program
run: all
	./$(TARGET)

# Rule to clean up object files and the executable
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean
