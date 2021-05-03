cc = gcc
prom = db4
deps = extmem.h
obj = extmem.o main.o linear_search.o tpmms.o

$(prom): $(obj)
	$(cc) -o $(prom) $(obj)

%.o: %.c $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(prom)