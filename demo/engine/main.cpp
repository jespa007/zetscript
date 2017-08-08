#include "CZetScript.h"
#include "CRender.h"
#include "CInput.h"

using namespace zetscript;

int main(int argc, char *argv[]){


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

	// here we initiate the singletons ...
	CRender *render = CRender::getInstance();
	CInput *input = CInput::getInstance();
	CZetScript *zetscript = CZetScript::getInstance();

	//--------------------------------------------------
	// 1. Make our C++ bindings for script calls
	if(!CScriptClass::register_C_Class<CRender>("CRender")) return false;
	if(!CScriptClass::register_C_Class<CImage>("CImage")) return false;
	if(!CScriptClass::register_C_Class<CFont>("CFont")) return false;


	if(!register_C_Variable("TR_UP",TR_UP)) return false;
	if(!register_C_Variable("TR_DOWN",TR_DOWN)) return false;
	if(!register_C_Variable("TR_LEFT",TR_LEFT)) return false;
	if(!register_C_Variable("TR_RIGHT",TR_RIGHT)) return false;


	// CRender bindings...
	if(!register_C_Function("getRender",CRender::getInstance)) return false;
	if(!CScriptClass::register_C_FunctionMemberInt<CRender>("drawImage",static_cast<void (CRender::*)(int *,int*,CImage * )>(&CRender::drawImage))) return false;
	if(!CScriptClass::register_C_FunctionMemberInt<CRender>("drawText",static_cast<void (CRender::*)(int *,int*,CFont *, string * )>(&CRender::drawText))) return false;

	// CFont bindings...
	if(!CScriptClass::register_C_FunctionMemberInt<CFont>("load",static_cast<void (CFont::*)(string *,int *,int* )>(&CFont::load))) return false;


	int idxSt=CState::saveState();


	// 2. eval the script...
	if(!zetscript->eval_file(argv[1])){
		return -1;
	}

	// create window 640x480
	render->createWindow(640,480);

	// create direct call script function
	auto init=zetscript->script_call("init");
	auto update=zetscript->script_call("update");

	if(init){
		(*init)(NULL);
	}

	do{
		// clear screen...
		render->clear(0,0,0);

		// update keyboard events...
		input->update();


		// if press F5 reload file...
		if(T_F5) {

			CState::setState(idxSt); // restore everithing before saveState

			// 2. reeval the script...
			if(!zetscript->eval_file(argv[1])){
				break;
			}

			if(init){
				(*init)(NULL);
			}

			// recreate script function
			init=zetscript->script_call("init");
			update=zetscript->script_call("update");

			if(init){
				(*init)(NULL);
			}

			printf("State restored");
		}

		// call update script function...
		if(update){
			(*update)(NULL);
		}


		// update screen...
		render->update();

	}while(!T_ESC);

	// destroy singletons...
	CInput::destroy();
	CRender::destroy();
	CZetScript::destroy();

	return 0;
}
