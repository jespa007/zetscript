namespace zetscript{
	namespace zs_io{


		bool 	FileExists(const std::string & m_file);
		char * 	ReadFile(const std::string & filename, int & n_readed_bytes);
		int  	FileLength(const std::string & filename);

	}
}
