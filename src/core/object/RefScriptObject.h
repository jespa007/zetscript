/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class RefScriptObject : public ScriptObject{
	public:
		RefScriptObject(ScriptEngine *_zs, ScriptObject *_so_ref);
		void setRefObject(ScriptObject *_ref_object);
		ScriptObject *getRefObject();
		~RefScriptObject();


	protected:
		ListNode<RefScriptObject *> *ref_object_node;
		ScriptObject *ref_object;

		void dettachRefObject();
	};

}
