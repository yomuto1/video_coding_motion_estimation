VPATH=./src/
EXEC=app_vcme
OBJDIR=./obj/

CC=g++
NVCC=nvcc 
OPTS=-Ofast
LDFLAGS= -lm 
COMMON= 
CFLAGS=-Wall -Wfatal-errors -mfpu=neon -ftree-vectorize

OPTS=-O3 #-g

CFLAGS+=$(OPTS)

OBJ=main.o FS.o app_vcme_common_utils.o

OBJS = $(addprefix $(OBJDIR), $(OBJ))
DEPS = $(wildcard src/*.h) Makefile

all: obj results $(EXEC)

$(EXEC): $(OBJS)
	 $(CC) `pkg-config --cflags opencv` $(COMMON) $(CFLAGS) $^ `pkg-config --libs opencv` -o $@ $(LDFLAGS)

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.cu $(DEPS)
	$(NVCC) $(ARCH) $(COMMON) --compiler-options "$(CFLAGS)" -c $< -o $@

obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(OBJS) $(EXEC)

