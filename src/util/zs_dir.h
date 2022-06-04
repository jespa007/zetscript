/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace zs_dir{
		bool change_dir(const zs_string & path);
		zs_string get_current_directory();
		bool exists(const zs_string & _dir);
	}

}
