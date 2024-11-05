all:
	g++ bss.cpp -o bss
	@echo "Usage: ./bss input.txt"

clean:
	rm -f bss
