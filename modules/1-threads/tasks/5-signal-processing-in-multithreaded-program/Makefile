CC = gcc
LDFLAGS = -lpthread

SRC_DIR = src
BLD_DIR = build

define compile
$(CC) $< -o $(BLD_DIR)/$(1) $(LDFLAGS)
endef


all: ab c d

ab c d: | $(BLD_DIR)

ab: $(SRC_DIR)/ab-three-threads.c
	$(call compile,ab-tt)

c: $(SRC_DIR)/c-additional-4th-thread.c
	$(call compile,c-a4t)

d: $(SRC_DIR)/d-two-threads-process-sigint.c
	$(call compile,d-ttps)


$(BLD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BLD_DIR)


.PHONY: all $(BLD_DIR) clean
