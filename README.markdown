MappedFile
==========

MappedFile allows you to create a simple read-only file mapping
in an object-oriented cross-platform way. It may be used to read
*small* files completely in memory without the performance penalty
of `read` syscalls.

Usage
-----
MappedFile is designed to be as minimal as possible and thus
easy to use. The following example shows this by loading a shader
into OpenGL.

	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	try {
		MappedFile map("shader.frag"); // maps "shader.frag" into memory
		// an exception is thrown if the file cannot be opened
	} catch (MappedFile::IOException e) {
		std::cout << e.what() << std::endl;
		std::exit(1);
	}
	// map.length() gets the length in bytes of the mapped file
	// map.ptr() returns a char pointer to the mapped data
	glShaderSource(shader, map.length(), map.ptr(), NULL);
	// the file is automatically unmapped if map goes out of scope

If you don't want to use exceptions define `NO_EXCEPTIONS`. MappedFile will
then call exit(1) if an error occurs.

Supported Operating Systems
---------------------------

At the time of this writing the following OS were supported:

- Windows 95 and higher through `MapViewOfFile`
- POSIX through `mmap`
- Other OS through `malloc` and `fread`

*Note:* The `malloc` backend does not use memory-mapped files and is therefore
substantially slower and may need more memory.

Copyright
---------

Copyright (C) 2008 Benjamin Kramer

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.