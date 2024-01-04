#include "NumberZs.h"

class Data{
public:
	zetscript::Vector<float> data;
	Data(){
		float n[]={0,1,1,10,3,4,6};
		for(size_t i=0; i < ZS_ARRAY_SIZE(n); i++){
			data.push(n[i]);
		}
	} 
};

Data *data=NULL;

Data * getData(
	zetscript::ScriptEngine *_script_engine
){
	return data;
}

bool DataZs_in(
	zetscript::ScriptEngine *_script_engine
	, Data *_this
	, zetscript::zs_float *_value
){
	for(int i=0; i < _this->data.length(); i++){
		if(*_value == _this->data.get(i)){
			return true;
		}
	}
	return false;
}

int main(){

	zetscript::ScriptEngine script_engine;

	data=new Data();

	script_engine.registerType<Data>("Data");
	script_engine.registerFunction("getData",getData);
	script_engine.registerMemberFunction<Data>("_in",DataZs_in);

	script_engine.compileAndRun(
		"var data=getData();\n"
		"if(10 in data){\n"
		"	Console::outln(\"10 is content in data\")\n"
		"}\n"
	);


	delete data;

	return 0;
}


