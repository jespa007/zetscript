#pragma once

#ifdef _WIN32
#define DLL_FILES "*.dll"
#else
#define DLL_FILES "*.so"
#endif


class CIO_Utils
{


	int m_totalLength;
	unsigned char *buffer;
	std::string  m_filename,m_flags;


public:
	static bool fileExists(const string & m_file);
	//static bool readFile(const string & filename, char *buffer, int length);

	static void writeFile(const string & filename, string & data);
	static ByteBuffer * readFile(const string & filename);


	static int  getLength(const  string  & file);
	static int  getLength(FILE * file);
	static bool setWorkPath(const string & m_path);
	static std::string getCwd();

	static std::string  getFolder(const string & _filename);
	static std::string  getFileName(const string & _filename);
	static std::string  getFileNameWithoutExtension(const string & _filename);

	//static bool	 isDirectory(const string & filename);
	static bool	 createDirectory(const string & filename);

	static std::vector<string > searchFiles(const std::string & folder, const std::string & filter="*");
	//static void getFilenameFromAbsolutePath(string & filename, const string & absolute_path_with_filename);




};
