#pragma once

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

namespace zetscript{
	namespace zs_path{
		std::string  get_directory(const std::string &  _path);
		std::string  get_filename(const std::string &  _path);
		std::string  get_filename_without_extension(const std::string & _path);
	}
}
