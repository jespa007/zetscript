/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

class ClassA{
	public:
	 void fun1(const zetscript::String & _s,bool _show_print){
		 if(_show_print){
			 printf("from A: %s\n",_s.toConstChar());
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

void setNumRef(Num *_num){
	num_ref=_num;
}


struct ParamA{zetscript::String _s;ParamA(){_s="";}};
struct ParamB{zetscript::zs_float _f;ParamB(){_f=0;}};

ClassA *ClassA_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new ClassA;
}

void ClassA_fun1(zetscript::ScriptEngine *_script_engine, ClassA *_this, ParamA *_param_a,bool *_show_print){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->fun1(_param_a->_s.toConstChar(),*_show_print);
}

bool ClassA_lt(zetscript::ScriptEngine *_script_engine, ClassA *_n1, ClassA *_n2){
	ZS_UNUSUED_3PARAMS(_script_engine,_n1,_n2);
	return true;
}

bool ClassA_lt(zetscript::ScriptEngine *_script_engine, ClassA *_n1, zetscript::zs_float *_n2){
	ZS_UNUSUED_3PARAMS(_script_engine,_n1,_n2);
	printf("from float 1!\n");
	return true;
}

bool ClassA_lt(zetscript::ScriptEngine *_script_engine, zetscript::zs_float *_n1, ClassA *_n2){
	ZS_UNUSUED_3PARAMS(_script_engine,_n1,_n2);
	printf("from float 2!\n");
	return true;
}


void ClassA_delete(zetscript::ScriptEngine *_script_engine,ClassA *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}


ClassB *ClassB_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new ClassB;
}

void ClassB_fun1(zetscript::ScriptEngine *_script_engine, ClassB *_this, ParamB *_param_b,bool *_show_print){
	ZS_UNUSUED_PARAM(_script_engine);
	_this->fun1(_param_b->_f,*_show_print);
}

void ClassB_delete(zetscript::ScriptEngine *_script_engine,ClassB *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}

ClassC *ClassC_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new ClassC;
}

void ClassC_fun1(zetscript::ScriptEngine *_script_engine, ClassC *_c, bool *_b,bool *_show_print){
	ZS_UNUSUED_PARAM(_script_engine);
	_c->fun1(*_b,*_show_print);
}

/*void ClassC_num_setter(zetscript::ScriptEngine *_script_engine, ClassC *_c, zetscript::zs_int _num){
	ZS_UNUSUED_PARAM(_script_engine);
	_c->num=_num;
}*/

Num * ClassC_num_getter(zetscript::ScriptEngine *_script_engine, ClassC *_c){
	ZS_UNUSUED_PARAM(_script_engine);
	return &_c->num;
}

Num *ClassC_newNum(zetscript::ScriptEngine *_script_engine,ClassC *_c){
	ZS_UNUSUED_2PARAMS(_script_engine,_c);
	return num_ref;
}

zetscript::zs_int Num_x_getter(zetscript::ScriptEngine *_script_engine, Num *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	return _this->x;
}

void Num_setPosition(zetscript::ScriptEngine *_script_engine, Num *_this){
	ZS_UNUSUED_2PARAMS(_script_engine,_this);
}

void Num_pre_increment(zetscript::ScriptEngine *_script_engine, Num *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	++_n->x;
	++_n->y;
}

void Num_pre_decrement(zetscript::ScriptEngine *_script_engine, Num *_n){
	ZS_UNUSUED_PARAM(_script_engine);
	--_n->x;
	--_n->y;
}


ClassD * ClassC_get_d(zetscript::ScriptEngine *_script_engine, ClassC *_c){
	ZS_UNUSUED_PARAM(_script_engine);
	return &_c->d;
}

void ClassC_delete(zetscript::ScriptEngine *_script_engine,ClassC *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}

bool allCharsTheSame(const zetscript::String & input){

    for(int i =1; i < input.length(); i++){
        if(input[i-1] != input[i]) return false;
    }
    return true;
}

bool allValuesTheSame(zetscript::ScriptEngine *_script_engine,zetscript::ArrayScriptObject * sov){
	ZS_UNUSUED_PARAM(_script_engine);
	zetscript::Vector<zetscript::StackElement *> *stk_elements = sov->getStkListElements();
   for(int i =1; i < stk_elements->length(); i++){
	   zetscript::StackElement *stk_1=stk_elements->get(i-1);
	   zetscript::StackElement *stk_2=stk_elements->get(i-0);
       if(stk_1->value != stk_2->value) return false;
   }
   return true;
}

