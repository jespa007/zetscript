/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace zs_file{

		bool 	exists(const zs_string & m_file);
		char * 	read(const zs_string & filename, size_t & n_readed_bytes);
		int	size(const zs_string & filename);

	}
}
