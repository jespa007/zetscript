/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace zs_file{

		bool 		exists(const zs_string & _file);
		zs_buffer	*read_text(const zs_string & _filename);
		int			size(const zs_string & _filename);

	}
}
