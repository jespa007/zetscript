function swap(ref _a, ref _b){
	var tmp=_a
	_a=_b;
	_b=tmp;
}

var i=5,j=10;

Console::outln("i:{0} j:{1}",i,j)

swap(i,j)

Console::outln("swap(i,j) => i:{0} j:{1}",i,j)

