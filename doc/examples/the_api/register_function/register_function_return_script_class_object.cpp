#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ClassScriptObject;
using zetscript::zs_int;

// C structure to register
struct Point{
	int x,y;

	Point(){
		x=0;
		y=0;
	}
};

//------------------------------
// WRAP POINT

// defines new function Point ClassScriptObject
Point *PointWrap_new(ZetScript *_zs){
	return new  Point();
}

// defines getter property Point::x ClassScriptObject
zs_int PointWrap_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property Point::y ClassScriptObject
zs_int PointWrap_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}

// defines delete function Point ClassScriptObject
void PointWrap_delete(ZetScript *_zs, Point *_this){
	delete _this;
}

// WRAP POINT
//------------------------------

// C function that returns classScriptObject
ClassScriptObject *returnPoint(ZetScript *_zs){

	// define class script object
	ClassScriptObject *class_script_object=NULL;

	// instance point
	Point *point=new Point();

	// initialize x and y
	point->x=10;
	point->y=10;

	// instance new ClassScriptObject using ZetScript context and passing point instance
	class_script_object=_zs->newClassScriptObject(point);

	// return class script object
    return class_script_object;
}

int main(){
	ZetScript zs;

	// Register class Point as instanciable
	zs.registerClass<Point>("Point",PointWrap_new,PointWrap_delete);

	// register property getter Point::x
	zs.registerMemberPropertyGetter<Point>("x",PointWrap_get_x);

	// register property getter Point::y
	zs.registerMemberPropertyGetter<Point>("y",PointWrap_get_y);

	// register C function that returns Point ScriptClassObject
    zs.registerFunction("returnPoint",returnPoint);

    // Eval script that C function and prints the result by console
    zs.eval(
        "Console::outln(\"result : \"+returnPoint());"
 	);

    return 0;
}
