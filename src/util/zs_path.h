/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

namespace zetscript{
	namespace zs_path{
		zs_string  get_directory(const zs_string &  _path);
		zs_string  get_filename(const zs_string &  _path);
		zs_string  get_filename_without_extension(const zs_string & _path);
	}
}
