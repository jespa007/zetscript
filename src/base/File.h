/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace File{

		bool 		exists(const String & _file);
		Buffer	*readText(const String & _filename);
		int			size(const String & _filename);

	}
}
