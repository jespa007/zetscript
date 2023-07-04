
#include "NumberZs.h"

class Data{
public:
	std::vector<float> data;
	Data(){
		this.data={0,1,1,10,3,4,6}
	} 
};

Data *DataZs_new(zetscript::ZetScript *_zs){
	return new Number();
}

bool DataZs_in(zetscript::ZetScript *_zs, zs_float *_value){
	for(auto d in data){
		if(d == *_value){
			return true;;
		}
	}
	return false;
}

void DataZs_delete(zetscript::ZetScript *_zs, Data *_this){
	delete _this;
}



int main(){

	ZetScript zs;

	// Register 'Data' type
	zs.registerType<Data>("Data",DataZs_new,DataZs_delete);

	// Register '_in' metamethod
	zs.registerMemberFunction<Data>("_in",DataZs_in);


	// Evaluates a script that instances "Number" with 10.5 and later it adds 20
	zs.eval(
		"var data=new Data();\n"
		"if(10 in data){\n"
		"	Console::outln(\"10 is content in data\")\n"
		"}\n"
	);

	return 0;
};


