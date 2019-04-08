INCLUDES = $(shell find ../ -type f -name "*.o" | grep -v "Adventure" | grep -v "Utils" | sort)

%:
	g++ $@.cpp -std=c++17 $(INCLUDES) -Wall -Wno-sign-compare -o $@.exe
	./$@

clean:
	rm -f *.exe
	rm -f *.gch