#pragma once

namespace zetscript{
	namespace zs_dir{
		bool chdir(const std::string & path);
		std::string get_current_directory();
	}

}
