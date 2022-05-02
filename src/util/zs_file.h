/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace zs_file{

		bool 		exists(const zs_string & m_file);
		zs_string	read_text(const zs_string & filename);
		int			size(const zs_string & filename);

	}
}