zetscript::Vector<zetscript::zs_int> newRandomCountExt(zetscript::ScriptEngine *_script_engine,zetscript::zs_int max_number, zetscript::zs_int n_elements){
	ZS_UNUSUED_PARAM(_script_engine);
	if(max_number==0 || n_elements==0){
		throw("max number or n_elements are 0s");
	}
		//new throw("invalid number");

	zetscript::Vector<zetscript::zs_int> index_rand;// = new int [n_elements];

	zetscript::zs_int item=0;
	bool found = false;
	for(int i = 0; i < n_elements; i++){

		do{
			 item = rand()%(max_number);
			found = false;
		 	for(int j=0; j < i;j++){
				if(index_rand.get(j) == item){
					found=true;
				}
		 	}
		}while(found);

		index_rand.push(item);

	}
	return index_rand;
}

zetscript::ArrayScriptObject * reorderValuesFromIntArray(zetscript::ScriptEngine *_script_engine,zetscript::ArrayScriptObject *_input){

	zetscript::ArrayScriptObject *output=zetscript::ArrayScriptObject::newArrayScriptObject(_script_engine);
	zetscript::Vector<zetscript::StackElement *> *input=_input->getStkListElements();
	uint16_t input_count=input->length();
	zetscript::Vector<zetscript::zs_int> rand_txt;

    if(allValuesTheSame(_script_engine,_input)){
          return _input;
    }


    bool equal = true;
    try{
		do{
			rand_txt= newRandomCountExt(_script_engine,input_count,input_count);
			 for(zetscript::zs_int i =0; i < input_count; i++){
		         equal&=(i==rand_txt.get(i)); // check if not consecutive...
			 }
		}while(equal);
    }catch(std::exception & ex){
    	fprintf(stderr,"%s",ex.what());
    }

    for(int i =0; i < input_count; i++){
    	// save resulting
    	zetscript::StackElement *stk=output->newSlot();
    	stk->value=_input->getStackElementByIndex(rand_txt.get(i))->value;
    	stk->properties=zetscript::STACK_ELEMENT_PROPERTY_INT;
    }

     return output;
}



//-------
// PARAMS

ParamA *ParamA_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new ParamA;
}


void ParamA_delete(zetscript::ScriptEngine *_script_engine,ParamA *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}

ParamB *ParamB_new(zetscript::ScriptEngine *_script_engine){
	ZS_UNUSUED_PARAM(_script_engine);
	return new ParamB;
}


void ParamB_delete(zetscript::ScriptEngine *_script_engine,ParamB *_this){
	ZS_UNUSUED_PARAM(_script_engine);
	delete _this;
}

//------------------------------
// TEST NATIVE CALL FUNCTION MEMBER

void test_call_function_member(zetscript::ScriptEngine *_script_engine, bool _show_print=true){

	Num num;
	setNumRef(&num);

	// ensure working doing various clear-eval
	for(int i=0; i < 5; i++){

		_script_engine->clear();

		try{
		_script_engine->compileAndRun(
			zetscript::String::format(
					//"import \"include.zs\"\n"
					"function class_c_load(_class_c){\n"
						"_class_c.num_ref=_class_c.newNum()\n"
						"if(%s){\n"
						"Console::outln(\"load\")\n"
						"}\n"
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
						"updateTutorial(){\n"
							"for(var i=0; i < 5; i++){\n"
							"	this.v2d_target=this.num_ref\n"
							"}\n"
							"this.v2d_target.setPosition();\n"
							//"n.load()"
							"if(%s){\n"
							"	Console::outln(\"update\")\n"
							"}\n"
						"}\n"
						"update(){\n"
							"this.updateTutorial();\n"
						"}\n"
					"}\n"
					"function ini2(n){n.load()}"
					"var c=new ClassCWrap(new ClassC())\n"
					"ini2(c);\n"
					"c.update();\n"
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
			)
		//,EVAL_OPTION_PRINT_BYTE_CODE

		);

		//_script_engine->clear();

		}catch(std::exception & ex){
			fprintf(stderr,"%s\n",ex.what());
		}

		/*auto ini=new std::function<void(Num *)>(_script_engine->bindScriptFunction<void (Num *)>("c.ini"));
		(*ini)(&num);
		delete ini;*/

		//_script_engine->clear();

		//_script_engine->clear();

		//(*ini)();

		//delete ini;

	}

}

