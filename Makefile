all:
	g++ bss.cpp -o bss
	@echo "Usage: ./bss input.txt"

debug:
	g++ bss.cpp -o bss -g

clean:
	rm -f bss a.out
