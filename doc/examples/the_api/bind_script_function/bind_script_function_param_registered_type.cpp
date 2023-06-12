#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::zs_int;

// Class Point to register
struct Point{
	int x,y;

	Point(){
		x=0;
		y=0;
	}

   Point(int _x, int _y){
		x=_x;
		y=_y;
	}
};

//------------------------------
// Point class functions to register

// defines getter property Point::x ClassScriptObject
zs_int Point_get_x(ZetScript *_zs, Point *_this){
	return _this->x;
}

// defines getter property Point::y ClassScriptObject
zs_int Point_get_y(ZetScript *_zs, Point *_this){
	return _this->y;
}


// 
//------------------------------

int main()
{
	ZetScript zs;

   // Register class Point
	zs.registerClass<Point>("Point");

	// register property getter Point::x
	zs.registerMemberPropertyGetter<Point>("x",Point_get_x);

	// register property getter Point::y
	zs.registerMemberPropertyGetter<Point>("y",Point_get_y);

    // Evaluates ZetScript function 'paramPoint' that prints the contents of '_point'
    zs.eval(
 		"function paramPoint(_point){\n"
        "    Console::outln(\"result : \"+_point);\n"
        "}\n"
 	);

    // It binds 'concat' as '(void *)(ArrayScriptObject *, ArrayScriptObject *)'
    auto paramPoint=zs.bindScriptFunction<void(Point * _point)>("paramPoint");

    // Prepare parameters
    auto point=Point(10,20);

    // Calls binded ZetScript function with parameters
    paramPoint(&point);


 	return 0;
}

