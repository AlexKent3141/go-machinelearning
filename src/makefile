APP_DATA=data.out
APP_TRAIN=train.out
CXXFLAGS=-Wall -std=c++14 -O3

src=$(shell find data/ -type f -name '*.cpp')
src += $(shell find core/ -type f -name '*.cpp')

src_data=$(src) data.cpp
obj_data=$(src_data:.cpp=.o)

src_train=$(src) train.cpp
obj_train=$(src_train:.cpp=.o)

data: $(obj_data)
	$(CXX) $^ -o $(APP_DATA)

train: $(obj_train)
	$(CXX) $^ -o $(APP_TRAIN)

.PHONY: clean
clean:
	rm -f $(obj_data) $(obj_train) $(APP_DATA) $(APP_TRAIN)