//------------------------------
// TEST NATIVE CALL WITH NULLS
void test_native_function_with_nulls(
		zetscript::ScriptEngine *_script_engine
		,zetscript::zs_int _p1
		,zetscript::zs_float *_p2
		,bool *_p3
		,ClassA *_p4
){
	ZS_UNUSUED_PARAM(_script_engine);
	if(_p1 != 0){std::runtime_error("_p1 not 0");}
	if(_p2 != NULL){std::runtime_error("_p2 not NULL");}
	if(_p3 != NULL){std::runtime_error("_p3 not NULL");}
	if(_p4 != NULL){std::runtime_error("_p4 not NULL");}

}

void test_call_native_function_with_nulls(zetscript::ScriptEngine *_script_engine){


	_script_engine->compileAndRun(
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


void test_call_native_function(zetscript::ScriptEngine *_script_engine, bool _show_print=true){

	_script_engine->registerType<ClassA>("ClassA", ClassA_new, ClassA_delete);
	_script_engine->registerType<ClassB>("ClassB",ClassB_new,ClassB_delete);
	_script_engine->registerType<ClassC>("ClassC",ClassC_new,ClassC_delete);
	_script_engine->registerType<Num>("Num");
	_script_engine->registerType<ClassD>("ClassD");

	_script_engine->registerType<ParamA>("ParamA",ParamA_new,ParamA_delete);
	_script_engine->registerType<ParamB>("ParamB",ParamB_new,ParamB_delete);


	_script_engine->registerFunction("reorderValuesFromIntArray",reorderValuesFromIntArray);

	_script_engine->registerMemberFunction<ClassA>("fun1",ClassA_fun1);
	_script_engine->registerStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ScriptEngine *,ClassA *,ClassA *)>(ClassA_lt));
	_script_engine->registerStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ScriptEngine *,ClassA *,zetscript::zs_float *)>(ClassA_lt));
	_script_engine->registerStaticMemberFunction<ClassA>("_lt",static_cast<bool (*)(zetscript::ScriptEngine *,zetscript::zs_float *,ClassA *)>(ClassA_lt));


	_script_engine->extends<ClassB,ClassA>();

	_script_engine->registerMemberFunction<ClassB>("fun1",ClassB_fun1);


	_script_engine->extends<ClassC,ClassB>();

	_script_engine->registerMemberFunction<ClassC>("newNum",ClassC_newNum);

	_script_engine->registerMemberPropertyMetamethod<ClassC>("num","_get",ClassC_num_getter);
	_script_engine->registerMemberFunction<Num>("_preinc",Num_pre_increment);
	_script_engine->registerMemberFunction<Num>("_predec",Num_pre_decrement);

	_script_engine->registerMemberPropertyMetamethod<Num>("x","_get",Num_x_getter);
	_script_engine->registerMemberFunction<Num>("setPosition",Num_setPosition);

	_script_engine->registerMemberFunction<ClassC>("get_d",ClassC_get_d);
	//_script_engine->extends<ClassC,ClassB>();

	//_script_engine->registerMemberFunction<ClassC>("fun1",ClassC_fun1);


	_script_engine->registerFunction("test_native_function_with_nulls",test_native_function_with_nulls);

	//a->printListMemberFunctions();

	//b->printListMemberFunctions();

	//c->printListMemberFunctions();

	_script_engine->saveState();


	test_call_native_function_with_nulls(_script_engine);

	test_call_function_member(_script_engine,_show_print);

}

void test_call_native_function_no_print(zetscript::ScriptEngine *_script_engine){
	test_call_native_function(_script_engine,false);
}

#ifdef __MAIN__
int main(){
	zetscript::ScriptEngine script_engine;
	try{
		test_call_native_function(&script_engine);
		//test_bug();
	}catch(std::exception & ex){
		fprintf(stderr,"%s\n",ex.what());
		return -1;
	}
	return 0;
}
#endif


