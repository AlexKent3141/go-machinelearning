APP_VISUAL=visual.out
CXXFLAGS=-Wall -std=c++14 -O3
LDFLAGS=-lSDL

src=$(shell find ../data/ -type f -name '*.cpp')
src += $(shell find ../core/ -type f -name '*.cpp')
src += $(wildcard *.cpp)

src_visual=$(src)
obj_visual=$(src_visual:.cpp=.o)

visual: $(obj_visual)
	$(CXX) $^ -o $(APP_VISUAL) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj_visual) $(APP_VISUAL)
