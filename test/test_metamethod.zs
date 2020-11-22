class Vector2{

	constructor(_x,_y){
		if(_x==undefined){
			_x=0;
		}
		
		if(_y==undefined){
			_y=0;
		}
		
		this.x=_x;
		this.y=_y;
	}
	
	// defines _add metamethod...
	_add(v1,v2){
		var v3=new Vector2();
		v3.x=v1.x + v2.x;
		v3.y=v1.y + v2.y;
		return v3;
	}
	
	_toString(){ // a way to have a custom string output
		return "("+this.x+","+this.y+")";
	}
};

var v=(new Vector2(0,2)+new Vector2(0,2));//+new Vector2()
//var v=new Vector2(0,2)+new Vector2(0,2)
//var v=new Vector2();
System::println(v)

System::println("key press: {0}",System::readChar())

//print(new Vector2(0,2)); // it prints each new vector
//print((new Vector2(0,2)+new Vector2(1,0)+new Vector2())); // it sums and then it prints*/

