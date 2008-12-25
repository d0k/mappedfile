/* mappedfile.cpp -- provides a class for memory mapped files
 *
 * Copyright (C) 2008 Benjamin Kramer
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#include "mappedfile.h"
#include <sstream>

#ifndef HAVE_MMAP
#if defined(__linux__) || defined(BSD) || defined(__APPLE__) || defined(__SVR4) // linux, bsd, OSX, solaris
#define HAVE_MMAP 1
#endif
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif HAVE_MMAP
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#else
#include <stdio.h>
#endif

namespace util {

MappedFile::MappedFile(const char *path) {
#ifdef _WIN32
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::ostringstream oss;
		oss << "Couldn't open File \"" << path << '"';
		throw IOException(oss.str());
	}
	size = GetFileSize(hFile, NULL);
	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, size, NULL);
	data = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, size);
	CloseHandle(hMap);
	CloseHandle(hFile);
#elif HAVE_MMAP
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		std::ostringstream oss;
		oss << "Couldn't open File \"" << path << '"';
		throw IOException(oss.str());
	}
	size = lseek(fd, 0, SEEK_END);
	data = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);
#else
	FILE *fd = fopen(path, "r");
	if (fd == NULL) {
		std::ostringstream oss;
		oss << "Couldn't open File \"" << path << '"';
		throw IOException(oss.str());
	}
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	rewind(fd);
	data = (char*)malloc(size);
	fread(data, size, 1, fd);
	fclose(fd);
#endif
}

MappedFile::~MappedFile() {
#ifdef _WIN32
	UnmapViewOfFile(data);
#elif HAVE_MMAP
	munmap(data, size);
#else
	free(data);
#endif
}

}