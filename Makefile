
# target file name
TARGET 	:= zdia


DIR_BIN := bin
DIR_INC :=
DIR_LIB :=


CC     	:= gcc
CFLAGS 	:= -Wall -Werror -O0 -g
LDFLAGS :=
 

SRC 	:= test.c \
           server.c \
	       conn.c \
	       array.c \
	       list.c \
	       queue.c \
	       receiver.c \
	       worker.c \
	       tcp.c \
	       common.c \
	       avp.c \
	       msg.c \
           # don't delete this line

OBJ 	:= $(SRC:%.c=%.o)
 
LIB		:= -lpthread -lexpat

%.o: %.c
	$(CC) $(CFLAGS) $(DIR_INC) -c $< -o $(DIR_BIN)/$@
 
.PHONY: all clean
 
all: $(DIR_BIN) $(TARGET)
 
$(TARGET): $(OBJ) 
	$(CC) $(LDFLAGS) $(DIR_LIB) $(LIB) $(addprefix $(DIR_BIN)/, $^) -o $(DIR_BIN)/$(TARGET)
	@echo build complete!
	@echo --------------
 
$(DIR_BIN):
	@mkdir -p $@
 
clean:
	@rm -rf $(DIR_BIN)
	@echo cleaned.
