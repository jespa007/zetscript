namespace zs{
	namespace io{


	static bool fileExists(const std::string & m_file);
	//static bool readFile(const std::string & filename, char *buffer, int length);
	static char * readFile(const std::string & filename, int & n_readed_bytes);
	static int  getFileLength(const std::string & filename);

	}
}
