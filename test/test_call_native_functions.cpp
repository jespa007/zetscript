/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

class ClassA{
	public:
	 void fun1(const std::string & _s,bool _show_print){
		 if(_show_print){
			 printf("from A: %s\n",_s.c_str());
		 }
	 }
};

class ClassB:public ClassA{
public:
    void fun1(float _f,bool _show_print){
    	if(_show_print){
    		printf("from B: %.02f\n",_f);
    	}
    }
};


class ClassD{};
class ClassC:public ClassB{
public:
	ClassD d;
	int num;
	ClassC(){
		num=0;
	}

    void fun1(bool _b,bool _show_print){
    	if(_show_print){
    		printf("from C: %s\n",_b?"true":"false");
    	}
    }
};


struct ParamA{zetscript::zs_string _s;ParamA(){_s="";}};
struct ParamB{zetscript::zs_float _f;ParamB(){_f=0;}};

ClassA *ClassAWrap_new(zetscript::ZetScript *_zs){
	return new ClassA;
}

void ClassAWrap_fun1(zetscript::ZetScript *_zs, ClassA *_this, ParamA *_param_a,bool *_show_print){
   _this->fun1(_param_a->_s.c_str(),*_show_print);
}

void ClassAWrap_delete(zetscript::ZetScript *_zs,ClassA *_this){
	delete _this;
}


ClassB *ClassBWrap_new(zetscript::ZetScript *_zs){
	return new ClassB;
}

void ClassBWrap_fun1(zetscript::ZetScript *_zs, ClassB *_this, ParamB *_param_b,bool *_show_print){
  _this->fun1(_param_b->_f,*_show_print);
}

void ClassBWrap_delete(zetscript::ZetScript *_zs,ClassB *_this){
	delete _this;
}

ClassC *ClassCWrap_new(zetscript::ZetScript *_zs){
	return new ClassC;
}

void ClassCWrap_fun1(zetscript::ZetScript *_zs, ClassC *_c, bool *_b,bool *_show_print){
  _c->fun1(*_b,*_show_print);
}

void ClassCWrap_num_setter(zetscript::ZetScript *_zs, ClassC *_c, zetscript::zs_int _num){
  _c->num=_num;
}

zetscript::zs_int ClassCWrap_num_getter(zetscript::ZetScript *_zs, ClassC *_c){
  return _c->num;
}

ClassD * ClassCWrap_get_d(zetscript::ZetScript *_zs, ClassC *_c){
  return &_c->d;
}


void ClassCWrap_delete(zetscript::ZetScript *_zs,ClassC *_this){
	delete _this;
}

//-------
// PARAMS

ParamA *ParamAWrap_new(zetscript::ZetScript *_zs){
	return new ParamA;
}


void ParamAWrap_delete(zetscript::ZetScript *_zs,ParamA *_this){
	delete _this;
}

ParamB *ParamBWrap_new(zetscript::ZetScript *_zs){
	return new ParamB;
}


void ParamBWrap_delete(zetscript::ZetScript *_zs,ParamB *_this){
	delete _this;
}

//------------------------------
// TEST NATIVE CALL FUNCTION MEMBER

#define MAX_CLASSES 6

void test_bug(){
	int iterations=5;
	zetscript::ZetScript zs;

	const char *str_classes[MAX_CLASSES]={
		"main"
		,"intro"
		,"tutorial"
		,"task"
		,"end"
		,"end1"
	};

	std::function<void ()> *fun[MAX_CLASSES]={NULL};

	do{
		zs.clear();

		for(int i=0; i < MAX_CLASSES; i++){

			zs.eval(zetscript::zs_strutils::format(
				"class c%s{\n"
					"c%s(){\n"
					"	this.self=getScene(\"%s\")\n"
					"}\n"
				"}\n"
				,str_classes[i]
				,str_classes[i]
				,str_classes[i]

			));
		}

		for(int i=0; i < MAX_CLASSES; i++){
			zs.eval(zetscript::zs_strutils::format(
				"	function c%s::load(){\n"
				"	}\n"
				,str_classes[i])
			);
		}

		// add  function
		for(int i=0; i < MAX_CLASSES; i++){
			zs.eval(zetscript::zs_strutils::format(
				"	function c%s::ini(){\n"
				//"		Console::outln(\"ok\")\n"
				"		asdasdsa;\n"
				"	}\n"
				,str_classes[i])
			);
		}



		/*zs.eval(
			"//---- CONSTANTS\n"
			"var BL_MAX_TRIALS= 60;\n"
			"var N_TRIALS_BASELINE=2.0;\n"
			"\n"
			"//-----------------------------------------\n"
			"// KEYS\n"
			"var T_SPACE = 32;\n"
			"var T_B 	= 98;\n"
		);*/


		for(int i=0; i < MAX_CLASSES; i++){

			if(i==MAX_CLASSES-1){
				int hh=0;
				hh++;
			}

			zs.eval(zetscript::zs_strutils::format(
				"var %s=new c%s();\n"
				"%s.load();"
				,str_classes[i]
				,str_classes[i]
				,str_classes[i]
			));
		}

		//zs.printGeneratedCode();

		for(int i=0; i < MAX_CLASSES; i++){
			fun[i]=new std::function<void ()>(zs.bindScriptFunction<void()>(
					zetscript::zs_strutils::format(
						"%s.ini"
						,str_classes[i]
					)
			));
		}

		for(int i=0; i < MAX_CLASSES; i++){
			try{
				(*fun[i])();
			}catch(std::exception & ex){
				fprintf(stderr,"err:%s\n",ex.what());
			}
		}

		for(int i=0; i < MAX_CLASSES; i++){
			delete fun[i];
			fun[i]=NULL;
		}

	}while(--iterations);


}

