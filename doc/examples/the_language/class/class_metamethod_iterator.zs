class ContainerIterator{
	constructor(_container){
		this.container=_container;
		this.index=0;
	}
	_get(){
		// returns 2 values: index as key and this.container.data[this.index] as value
		return this.index,this.container.data[this.index];
	}
	_postinc(){
		if(this._end()){
			return;
		}
		this.index++;
		
	}
	_end(){
		return this.index>=this.container.data.length;
	}
}

class Container{
	constructor(){
		this.data=[10,20,30,40,50,60,70]
	}
	
	_iter(){
		return new ContainerIterator(this)
	}
}

var container=new Container();


// iterate over all elements
for(var k,v in container){
	Console::outln("key: {0}  value: {1}",k,v)
}