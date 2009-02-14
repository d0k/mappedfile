// this program maps its source into memory and outputs it then to stdout

#include "mappedfile.h"
#include <iostream>

int main() {
	MappedFile map(__FILE__); // maps the file into memory

	// write size bytes from the mapped memory to cout
	std::cout.write(map.ptr(), map.size());
	return 0; // the memory is unmapped when map goes out of scope
}
