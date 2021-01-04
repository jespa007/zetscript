/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define NEW_VECTOR_VAR (new ScriptObjectVector(this->zs)) //ScriptClass::getInstance()->getRegisteredClassByIdx(ScriptClass::getInstance()->getIdxClassVector())))//,(void *)(new ScriptObjectVector())))

namespace zetscript{

	class CZetgine;
	class  ScriptObjectVector: public ScriptObject{
	public:
		StackElement return_callc;

		template<typename T>
		static std::vector<T> toStdVector(ScriptObjectVector *v_in){
			std::vector<T> v_out;
			const char * dst_convert_type = typeid(T).name();
			float aux_flt;
			zs_vector * elements = v_in->getAllElements();


			for(unsigned i = 0; i < elements->count; i++){

				StackElement *sv=elements->get(i);

				switch(sv->properties & MSK_STK_PROPERTY_TYPE_PRIMITIVES)
				{
					case MSK_STK_PROPERTY_FLOAT:
						if(ZS_STRCMP(dst_convert_type, ==,typeid(float).name())){
							memcpy(&aux_flt, &sv->stk_value, sizeof(float));
							v_out.push_back(aux_flt);
						}else if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name())){
							v_out.push_back((zs_int)sv->stk_value);
						}else{
							THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<float>");
							return v_out;
						}
						break;
					default:
					case MSK_STK_PROPERTY_UNDEFINED:
					case MSK_STK_PROPERTY_BOOL:
					case MSK_STK_PROPERTY_FUNCTION:
						THROW_RUNTIME_ERROR("type not implemented yet");
						return v_out;
						break;
					case MSK_STK_PROPERTY_ZS_INT:
						if(ZS_STRCMP(dst_convert_type, ==,typeid(int).name()) || ZS_STRCMP(dst_convert_type, ==,typeid(float).name())){// typeid(int).name()) || ){
							v_out.push_back((zs_int)sv->stk_value);
						}else{
							THROW_RUNTIME_ERROR("Error trying to cast element on std::vector<int>");
							return v_out;
						}
						break;
				}

			}
			return v_out;
		}

		static void    			pushSf(ScriptObjectVector *sv,StackElement  * stk);
		static StackElement *  	popSf(ScriptObjectVector *sv);

		ScriptObjectVector(ZetScript *_zs);

		virtual StackElement * 	getElementAt(short idx);
		virtual zs_int count();
		zs_vector * getAllElements();

		StackElement *			newSlot();
		void 					push(StackElement  * stk);
		StackElement *			pop();

		virtual ~ScriptObjectVector();
	protected:
		StackElement 		stk_count;
		zs_vector stk_elements;


		bool 				eraseElementAt(short idx);

	private:


	};

}
