APP_DATA=data.out
CXXFLAGS=-Wall -std=c++11 -O3

src=$(shell find data/ -type f -name '*.cpp')
src += $(shell find core/ -type f -name '*.cpp')

src_data = $(src) data.cpp
obj_data=$(src_data:.cpp=.o)

data: $(obj_data)
	$(CXX) $^ -o $(APP_DATA)

.PHONY: clean
clean:
	rm -f $(obj_data) $(APP_DATA)
