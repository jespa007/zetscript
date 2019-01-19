/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"
#include <istream>

#define ZETSCRIP_COPYRIGHT "ZetScript %i.%i.%i Copyright (C) 2016-2019 Jordi Espada\n",ZETSCRIPT_MAJOR_VERSION,ZETSCRIPT_MINOR_VERSION,ZETSCRIPT_PATCH_VERSION

using namespace zetscript;

class A{
protected:
	unsigned b;
public:
	A(){ this->b=10;}
	virtual unsigned ret(unsigned i){ return i*this->b;}
	virtual ~A(){};
};

class B:public A{
public:
	virtual unsigned ret(unsigned i){ return i*this->b;}
};

class C:public B{
public:
	virtual unsigned ret(unsigned i){ return i*this->b;}
};

class D:public C{
public:
	virtual unsigned ret(unsigned i){ return i*this->b;}
};

class E:public D{
public:
	virtual unsigned ret(unsigned i){ return i*this->b;}
};

class F:public E{
public:
	virtual unsigned ret(unsigned i){ return i*this->b;}
};

class F_A{
protected:
	unsigned b;
	std::function<unsigned(unsigned)> *f;
public:
	F_A(){ this->b=10;this->f=NULL;}
	unsigned ret(unsigned i){
		if(f==NULL){
			return i*this->b;
		}else{
			return (*f)(i);
		}
	}

};

class F_B:public F_A{
public:
	unsigned ret(unsigned i){ return i*this->b;}
};

class F_C:public F_B{
public:
	unsigned ret(unsigned i){ return i*this->b;}
};

class F_D:public F_C{
public:
	unsigned ret(unsigned i){ return i*this->b;}
};

class F_E:public F_D{
public:
	unsigned ret(unsigned i){ return i*this->b;}
};


// NEW_FUNCTION(f) new std::function<int(int)>([this](int i) { return this->ret(i); })
#define NEW_RET_FUNCTION_1P(VAR,TYPE_RET,FUN,TYPE_P1) {if(VAR!=NULL){delete VAR;};VAR=new std::function<TYPE_RET(TYPE_P1)>([](TYPE_P1 i) { return this->FUN(i); });

class F_F:public F_E{
public:
	F_F(){
		//f=new std::function<int(int)>([]())   (std::bind(&F_F::ret, this,std::placeholders::_1));
		NEW_RET_FUNCTION_1P(f,unsigned,ret,unsigned);
	}
	unsigned ret(unsigned i){ return i*this->b;}
};


unsigned b=10;

unsigned ret(unsigned i){ return i*b;}

int main(int argc, char * argv[]) {

	F f;
	F_F ff;
	A *a = (A *)&f;
	F_A *f_a=(F_A *)&ff;
	unsigned kk=0;
	for(unsigned i=0; i < 1000000000; i++){
		//kk+=a->ret(i);
		kk+=f_a->ret(i);
		//kk+=ret(i);
	}

	printf("result:%i\n",kk);

	return 0;


	CZetScript *zetscript = CZetScript::getInstance();

	if (argc > 1) {
		bool execute=true;
		bool show_bytecode=false;
		string file="";

		for(int i=1; i < argc; i++){

			vector<string> a=CZetScriptUtils::split(argv[i],'=');
			switch(a.size()){
			case 1:

				if(strcmp(argv[i],"--no_execute")==0){
					execute = false;
				}else if(strcmp(argv[i],"--show_bytecode")==0){
					show_bytecode=true;

				}else if(strcmp(argv[i],"--version")==0){
					printf(ZETSCRIP_COPYRIGHT);
					exit(0);
				}else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			case 2:

				if(strcmp(a[0].c_str(),"--file")==0){
					file=a[1].c_str();
				}
				else{
					fprintf(stderr,"invalid argument %s\n",argv[i]);
					exit(-1);
				}
				break;
			default:
				fprintf(stderr,"invalid argument %s\n",argv[i]);
				exit(-1);
				break;
			}

		}

		if(file==""){
			fprintf(stderr,"Program with arguments you must specify file (i.e --file=filename )\n");
			exit(-1);
		}

		try{

			zetscript->evalFile(file,execute,show_bytecode);
		}catch(script_error & error){
			fprintf(stderr,"%s\n",error.what());
		}
		catch(std::exception & error){
			fprintf(stderr,"%s\n",error.what());
		}
	}
	else{

		bool exit = false;
		string expression;
		printf(ZETSCRIP_COPYRIGHT);
		printf("\n");



		do{
			printf("zs>");
			std::getline(std::cin,expression);

			exit = expression=="exit" || expression=="quit";
			if(!exit){ // evaluate expression
				try{
					zetscript->evalString(expression);
				}catch(script_error & ex){
					fprintf(stderr,"%s\n",ex.what());
				}
			}

		}while(!exit);// && (instr[++idx_ptr] != NULL));
	}

	CZetScript::destroy();

#ifdef __MEMMANAGER__
	MEM_ViewStatus();
#endif

	return 0;
}
