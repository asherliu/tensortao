exe = graph_loader.bin 

cc = "$(shell which g++)" 
#flags = -I. -fopenmp -march=athlon64 -O3
flags = -I. -O3
#flags += -std=c++11

ifeq ($(debug), 1)
	flags+= -DDEBUG 
endif

objs = $(patsubst %.cpp,%.o,$(wildcard ../../lib/*.cpp)) \
			$(patsubst %.cpp,%.o,$(wildcard *.cpp))

deps = $(wildcard ./*.hpp) \
				$(wildcard *.h) \
				Makefile

%.o:%.cpp $(deps)
	$(cc) -c $< -o $@ $(flags)

$(exe):$(objs)
	$(cc) $(objs) -o $(exe) $(flags)

test:$(exe)
	./cpu_ibfs /home/hang/scale_16/beg_16_16.bin /home/hang/scale_16/csr_16_16.bin 64 128 2 

clean:
	rm -rf $(exe) $(objs) 
