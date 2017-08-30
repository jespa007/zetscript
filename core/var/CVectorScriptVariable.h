#pragma once

#define NEW_VECTOR_VAR (new CVectorScriptVariable()) //CScriptClass::getInstance()->getRegisteredClassByIdx(CScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new CVectorScriptVariable())))

namespace zetscript{

	class  CVectorScriptVariable: public CScriptVariable{

	public:
		template<typename _T>
		static vector<_T> cast(CVectorScriptVariable *v_in){
			vector<_T> v_out;
			string typestr = typeid(_T).name();


			if(v_in){
				for(int i = 0; i < v_in->size(); i++){

					tStackElement sv=v_in->m_objVector[i];

					switch(sv.properties & MASK_VAR_PRIMITIVE_TYPES)
					{
						default:
						case INS_PROPERTY_TYPE_UNDEFINED:
						case INS_PROPERTY_TYPE_NULL:
						case INS_PROPERTY_TYPE_NUMBER:
						case INS_PROPERTY_TYPE_BOOLEAN:
						case INS_PROPERTY_TYPE_STRING:
						case INS_PROPERTY_TYPE_FUNCTION:
							zetscript::zs_print_error_cr("type not implemented to cast. Sorry.");
							return v_out;
							break;
						case INS_PROPERTY_TYPE_INTEGER:
							if((typestr == typeid(int).name())){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								zetscript::zs_print_error_cr("Error trying to cast element on vector<int>");
								return v_out;
							}
							break;

					}

					/*if((typestr == typeid(int).name())  && (se->properties & INS_PROPERTY_TYPE_INTEGER)){
						ptr = &value;
						*((intptr_t *)ptr) = (intptr_t)se->stkValue;
					}
					else if((typestr == typeid(float).name())  && (se->properties & INS_PROPERTY_TYPE_NUMBER)){
						ptr = &value;
						memcpy(ptr,&se->stkValue,sizeof(float));
					}
					else if((typestr == typeid(string).name())  && (se->properties & INS_PROPERTY_TYPE_STRING)){
						ptr=&value;
						*((string *)ptr) = *((string *)se->stkValue);
					}
					else if((typestr == typeid(bool).name())  && (se->properties & INS_PROPERTY_TYPE_BOOLEAN)){
						value = (bool)se->stkValue;
					}
					else{
						fprintf(stderr,"eval<%s>(...): Error evaluating \"%s\". Property:0x%X",typestr.c_str(),str_to_eval.c_str(),se->properties);
						exit(-1);
					}*/




				}
			}


			return v_out;
		}

		int _i_size;

		vector<tStackElement> m_objVector;

		CVectorScriptVariable();
		//CVectorScriptVariable(CScriptClass *info_registered_class);
		virtual bool unrefSharedPtr();
		virtual bool initSharedPtr();
		//void add(const tStackElement  & v);
		void add(tStackElement  * v);
		void pop();
	//	void add(int * v);
	//	void add(bool * v);
	//	void add( string * v);
	//	void add(float * v);
		tStackElement *push();
	//	void add(CScriptVariable * v);
		int size();




	};

}
