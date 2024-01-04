/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	class Path{
	public:
		static String  getDirectory(const String &  _path);
		static String  getFilename(const String &  _path);
		static String  getFilenameWithoutExtension(const String & _path);
	};
}
