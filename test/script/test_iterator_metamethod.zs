class CustomObjectIterator{
	constructor(_cv){
		this.cv=_cv		
		this.idx=0
	}
	
	_get(){
		return this.cv.data[this.idx]
	}
	
	_postinc(){
		if(this._end() == false){
			this.idx++;
		}
	}
	
	_end(){
		return this.idx==this.cv.data.length
	}
}

class CustomVector{
	var data=[1,2,3,4]
	
	_iter(){
		return new CustomObjectIterator(this)
	}
}

var data=new CustomVector([1,2,3,4])

for(var d in data){
	Console::outln(d)
}