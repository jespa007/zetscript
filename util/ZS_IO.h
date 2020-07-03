namespace ZetScript{
	namespace zs_io{


		bool 	file_exists(const std::string & m_file);
		char * 	read_file(const std::string & filename, int & n_readed_bytes);
		int  	file_length(const std::string & filename);

	}
}
