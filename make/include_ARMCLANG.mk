CC  = armclang
LINKER = $(CC)

ANSI_CFLAGS  = -ansi
ANSI_CFLAGS += -std=c99
ANSI_CFLAGS += -pedantic
# ANSI_CFLAGS += -Wextra

ifeq ($(strip $(ISA)),ARM)
	ifeq ($(strip $(SIMD)),SVE)
		CFLAGS  =  -Ofast -mcpu=neoverse-n2 -march=armv8.2-a+simd+sve2  $(ANSI_CFLAGS) -fopenmp 
	endif
	ifeq ($(strip $(SIMD)),SVET)
		CFLAGS  =  -Ofast -mcpu=neoverse-n2 -march=armv8.2-a+simd+sve2  $(ANSI_CFLAGS)  -fopenmp
	endif
	ifeq ($(strip $(SIMD)),NEON)
		CFLAGS  = -Ofast -mcpu=neoverse-n2 -march=armv8.2-a+simd+nosve  $(ANSI_CFLAGS) -fopenmp 
	endif
	ifeq ($(strip $(SIMD)),NEONT)
		CFLAGS  = -Ofast -mcpu=neoverse-n2 -march=armv8.2-a+simd+nosve $(ANSI_CFLAGS)  -fopenmp
	endif

endif
# MacOSX with Apple Silicon and homebrew
INCLUDES = -I/opt/homebrew/Cellar/libomp/18.1.8/include
LIBS     = -lm  -L/opt/homebrew/Cellar/libomp/18.1.8/lib -lomp
LFLAGS   = -lm
DEFINES  += -D_GNU_SOURCE 
