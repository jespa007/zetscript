var v=[1,2,3,4];

var i=0,j=0;

for(
		var e
		,  _for_in_index1=0
		,  _for_in_count1=v.size()
		;--_for_in_count1>=0
		;i+=2, j+=3,_for_in_index1++
	){
	e=v[_for_in_index1];
	
	if(_for_in_index1 > 2){
		break;
	}

	
	if(_for_in_index1 % 2){
		continue;
	}
	
	print("e:"+e+" i:"+i+" j:"+j+" _for_in_index1"+_for_in_index1);	
	
}

i=0;
switch(i){
default:
case -1:
case 0:
	print("k");
case 1:
	print("kk");
	break;

}


i=0;
while(i < 10){
	
	print("i:"+i);

	if(++i > 2){
		break;
	}
}

foreach(e in v){
	print("e:"+e);
}

