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
						case STK_PROPERTY_TYPE_UNDEFINED:
						case STK_PROPERTY_TYPE_NULL:
						case STK_PROPERTY_TYPE_NUMBER:
						case STK_PROPERTY_TYPE_BOOLEAN:
						case STK_PROPERTY_TYPE_STRING:
						case STK_PROPERTY_TYPE_FUNCTION:
							zs_print_error_cr("type not implemented to cast. Sorry.");
							return v_out;
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if((typestr == typeid(int).name())){
								v_out.push_back((intptr_t)sv.stkValue);
							}else{
								zs_print_error_cr("Error trying to cast element on vector<int>");
								return v_out;
							}
							break;

					}
				}
			}


			return v_out;
		}

		int _i_size;

		vector<tStackElement> m_objVector;

		CVectorScriptVariable();

		virtual bool unrefSharedPtr();
		virtual bool initSharedPtr(bool is_assigned=false);

		void add(tStackElement  * v);
		void pop();
		tStackElement *push();
		int size();




	};

}