//------------------------------
// TEST NATIVE CALL FUNCTION MEMBER

void test_call_function_member(zetscript::ZetScript *_zs, bool _show_print=true){

	_zs->eval(
		zetscript::zs_strutils::format(
				"var c=new ClassC();\n"
				"c.fun1(new ParamA(),%s);\n"
				"c.num=10;\n"
				"Console::outln(c);\n"
				//"c.get_d.x=0;\n"
				//"(new ClassC()).fun1(1.5,%s)\n"
				//"(new ClassC()).fun1(false,%s)\n"
				,_show_print?"true":"false"
				,_show_print?"true":"false"
				,_show_print?"true":"false"
		)
	);

}

//------------------------------
// TEST NATIVE CALL WITH NULLS
void test_native_function_with_nulls(
		zetscript::ZetScript *_zs
		,zetscript::zs_int _p1
		,zetscript::zs_float *_p2
		,bool *_p3
		,ClassA *_p4
){
	if(_p1 != 0){std::runtime_error("_p1 not 0");}
	if(_p2 != NULL){std::runtime_error("_p2 not NULL");}
	if(_p3 != NULL){std::runtime_error("_p3 not NULL");}
	if(_p4 != NULL){std::runtime_error("_p4 not NULL");}

}

void test_call_native_function_with_nulls(zetscript::ZetScript *_zs){


	_zs->eval(
		// calls C function 'test_native_function_with_nulls' passing all parameters as null.
		"test_native_function_with_nulls(\n"
			"null\n"
			",null\n"
			",null\n"
			",null\n"
		");\n"
		""
	);
}


void test_call_native_function(zetscript::ZetScript *_zs, bool _show_print=true){

	auto a=_zs->bindType<ClassA>("ClassA",ClassAWrap_new,ClassAWrap_delete);
	auto b=_zs->bindType<ClassB>("ClassB",ClassBWrap_new,ClassBWrap_delete);
	auto c=_zs->bindType<ClassC>("ClassC",ClassCWrap_new,ClassCWrap_delete);
	auto d=_zs->bindType<ClassD>("ClassD");

	_zs->bindType<ParamA>("ParamA",ParamAWrap_new,ParamAWrap_delete);
	_zs->bindType<ParamB>("ParamB",ParamBWrap_new,ParamBWrap_delete);


	_zs->bindMemberFunction<ClassA>("fun1",ClassAWrap_fun1);

	_zs->extendsFrom<ClassB,ClassA>();

	_zs->bindMemberFunction<ClassB>("fun1",ClassBWrap_fun1);


	_zs->extendsFrom<ClassC,ClassB>();

	_zs->bindMemberPropertySetter<ClassC>("num",ClassCWrap_num_setter);
	_zs->bindMemberPropertyGetter<ClassC>("num",ClassCWrap_num_getter);
	_zs->bindMemberFunction<ClassC>("get_d",ClassCWrap_get_d);
	//_zs->extendsFrom<ClassC,ClassB>();

	//_zs->bindMemberFunction<ClassC>("fun1",ClassCWrap_fun1);


	_zs->bindFunction("test_native_function_with_nulls",test_native_function_with_nulls);

	//a->printListMemberFunctions();

	//b->printListMemberFunctions();

	//c->printListMemberFunctions();


	test_call_native_function_with_nulls(_zs);

	test_call_function_member(_zs,_show_print);

}



#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		//test_call_native_function(&zs);
		test_bug();
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}
	return 0;
}
#endif


