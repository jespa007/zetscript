/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class RefDictionaryScriptObject : public ScriptObject{
	public:
		RefDictionaryScriptObject(ZetScript *_zs, ScriptObject *_so_ref);
		void setRefObject(ScriptObject *_ref_object);
		ScriptObject *getRefObject();
		~RefDictionaryScriptObject();


	protected:
		ListNode<RefDictionaryScriptObject *> *ref_object_node;
		ScriptObject *ref_object;

		void dettachRefObject();
	};

}
