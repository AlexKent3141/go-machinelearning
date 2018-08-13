GPU=0

APP_DATA=data.out
APP_TRAIN=train.out
APP_VISUAL=visual.out
CXXFLAGS=-Wall -std=c++14 -O3
LDFLAGS=-lpthread -Llib -ldarknet

ifeq ($(GPU), 1)
LDFLAGS+= -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -lcurand
endif

src=$(shell find src/data/ -type f -name '*.cpp')
src+= $(shell find src/core/ -type f -name '*.cpp')

src_data=$(src) src/data.cpp
obj_data=$(src_data:.cpp=.o)

src_train=$(src) src/train.cpp
obj_train=$(src_train:.cpp=.o)

src_visual=$(src) src/visual.cpp
src_visual+= $(shell find src/visual/ -type f -name '*.cpp')
obj_visual=$(src_visual:.cpp=.o)

data: $(obj_data)
	$(CXX) $^ -o $(APP_DATA)

train: $(obj_train)
	$(CXX) $^ -o $(APP_TRAIN) $(LDFLAGS)

visual: $(obj_visual)
	$(CXX) $^ -o $(APP_VISUAL) $(LDFLAGS) -lSDL

.PHONY: clean
clean:
	rm -f $(obj_data) $(obj_train) $(APP_DATA) $(APP_TRAIN) $(APP_VISUAL)
