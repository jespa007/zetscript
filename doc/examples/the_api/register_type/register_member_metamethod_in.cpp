#include "NumberZs.h"

using zetscript::zs_float;
using zetscript::ZetScript;

class Data{
public:
	std::vector<float> data;
	Data(){
		this->data={0,1,1,10,3,4,6};
	} 
};

Data *data=NULL;

Data * getData(zetscript::ZetScript *_zs){
	return data;
}

bool DataZs_in(zetscript::ZetScript *_zs, Data *_this, zs_float *_value){
	for(auto d : _this->data){
		if(d == *_value){
			return true;;
		}
	}
	return false;
}

int main(){

	ZetScript zs;

	data=new Data();

	zs.registerType<Data>("Data");
	zs.registerFunction("getData",getData);
	zs.registerMemberFunction<Data>("_in",DataZs_in);

	zs.eval(
		"var data=getData();\n"
		"if(10 in data){\n"
		"	Console::outln(\"10 is content in data\")\n"
		"}\n"
	);


	delete data;

	return 0;
}


