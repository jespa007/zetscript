#include "NumberZs.h"

using zetscript::zs_float;
using zetscript::ZetScript;

class Data{
public:
	zetscript::Vector<float> data;
	Data(){
		float n[]={0,1,1,10,3,4,6};
		for(int i=0; i < ZS_ARRAY_SIZE(n); i++){
			data.append(n[i]);
		}
	} 
};

Data *data=NULL;

Data * getData(zetscript::ZetScript *_zs){
	return data;
}

bool DataZs_in(zetscript::ZetScript *_zs, Data *_this, zs_float *_value){
	for(int i=0; i < _this->data.size(); i++){
		if(*_value == _this->data.get(i)){
			return true;
		}
	}
	return false;
}

int main(){

	ZetScript zs;

	data=new Data();

	zs.registerScriptType<Data>("Data");
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


