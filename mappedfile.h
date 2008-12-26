/* mappedfile.h -- provides a class for memory mapped files
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

#ifndef UTIL_MAPPEDFILE_H
#define UTIL_MAPPEDFILE_H

#include <cstddef>
#ifndef NO_EXCEPTIONS
#include <stdexcept>
#endif

namespace util {

/*!
 * MappedFile allows you to create a simple read-only file mapping in an
 * object-oriented cross-platform way.
 */
class MappedFile {
private:
	std::size_t size;
	char *data;
	MappedFile(const MappedFile& x) {}
	MappedFile& operator=(const MappedFile& x) {return *this;}
public:
	/*!
	 * Maps the named file into memory.
	 * The file is mapped into memory. All filehandles are closed afterwards.
	 * \param path path of the file being mapped
	 * \exception IOException the file couldn't be opened
	 */
	MappedFile(const char *path);
	/*!
	 * Unmaps the file and releases all memory.
	 */
	~MappedFile();
	/*!
	 * Get the size of the file in memory.
	 */
	std::size_t length() const { return size; }
	/*!
	 * Gets the nth byte from the mapped file.
	 */
	char operator[](std::size_t n) const { return data[n]; }
	/*!
	 * Gets a read-only pointer to the mapped data.
	 */
	const char* ptr() const { return data; }

#ifndef NO_EXCEPTIONS
	struct IOException : public std::runtime_error {
		IOException(const std::string& message) : std::runtime_error(message) {}
	};
#endif
};

}

#endif
