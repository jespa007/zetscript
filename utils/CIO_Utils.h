/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class CIO_Utils
	{

	public:


		static string  getFileName(const string &  _path);
		static string  getFileNameWithoutExtension(const string & _path);
		static bool fileExists(const string & m_file);
		//static bool readFile(const string & filename, char *buffer, int length);
		static char * readFile(const string & filename, int & n_readed_bytes);
		static int  getLength(const string & filename);
	};

}
