exe = multi_bin_to_2d_csr.bin

cc = "$(shell which g++)" 

ifeq ($(debug), 1)
	flags = -O0 -g  -I. -fopenmp 
else
	flags = -O3 -I. -fopenmp 
endif

objs =$(patsubst %.cpp,%.o,$(wildcard *.cpp))

deps =	$(wildcard *.h) \
				Makefile

%.o:%.cpp $(deps)
	$(cc) -c $< -o $@ $(flags)

$(exe):$(objs)
	$(cc) $(objs) -o $(exe) $(flags)

clean:
	rm -rf $(exe) $(objs)
