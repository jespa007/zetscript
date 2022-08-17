/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace zs_file{

		bool 		exists(const std::string & _file);
		zs_buffer	*read_text(const std::string & _filename);
		int			size(const std::string & _filename);

	}
}
