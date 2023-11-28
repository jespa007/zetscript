/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class Console{
	public:

		static int getline(char **lineptr, size_t *n, FILE *stream);
		static char readChar();
	};
}
