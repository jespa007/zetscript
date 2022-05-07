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


class Num{
public:
	int x,y;

	Num(){
		x=0;
		y=0;
	}
};

Num *num_ref=NULL;

class ClassD{};
class ClassC:public ClassB{
public:
	ClassD d;
	Num num;
	ClassC(){
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
	ZS_UNUSUED_PARAM(_zs);
	return new ClassA;
}

void ClassAWrap_fun1(zetscript::ZetScript *_zs, ClassA *_this, ParamA *_param_a,bool *_show_print){
	ZS_UNUSUED_PARAM(_zs);
	_this->fun1(_param_a->_s.c_str(),*_show_print);
}

bool ClassAWrap_lt(zetscript::ZetScript *_zs, ClassA *_n1, ClassA *_n2){
	ZS_UNUSUED_PARAM(_zs);
	return true;
}

bool ClassAWrap_lt(zetscript::ZetScript *_zs, ClassA *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_PARAM(_zs);
	printf("from float 1!\n");
	return true;
}

bool ClassAWrap_lt(zetscript::ZetScript *_zs, zetscript::zs_float *_n1, ClassA *_n2){
	ZS_UNUSUED_PARAM(_zs);
	printf("from float 2!\n");
	return true;
}


void ClassAWrap_delete(zetscript::ZetScript *_zs,ClassA *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}


ClassB *ClassBWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ClassB;
}

void ClassBWrap_fun1(zetscript::ZetScript *_zs, ClassB *_this, ParamB *_param_b,bool *_show_print){
	ZS_UNUSUED_PARAM(_zs);
	_this->fun1(_param_b->_f,*_show_print);
}

void ClassBWrap_delete(zetscript::ZetScript *_zs,ClassB *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

ClassC *ClassCWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ClassC;
}

void ClassCWrap_fun1(zetscript::ZetScript *_zs, ClassC *_c, bool *_b,bool *_show_print){
	ZS_UNUSUED_PARAM(_zs);
	_c->fun1(*_b,*_show_print);
}

/*void ClassCWrap_num_setter(zetscript::ZetScript *_zs, ClassC *_c, zetscript::zs_int _num){
	ZS_UNUSUED_PARAM(_zs);
	_c->num=_num;
}*/

Num * ClassCWrap_num_getter(zetscript::ZetScript *_zs, ClassC *_c){
	ZS_UNUSUED_PARAM(_zs);
	return &_c->num;
}

Num *ClassCWrap_newNum(zetscript::ZetScript *_zs,ClassC *_c){
	return num_ref;
}

zetscript::zs_int NumWrap_x_getter(zetscript::ZetScript *_zs, Num *_n){
	ZS_UNUSUED_PARAM(_zs);
	return _n->x;
}

void NumWrap_pre_increment(zetscript::ZetScript *_zs, Num *_n){
	++_n->x;
	++_n->y;
}

void NumWrap_pre_decrement(zetscript::ZetScript *_zs, Num *_n){
	--_n->x;
	--_n->y;
}


ClassD * ClassCWrap_get_d(zetscript::ZetScript *_zs, ClassC *_c){
	ZS_UNUSUED_PARAM(_zs);
	return &_c->d;
}

void ClassCWrap_delete(zetscript::ZetScript *_zs,ClassC *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

//-------
// PARAMS

ParamA *ParamAWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ParamA;
}


