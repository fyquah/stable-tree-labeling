all: naive.out raw_pointers.out shared_pointers.out

clean:
	rm a.out

naive.out: naive.cpp
	g++ -O3 -o $@ -g $<

raw_pointers.out: raw_pointers.cpp
	g++ -O3 -o $@ -g $<

shared_pointers.out: shared_pointers.cpp
	g++ -O3 -o $@ -g $<
