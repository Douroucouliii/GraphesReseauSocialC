CC := gcc
SRC_DIR ?= src
OBJ_DIR ?= obj
DEBUG ?= 1

ifeq '$(DEBUG)' '1'
CFLAGS ?= -Wall -MMD -g
else
CFLAGS ?= -Wall -MMD -O3 -DNDEBUG
endif

LDFLAGS=

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

DEPS := $(OBJS:.o=.d)
DEPS_TEST := $(OBJS_TEST:.o=.d)

TARGET ?= exec

.PHONY: clean mrpropre

all: createRep $(TARGET)

createRep:
	@mkdir -p $(OBJ_DIR)/$(SRC_DIR)

$(TARGET): createRep $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

mrpropre: clean
	rm -f $(TARGET) $(TARGET_TEST)

-include $(DEPS)