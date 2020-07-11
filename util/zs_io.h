#pragma once


namespace zetscript{
	namespace zs_io{

		bool 	fileExists(const std::string & m_file);
		char * 	readFile(const std::string & filename, int & n_readed_bytes);
		int  	fileLength(const std::string & filename);

	}
}
