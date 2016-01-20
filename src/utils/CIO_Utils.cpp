
#include "script/zg_script.h"




string  CIO_Utils::getFolder(const string & _filename) {
	size_t found;
	string ss=".";
	found=_filename.find_last_of("/\\");

	if((int)found != -1)
		ss= _filename.substr(0,found);
	return ss;


}

string  CIO_Utils::getFileName(const string & _path) {
  size_t found;
  string ss=_path;
  found=_path.find_last_of("/\\");
  if((int)found != -1)
	  ss= _path.substr(found+1);
  return ss;
}

string  CIO_Utils::getFileNameWithoutExtension(const string & _path) {

	string file = getFileName(_path);


	std::string fName(file);
	    size_t pos = fName.rfind(".");
	    if(pos == std::string::npos)  //No extension.
	        return fName;

	    if(pos == 0)    //. is at the front. Not an extension.
	        return fName;

	    return fName.substr(0, pos);
}
//-----------------------------------------------------------------------------------------------v
// Normal file ops.




bool CIO_Utils::fileExists(const string & m_file) {

	if(m_file == "") return false;

#ifdef _WIN32
	FILE *fp = fopen(m_file.c_str(),"r");

	if(fp == NULL) return false;

	fclose(fp);

	return true;
#else

	if(isDirectory(m_file)) return false;


	struct stat buffer;
	  return (stat (m_file.c_str(), &buffer) == 0);
#endif


}




void CIO_Utils::writeFile(const string & filename, string & data){


	FILE  *fp;

	if((fp  =  fopen(filename.c_str(),"w"))  !=  NULL)
	{

			unsigned n = fwrite((char *)data.c_str(),1, data.size(),fp);

			if(n != data.size()) {
				print_warning_cr("number elements doesn't match with length file (%s)",filename.c_str());
			}

			fclose(fp);

	}
	else  print_error_cr("I can't open file \"%s\"",filename.c_str());
}


int  CIO_Utils::getLength(const  string  & file)
{

	int  ini,  end;
	FILE  *fp;

	if((fp  =  fopen(file.c_str(),"rb"))  !=  NULL)
	{

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);



		fclose(fp);

		return  (end  -  ini);
	}

	return    -1;
}

int  CIO_Utils::getLength(FILE  * file)
{

	int  ini,  end;
	FILE  *fp = file;

	if(fp != NULL)
	{
		unsigned int current = ftell(fp);

		fseek(fp,  0,SEEK_SET);
		ini  =  ftell(fp);
		fseek(fp,  0,SEEK_END);
		end  =  ftell(fp);

		fseek(fp,  current,SEEK_SET);
		//fclose(fp);

		return  (end  -  ini);
	}

	return    -1;
}





