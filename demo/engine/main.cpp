#include "CZetScript.h"
#include "CRender.h"
#include "CInput.h"

using namespace zetscript;




int main(int argc, char *argv[]){

	// SDL stuff


	// here we initiate the script engine ...







	CRender *render = CRender::getInstance();
	CInput *input = CInput::getInstance();
	CZetScript *zetscript = CZetScript::getInstance();

	//--------------------------------------------------
	// zetscript interface

	if(!CScriptClass::register_C_Class<CRender>("CRender")) return false;
	if(!CScriptClass::register_C_Class<CImage>("CImage")) return false;


	if(!register_C_Variable("TR_UP",TR_UP)) return false;
	if(!register_C_Variable("TR_DOWN",TR_DOWN)) return false;
	if(!register_C_Variable("TR_LEFT",TR_LEFT)) return false;
	if(!register_C_Variable("TR_RIGHT",TR_RIGHT)) return false;

	if(!register_C_Function("getRender",CRender::getInstance)) return false;
	//if(!register_C_FunctionMember(CRender,drawImage)) return false;
	if(!CScriptClass::register_C_FunctionMemberInt<CRender>("drawImage",static_cast<void (CRender::*)(int *,int*,CImage * )>(&CRender::drawImage))) return false;


	//return 0;




	if (argc < 2) {
		printf("Put the game file to parse.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("engine invader.zs");
		printf("\n");
		printf("\n");
		return 0;
	}

	if(!zetscript->eval_file(argv[1])){
		return -1;
	}

/*
	if(!zetscript->execute()){
		return -1;
	}
*/

	render->createWindow(640,480);



	/*CImage *img=NULL;

		img=new CImage(



		vector<char>{
		  1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
		 ,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		 ,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
		 ,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1
		},
		0xffffff
	);*/

	//CZetScript *zetscript = CZetScript::getInstance();


	auto init=zetscript->script_call("init");
	auto update=zetscript->script_call("update");


	//TR_LEFT=false;
	if(init){
		(*init)(NULL);
	}

	do{
		render->clear(0,0,0);



		input->update();

		if(update){
			(*update)(NULL);
		}

		//render->drawImage(0,0,img);


		render->update();
		// update input



		// update script



	}while(!T_ESC);


	CInput::destroy();
	CRender::destroy();



	return 0;
}
