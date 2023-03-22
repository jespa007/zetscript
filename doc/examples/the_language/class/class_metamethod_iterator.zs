class ContainerIterator{
	constructor(_container){
		this.container=_container;
		this.index=0;
	}
	_get(){
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

for(var k,v in container){
	Console::outln("{0} {1}",k,v)
}