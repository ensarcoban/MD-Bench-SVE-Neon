CC  = armclang
LINKER = $(CC)

ANSI_CFLAGS  = -ansi
ANSI_CFLAGS += -std=c99
ANSI_CFLAGS += -pedantic
# ANSI_CFLAGS += -Wextra

ifeq ($(strip $(ISA)),ARM)
CFLAGS  = -Ofast -mcpu=neoverse-n2 -march=armv8.2-a+simd+nosve -Xpreprocessor -fopenmp $(ANSI_CFLAGS)
endif
# MacOSX with Apple Silicon and homebrew
INCLUDES = -I/opt/homebrew/Cellar/libomp/18.1.8/include
LIBS     = -lm  -L/opt/homebrew/Cellar/libomp/18.1.8/lib -lomp
LFLAGS   = -lm
DEFINES  += -D_GNU_SOURCE -DVECTOR_WIDTH=2
