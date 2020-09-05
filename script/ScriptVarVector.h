/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new ScriptVarVector(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new ScriptVarVector())))

namespace zetscript{

	class CZetgine;
	class  ScriptVarVector: public ScriptVar{
	public:
		StackElement return_callc;

		template<typename T>
		static std::vector<T> toStdVector(ScriptVarVector *v_in){
			std::vector<T> v_out;
			const char * dst_convert_type = typeid(T).name();
			float aux_flt;
			std::vector<StackElement> * variable = v_in->getAllProperties();

			if(v_in){
				for(unsigned i = 0; i < variable->size(); i++){

					StackElement sv=variable->at(i);

					switch(sv.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPE_PRIMITIVES)
					{
						default:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
							if(ZS_STRCMP(dst_convert_type, ==,typeid(float).name())){
								memcpy(&aux_flt, &sv.stk_value, sizeof(float));
								v_out.push_back(aux_flt);
							}else if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name())){
								v_out.push_back((intptr_t)sv.stk_value);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<float>");
								return v_out;
							}
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
							THROW_RUNTIME_ERROR("type not implemented yet");
							return v_out;
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
							if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name()) || ZS_STRCMP(dst_convert_type, ==,typeid(float).name())){// typeid(int).name()) || ){
								v_out.push_back((intptr_t)sv.stk_value);
							}else{
								THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<int>");
								return v_out;
							}
							break;
					}
				}
			}
			return v_out;
		}

		static void    			pushSf(ScriptVarVector *sv,StackElement  * stk){return sv->push(stk);}
		static StackElement *  	popSf(ScriptVarVector *sv){return sv->pop();}
		//static int 				sizeSf(ScriptVarVector *sv){return sv->size();}


		ScriptVarVector(){}
		ScriptVarVector(ZetScript *_zs);

		void 					push(StackElement  * stk);
		StackElement *			pop();


		//virtual void unrefSharedPtr();

		//int size();

		//virtual void destroy();
		//virtual ~ScriptVarVector();
	};

}
