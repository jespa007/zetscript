class CLocalScope: public CObject{

public:
	enum TYPE{
		UNKNOW=0,
		BOOL,
		INTEGER,
		NUMBER,
		STRING,
		VAR
	};

private:

	static CUndefined *m_defaultVar;

	// each scope has its registered vars.... if the compiler tries to find a var and this is not found goes through parent scopes until
	// it finds it.

	typedef struct{
		int m_line;
		CObject *m_obj;
	}tInfoRegisteredVar;

	map<string,tInfoRegisteredVar *> m_registeredVariable;
	vector<CLocalScope *> m_scopeList;

	// The a parent scope ...
	CLocalScope *m_parentScope,*m_mainScope;


	class tInfoAsmOp{

	public:


	     //int type_op;
	     //tInfoObjectOperator *funOp;
	     //CObject *left_var_obj;
	     void *result_obj; // can be float/bool/string or variable.
	     //string type_res;

	     //------------------
	     TYPE result_type;
	     ASM_OPERATOR operator_type;
	     void *ptr_value; // can be float, bool or string.
	     //------------------

	     int index_left,index_right;
	    // bool (* isconvertable)(int value);

		tInfoAsmOp(){
			result_type=TYPE::UNKNOW;
			operator_type=ASM_OPERATOR::UNKNOW;
			//isconvertable=NULL;
			//left_var_obj=NULL;
		  //   type_op=0;
		   //  funOp=NULL;
			result_obj=NULL; // must be created before.

		   // type_res="none";
		    index_left=index_right=-1;
		    ptr_value=NULL;
		}



	};

	typedef struct{

	    vector<tInfoAsmOp *> asm_op;
	}tInfoStatementOp;


	//typedef struct _tLocalScope;

	//typedef struct _tLocalScope{

		vector<tInfoStatementOp> m_listStatements;

	//_tLocalScope *m_parentScope;


	//}tLocalScope;

	//typedef struct{

		//vector<tLocalScope *> m_localScope;
		//tLocalScope *base_scope;
		//map<string,tLocalScope *>	m_label;

	//}tContext;
		static tInfoRegisteredVar * existRegisteredVariableRecursive(const string & var_name, CLocalScope *lc);
		tInfoRegisteredVar * existRegisteredVariable(const string & var_name);

		void unregisterOperators();
		void insertNewStatment();


		ASM_OPERATOR getNumberOperatorId_TwoOps(const string & op,TYPE & result_type);
		ASM_OPERATOR getIntegerOperatorId_TwoOps(const string & op,TYPE & result_type);
		ASM_OPERATOR getBoleanOperatorId_TwoOps(const string & op,TYPE & result_type);
		ASM_OPERATOR getStringOperatorId_TwoOps(const string & op,TYPE & result_type);

		ASM_OPERATOR getNumberOperatorId_OneOp(const string & op);
		ASM_OPERATOR getBoleanOperatorId_OneOp(const string & op);
		ASM_OPERATOR getStringOperatorId_OneOp(const string & op);

		void insertPushScopeInstruction(CLocalScope * _goto_scope);
		void insertPopScopeInstruction();


		//tLocalScope * createLocalScope(tLocalScope *m_parent);
		//tContext * createContext();

		TYPE getTypeAsmResult(int index);
		static bool isVarDeclarationStatment(const char *statment, bool & error,char **eval_expression, int & m_line, CLocalScope * _localScope);
		static char * evalRecursive(const char * s,int & level_scope, int & m_line,CLocalScope *local_scope);

		void addLocalScope(CLocalScope *_ls);
		bool performExpression(const char *advanced_expression, int & m_line);
	//	vector<tInfoStatementOp> * getListStatments();

public:

	static void createSingletons();
	static void destroySingletons();

	CLocalScope(CLocalScope * m_parent=NULL);
	CLocalScope * getMainScope();

	//---------------------------------
	// Register functions
	CObject *getRegisteredVariable(const string & v, bool print_msg=true);
	bool registerVariable(const string & var_name, int m_line);
	bool defineVariable(const string & var_name, CObject *obj);
	CLocalScope * getParent();
	CLocalScope * getRootScope();

	//---------------------------------
	bool insertLoadValueInstruction(const string & v, string & type_ptr);
	bool insertMovVarInstruction(CObject *var, int right);

	bool insertOperatorInstruction(const string &op, int left, int right=-1);
	string getUserTypeResultCurrentStatmentAtInstruction(unsigned instruction);


	bool eval(const string & s);

	// execute instructions at local scope ...
	static bool execute(CLocalScope *lc);

	~CLocalScope();


};
