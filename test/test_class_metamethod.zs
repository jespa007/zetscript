class Vector2{

	Vector2(_x=0,_y=0){
		this.x=_x;
		this.y=_y;
	}
	
	// defines _add metamethod...
	/*_add(v1){
		
		return new Vector2(
				this.x+v1.x
				,this.y+v1.y);
	}
	*/
	_next(){
		this.x++;
		this.y++;
	}

	_toString(){ // a way to have a custom string output
		return "["+this.x+","+this.y+"]";
	}
};

//var v1=new Vector2(1,2)
//var v2=(new Vector2(3,4))+(new Vector2(5,6))
var v=new Vector2()
//Console::outln(v._toString());
//Console::outln(v++) // it will read vector [x,y] and postincrement
Console::outln(v++)
//Console::outln(v++) // it will read vector [x,y] and postincrement

//v=v+v; // concatenates vector because v reads a vector through _get.


//Console::outln("key press: {0}",Console::readChar())

//print(new Vector2(0,2)); // it prints each new vector
//print((new Vector2(0,2)+new Vector2(1,0)+new Vector2())); // it sums and then it prints*/

