/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace zs_dir{
		bool change_dir(const std::string & path);
		std::string get_current_directory();
		bool exists(const std::string & _dir);
	}

}