void ParamAWrap_delete(zetscript::ZetScript *_zs,ParamA *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

ParamB *ParamBWrap_new(zetscript::ZetScript *_zs){
	ZS_UNUSUED_PARAM(_zs);
	return new ParamB;
}


void ParamBWrap_delete(zetscript::ZetScript *_zs,ParamB *_this){
	ZS_UNUSUED_PARAM(_zs);
	delete _this;
}

//------------------------------
// TEST NATIVE CALL FUNCTION MEMBER

void test_call_function_member(zetscript::ZetScript *_zs, bool _show_print=true){

	Num num;
	num_ref=&num;

	for(int i=0; i < 10; i++){

		_zs->clear();

		try{
		_zs->eval(
			zetscript::zs_strutils::format(
					"(new ClassA()) < 1"
					//"import \"include.zs\"\n"
					/*"function class_c_load(_class_c){\n"
						"_class_c.num_ref=_class_c.newNum()\n"
						//"_class_c.num_ref.load()\n"
					"}\n"
					"class ClassCWrap{\n"
						"constructor(_this){\n"
							"this.self=_this;\n"
						"}\n"
						"load(){\n"
							"class_c_load(this)\n"
							//"Console::outln(\"load\")"
						"}\n"
						"newNum()\n{"
							"return this.self.newNum();\n"
						"}\n"
						"ini(n){\n"
							"ini2(n)\n"
							//"n.load()"
							//"Console::outln()"
						"}\n"
					"}\n"
					"function ini2(n){n.load()}"
					"var c=new ClassCWrap(new ClassC())\n"
					/*"var c=new ClassC();\n"
					"c.fun1(new ParamA(),%s);\n"
					"Console::outln(\"decrement\");\n"
					//"c.num.x=10;\n"
					"c.num;\n"
					//"c.num.x.y.z.t==10;\n"
					"Console::outln(c);\n"
					"c.load()\n"*/
					//"c.get_d.x=0;\n"
					//"(new ClassC()).fun1(1.5,%s)\n"
					//"(new ClassC()).fun1(false,%s)\n"
					,_show_print?"true":"false"
					,_show_print?"true":"false"
					,_show_print?"true":"false"
			)
		//,zetscript::EvalOption::EVAL_OPTION_SHOW_USER_BYTE_CODE

		);

		//_zs->clear();

		}catch(std::exception & ex){
			fprintf(stderr,"%s\n",ex.what());
		}

		/*auto ini=new std::function<void(Num *)>(_zs->bindScriptFunction<void (Num *)>("c.ini"));
		(*ini)(&num);
		delete ini;*/

		//_zs->clear();

		//_zs->clear();

		//(*ini)();

		//delete ini;

	}

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
	ZS_UNUSUED_PARAM(_zs);
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

	_zs->bindType<ClassA>("ClassA", ClassAWrap_new, ClassAWrap_delete);
	_zs->bindType<ClassB>("ClassB",ClassBWrap_new,ClassBWrap_delete);
	_zs->bindType<ClassC>("ClassC",ClassCWrap_new,ClassCWrap_delete);
	_zs->bindType<Num>("Num");
	_zs->bindType<ClassD>("ClassD");

	_zs->bindType<ParamA>("ParamA",ParamAWrap_new,ParamAWrap_delete);
	_zs->bindType<ParamB>("ParamB",ParamBWrap_new,ParamBWrap_delete);


	_zs->bindMemberFunction<ClassA>("fun1",ClassAWrap_fun1);
	_zs->bindStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ZetScript *,ClassA *,ClassA *)>(ClassAWrap_lt));
	_zs->bindStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ZetScript *,ClassA *,zetscript::zs_float *)>(ClassAWrap_lt));
	_zs->bindStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ZetScript *,zetscript::zs_float *,ClassA *)>(ClassAWrap_lt));


	_zs->extends<ClassB,ClassA>();

	_zs->bindMemberFunction<ClassB>("fun1",ClassBWrap_fun1);


	_zs->extends<ClassC,ClassB>();

	_zs->bindMemberFunction<ClassC>("newNum",ClassCWrap_newNum);

	_zs->bindMemberPropertyGetter<ClassC>("num",ClassCWrap_num_getter);
	_zs->bindMemberFunction<Num>("_pre_inc",NumWrap_pre_increment);
	_zs->bindMemberFunction<Num>("_pre_dec",NumWrap_pre_decrement);

	_zs->bindMemberPropertyGetter<Num>("x",NumWrap_x_getter);

	_zs->bindMemberFunction<ClassC>("get_d",ClassCWrap_get_d);
	//_zs->extends<ClassC,ClassB>();

	//_zs->bindMemberFunction<ClassC>("fun1",ClassCWrap_fun1);


	_zs->bindFunction("test_native_function_with_nulls",test_native_function_with_nulls);

	//a->printListMemberFunctions();

	//b->printListMemberFunctions();

	//c->printListMemberFunctions();

	_zs->saveState();


	test_call_native_function_with_nulls(_zs);

	test_call_function_member(_zs,_show_print);

}



#ifdef __MAIN__
int main(){
	zetscript::ZetScript zs;
	try{
		test_call_native_function(&zs);
		//test_bug();
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}
	return 0;
}
#endif


