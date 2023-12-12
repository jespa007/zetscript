#include "NumberZs.h"

using zetscript::zs_float;
using zetscript::ScriptEngine;

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

Data * getData(zetscript::ScriptEngine *_se){
	return data;
}

bool DataZs_in(zetscript::ScriptEngine *_se, Data *_this, zs_float *_value){
	for(int i=0; i < _this->data.length(); i++){
		if(*_value == _this->data.get(i)){
			return true;
		}
	}
	return false;
}

int main(){

	ScriptEngine se;

	data=new Data();

	se.registerScriptType<Data>("Data");
	se.registerFunction("getData",getData);
	se.registerMemberFunction<Data>("_in",DataZs_in);

	se.compileAndRun(
		"var data=getData();\n"
		"if(10 in data){\n"
		"	Console::outln(\"10 is content in data\")\n"
		"}\n"
	);


	delete data;

	return 0;
}


