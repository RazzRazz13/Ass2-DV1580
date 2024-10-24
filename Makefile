# Compiler and Linking Variables
CC = gcc
CFLAGS = -Wall -fPIC
LIB_MM_NAME = libmemory_manager.so
LIB_LL_NAME = liblinked_list.so
LIB = -pthread -lm

# Source and Object Files
MMSRC = memory_manager.c
LLSRC = linked_list.c
MMOBJ = $(MMSRC:.c=.o)
LLOBJ = $(LLSRC:.c=.o)

# Default target
all: mmanager list test_mmanager test_list

# Rule to create the dynamic library
$(LIB_MM_NAME): $(MMOBJ)
	$(CC) -shared -o $@ $(MMOBJ)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the memory manager
mmanager: $(LIB_MM_NAME)

# Build the linked list
list: $(LLOBJ) $(LIB_MM_NAME)
	$(CC) -shared -o $(LIB_LL_NAME) $(LLOBJ) -L. -lmemory_manager $(LIB)

# Test target to run the memory manager test program
test_mmanager: $(LIB_MM_NAME)
	$(CC) -o test_memory_manager test_memory_manager.c -L. -lmemory_manager $(LIB)

# Test target to run the linked list test program
test_list: $(LIB_LL_NAME) $(LIB_MM_NAME)
	$(CC) -o test_linked_list test_linked_list.c -L. -lmemory_manager -llinked_list $(LIB)

#run tests
run_tests: run_test_mmanager run_test_list
	
# run test cases for the memory manager
run_test_mmanager:
	LD_LIBRARY_PATH=. ./test_memory_manager 0

# run test cases for the linked list
run_test_list:
	LD_LIBRARY_PATH=. ./test_linked_list 0

# Clean target to clean up build files
clean:
	rm -f $(MMOBJ) $(LLOBJ) $(LIB_MM_NAME) $(LIB_LL_NAME) test_memory_manager test_linked_list