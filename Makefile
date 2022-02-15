CXXFLAGS=-g -std=c++11
CXX=g++

test_tokenizer: tokenizer.hpp errors_warnings.cpp tokenizer.cpp test_tokenizer.cpp
	$(CXX) $(CXXFLAGS) errors_warnings.cpp tokenizer.cpp test_tokenizer.cpp -o test_tokenizer

test_linker: tokenizer.hpp linker.hpp errors_warnings.cpp tokenizer.cpp linker.cpp test_linker.cpp
	$(CXX) $(CXXFLAGS) errors_warnings.cpp tokenizer.cpp linker.cpp test_linker.cpp -o test_linker

clean:
	rm -f *.o
	rm -f test_tokenizer
	rm -f test_linker