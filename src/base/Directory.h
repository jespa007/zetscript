/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace Directory{
		bool changeDir(const String & path);
		String getCurrentDirectory();
		bool exists(const String & _dir);
	}

}
