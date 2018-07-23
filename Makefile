CXX = c++
CXXFLAGS = -O3 -march=native -std=c++11

all:
	$(CXX) $(CXXFLAGS) SmtCacheBlocking.cpp -o SmtCacheBlocking -lpthread

.PHONY: clean
clean:
	rm -f SmtCacheBlocking
