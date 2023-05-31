#include "zetscript.h"

using zetscript::ZetScript;
using zetscript::ObjectScriptObject;
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

  // Evaluates ZetScript class A and function 'returnNewA' that returns an instance of type 'A'
  zs.eval(
    "// 'returnPoint' instances 'Point' type\n"
    "function returnPoint(){\n"
    "   return new Point();\n"
    "}\n"
  );

  // It binds 'returnPoint' as '(ClassScriptObject *)(void)'
  auto returnPoint=zs.bindScriptFunction<ObjectScriptObject *()>("returnPoint");

  // Calls ZetScript function which it returns 'ObjectScriptObject *' reference
  auto point=returnPoint();

  // Prints return value by console.
  printf("result : %s\n",point->toString().c_str());

  // 'unrefLifetimeObject' it decreases the reference count of thr script object to tell is not used anymore
  zs.unrefLifetimeObject(point);    

 	return 0;
}

