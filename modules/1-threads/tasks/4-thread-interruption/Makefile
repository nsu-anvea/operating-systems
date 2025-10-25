CC = gcc
LDFLAGS = -lpthread

SRC_DIR = src
BLD_DIR = build

define compile
$(CC) $< -o $(BLD_DIR)/$(1) $(LDFLAGS)
endef


all: a b c d

a b c d: | $(BLD_DIR)

a: $(SRC_DIR)/a-thread-print-in-while.c
	$(call compile,a-tpiw)

b: $(SRC_DIR)/b-pthread-testcancel.c
	$(call compile,b-pt)

c: $(SRC_DIR)/c-cleanup-push-pop.c
	$(call compile,c-cpp)

d: $(SRC_DIR)/d-signal-handler.c
	$(call compile,d-sh)


$(BLD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BLD_DIR)

.PHONY: all clean
