exe = text_to_bin.bin

cc = "$(shell which g++)" 
flags = -fopenmp 

ifeq ($(debug), 1)
	flags += -O0 -g
else
	flags += -O3
endif

objs = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

deps =$(wildcard *.h) \
				Makefile

%.o:%.cpp $(deps)
	$(cc) -c $< -o $@ $(flags)

$(exe):$(objs)
	$(cc) $(objs) -o $(exe) $(flags)

clean:
	rm -rf $(exe) $(objs)
