#pragma once


namespace zetscript{
	namespace zs_file{

		bool 	exists(const std::string & m_file);
		char * 	read(const std::string & filename, size_t & n_readed_bytes);
		int	size(const std::string & filename);

	}
}
