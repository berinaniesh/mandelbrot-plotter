CXX = clang++
CFLAGS = ""

debug:
	$(CXX) src/main.cpp -o bin/debug/mandelbrot

release:
	$(CXX) src/main.cpp -OFast -o bin/release/mandelbrot

