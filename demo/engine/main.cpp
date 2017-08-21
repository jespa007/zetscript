#include "CZetScript.h"
#include "CInput.h"
#include "CRender.h"
#include "CSoundPlayer.h"

#define WIDTH_VIDEO  320
#define HEIGHT_VIDEO 200

using namespace zetscript;

int currentTime(){
	return SDL_GetTicks();
}

int main(int argc, char *argv[]){

	Uint32 start_ticks=0,timeout=0;

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

	//printf("%i %i\n\n",sizeof(int), sizeof(void*));
	//return -1;


	// here we initiate the singletons ...
	CSoundPlayer *soundPlayer = CSoundPlayer::getInstance();
	CRender *render = CRender::getInstance();
	CInput *input = CInput::getInstance();
	CZetScript *zetscript = CZetScript::getInstance();

	//--------------------------------------------------
	// 1. Make our C++ bindings for script calls
	// Binds singleton CRender class...

	// generic classes...
	if(!register_C_Function("currentTime",currentTime)) return false;

	// Binds CImage class...
	if(!CScriptClass::register_C_Class<CImage>("CImage")) return false;
	// bind a custom constructor...
	CScriptClass::register_C_FunctionMemberInt<CImage>("CImage",static_cast<bool (CImage::*)(zetscript::CVector * )>(&CImage::createSquarePixmap));


	// Binds CSprite class
	if(!CScriptClass::register_C_Class<CSprite>("CSprite")) return false;
	if(!register_C_Function("checkCollision",CSprite::checkCollision)) return false;
	if(!register_C_VariableMember(CSprite,x)) return false;
	if(!register_C_VariableMember(CSprite,y)) return false;
	if(!register_C_VariableMember(CSprite,dx)) return false;
	if(!register_C_VariableMember(CSprite,dy)) return false;
	if(!register_C_VariableMember(CSprite,width)) return false;
	if(!register_C_VariableMember(CSprite,height)) return false;


	if(!register_C_FunctionMember(CSprite,setTimeFrame)) return false;
	if(!register_C_FunctionMember(CSprite,update)) return false;
	if(!register_C_FunctionMember(CSprite,addFrame)) return false;

	// Binds CFont class
	if(!CScriptClass::register_C_Class<CFont>("CFont")) return false;
	if(!register_C_FunctionMember(CFont,load)) return false;

	// Binds CSound class
	if(!CScriptClass::register_C_Class<CSound>("CSound")) return false;
	if(!register_C_FunctionMember(CSound,load)) return false;

	if(!CScriptClass::register_C_SingletonClass<CRender>("CRender")) return false;
	if(!register_C_Function("getRender",CRender::getInstance)) return false;
	if(!register_C_FunctionMember(CRender,drawImage)) return false;
	if(!register_C_FunctionMember(CRender,drawText)) return false;
	if(!register_C_FunctionMember(CRender,drawSprite)) return false;
	if(!register_C_FunctionMember(CRender,getWidth)) return false;
	if(!register_C_FunctionMember(CRender,getHeight)) return false;


	// Binds singleton CSoundPlayer...
	if(!CScriptClass::register_C_SingletonClass<CSoundPlayer>("CSoundPlayer")) return false;
	if(!register_C_Function("getSoundPlayer",CSoundPlayer::getInstance)) return false;
	if(!register_C_FunctionMember(CSoundPlayer,play)) return false;




	// Binds input global vars...
	if(!register_C_Variable("TR_UP",TR_UP)) return false;
	if(!register_C_Variable("TR_DOWN",TR_DOWN)) return false;
	if(!register_C_Variable("TR_LEFT",TR_LEFT)) return false;
	if(!register_C_Variable("TR_RIGHT",TR_RIGHT)) return false;
	if(!register_C_Variable("T_SPACE",T_SPACE)) return false;



	int idxSt=CState::saveState();


	// 2. eval the script...
	if(!zetscript->eval_file(argv[1])){
		return -1;
	}

	//return -1;

	// init SDL subsystems
	render->createWindow(WIDTH_VIDEO,HEIGHT_VIDEO);
	soundPlayer->setup();

	// create direct call script function
	auto init=zetscript->script_call("init");
	auto update=zetscript->script_call("update");
	auto unload=zetscript->script_call("unload");

	if(init){
		if(!(*init)(NULL)){
			return -1;
		}
	}




	do{




		// clear screen...
		render->clear(0,0,0);

		// update keyboard events...
		input->update();

		if(T_F9) {
			render->toggleFullscreen();
		}

		// if press F5 reload file...
		if(T_F5) {

			// call init function...
			if(unload){
				(*unload)(NULL);
			}

			CState::setState(idxSt); // restore everithing before saveState

			// 2. reeval the script...
			zetscript->eval_file(argv[1]);


			// 3. recreate script function
			init=zetscript->script_call("init");
			update=zetscript->script_call("update");
			unload=zetscript->script_call("unload");

			// call init function...
			if(init){
				(*init)(NULL);
			}

			printf("State restored\n");
		}

		start_ticks=SDL_GetTicks();

		// call update script function...
		if(update){
			(*update)(NULL);
		}

		if(timeout<SDL_GetTicks()){
			printf("FPS %.02f\n",1000.0f/(SDL_GetTicks()-start_ticks));
			timeout=SDL_GetTicks()+1000;
		}


		// update screen...
		render->update();



	}while(!T_ESC);

	if(unload){
		(*unload)(NULL);
	}

	// destroy singletons...
	CInput::destroy();
	CRender::destroy();
	CSoundPlayer::destroy();
	CZetScript::destroy();

	return 0;
}
