# Copyright (c) 2008-2013 Luxology LLC

LXSDK = ../LXSDK
LXSDK_INC = $(LXSDK)/include
LXSDK_BUILD = $(LXSDK)/samples/Makefiles/build
SRC_DIR=aaOcean
AAO_INC = dependencies/aaOcean
HELPERS_INC = dependencies/helpers/
DSFMT_HELPERS_INC = dependencies/helpers/dSFMT
KISSFFT_HELPERS_INC = dependencies/helpers/kissfft
OBJ_DIR=Linux/obj
TARGET_DIR=Linux/build
MYCXX = g++
LINK = g++
CXXFLAGS = -std=c++11 -D_OPENMP -fopenmp -g -c -I$(LXSDK_INC) -I$(AAO_INC) -I$(HELPERS_INC) -I$(KISSFFT_HELPERS_INC) -I$(DSFMT_HELPERS_INC) -fPIC -m64 -msse
LDFLAGS = -L$(LXSDK_BUILD) -L/usr/lib -L$(DSFMT_HELPERS_INC) -L$(KISSFFT_HELPERS_INC) -lcommon -lpthread -shared

OBJS = $(KISSFFT_HELPERS_INC)/kiss_fft.o $(KISSFFT_HELPERS_INC)/tools/kiss_fftnd.o $(DSFMT_HELPERS_INC)/dSFMT.o $(AAO_INC)/aaOceanClass.o $(OBJ_DIR)/aaOcean_init.o $(OBJ_DIR)/aaOcean_texture.o $(OBJ_DIR)/aaOcean_chanmod.o $(OBJ_DIR)/aaOcean_deformer.o $(OBJ_DIR)/aaOcean_command.o
TARGET = $(TARGET_DIR)/aaocean.lx

all: $(TARGET)

lxsdk:
	cd $(LXSDK)/samples/Makefiles/common; make

kissfft:
	cd $(KISSFFT_HELPERS_INC); $(MYCXX) $(CXXFLAGS) kiss_fft.cpp; cd tools;$(MYCXX) $(CXXFLAGS) kiss_fftnd.cpp

dsfmt:
	cd $(DSFMT_HELPERS_INC); $(MYCXX) $(CXXFLAGS) dSFMT.cpp

$(OBJ_DIR):
	mkdir $@

.PRECIOUS :$(OBJ_DIR)/%.o 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(MYCXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ_DIR) lxsdk kissfft dsfmt $(OBJS)
	$(LINK) -o $@ $(OBJS) $(LDFLAGS)

clean:
	cd $(LXSDK)/samples/Makefiles/common; make clean
	rm -rf $(TARGET) $(OBJ_DIR)
