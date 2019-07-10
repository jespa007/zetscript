namespace zs{
	namespace path{
		std::string  CZetScriptUtils::getFileName(const std::string & _path) {
		  size_t found;
		  std::string ss=_path;
		  found=_path.find_last_of("/\\");
		  if((int)found != -1)
			  ss= _path.substr(found+1);
		  return ss;
		}

		std::string  CZetScriptUtils::getFileNameWithoutExtension(const std::string & _path) {

			std::string file = getFileName(_path);


			std::string fName(file);
				size_t pos = fName.rfind(".");
				if(pos == std::string::npos)  //No extension.
					return fName;

				if(pos == 0)    //. is at the front. Not an extension.
					return fName;

				return fName.substr(0, pos);
		}


	}
}
