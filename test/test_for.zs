var v=[0,1,2,3,4,5,6,7,8,9,10];

// using i as idx
for(var i=0; i<v.length;i++){ 

	// ignores first element...
	if(i==1) { // ---> it does corruption memory
		break;
	}
	print("v["+i+"]="+v[i]);
	
}
/*
// k as iterator
for(var k in v){

	// it exits loop when k==8... 
	if(k==8){
		break;
	}
	print("v["+k+"]:"+v[k]);
}*/