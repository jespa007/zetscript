class Data{
	constructor(){
		this.data=[0,1,1,10,3,4,6]
	} 
	
 	_in(_value){
 		return _value in this.data;
 	}
};

var data=new Data();

if(10 in data){
	Console::outln("10 is content in data")
}

