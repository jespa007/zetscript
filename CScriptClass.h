/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



/*
//	______           _     _                    _   _____ _
//	| ___ \         (_)   | |                  | | /  __ \ |
//	| |_/ /___  __ _ _ ___| |_ ___ _ __ ___  __| | | /  \/ | __ _ ___ ___
//	|    // _ \/ _` | / __| __/ _ \ '__/ _ \/ _` | | |   | |/ _` / __/ __|
//	| |\ \  __/ (_| | \__ \ ||  __/ | |  __/ (_| | | \__/\ | (_| \__ \__ \
//	\_| \_\___|\__, |_|___/\__\___|_|  \___|\__,_|  \____/_|\__,_|___/___/
//            __/ |
//           |___/
// _________________________________________________
//  __________________________________
*/









namespace zetscript{


/**
 * Stores the basic information to build a object through built AST structure
 */



	class  CScriptClass:public CCommonClassFunctionBase{

	public:

		// STRUCTS






#if 0
		 static bool searchVarFunctionSymbol(CCommonClassFunctionBase * scope_info, tInstruction *iao, int current_idx_function,bool & symbol_not_found, unsigned int scope_type=0);

		 static void buildScopeVariablesBlock(CScriptFunction *root_class_irfs );
		 static void unloadRecursiveFunctions(CScriptFunction * info_function);

		 static bool updateFunctionSymbols(int idxSxriptFunctionObject, const string & parent_symbol, int n_function);// is_main_class, bool is_main_function);
#endif


		//------------- VARIABLES STRUCT ---------------




		//CCommonClassFunctionBase			scope_info;
		//tVariableSymbolInfo symbol_info;
		char  idx_function_member_constructor;
		//int idxClass;

		std::function<void * ()> 			* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		string classPtrType; // type_id().name();
		//unsigned char  idxClass;

		// vector<int> is set because C can have multiple herited classes (and we have to check whether an attempt C class to base of is not already set) where as script class only have one.
		vector<unsigned char > idxBaseClass;

		vector<CScriptFunction * > metamethod_operator[MAX_METAMETHOD_OPERATORS]; // overrided metamethod

		ZETSCRIPT_MODULE_EXPORT CScriptClass(unsigned char _idxClass, short _idxScope);

		virtual CScriptFunction						*  			registerFunctionMember(const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);

		bool is_c_class();

		//------------- STATIC METHODS ---------------


		//static bool initStaticVars();
		//static void destroyStaticVars();

		// HELPERS



		// FUNCTIONS
		//static void 											setVectorScriptClassNode(vector<CScriptClass *> 	* set_vec);


		/**
		 * Class Manager
		 */

		virtual ~CScriptClass();


	};


}


