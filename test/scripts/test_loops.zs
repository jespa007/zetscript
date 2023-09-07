
// for
var k=0;
for(var i=0;i < 10; i++){ 
	k++;
}

System::assert(k==10,"1.for k!=10");

k=0;
for(k=0;k < 10; k++){ 
}

System::assert(k==10,"2.for k!=10");

k=0;
while(k < 10){
	k++;
}

System::assert(k==10,"3.while k!=10");

k=0;
do{
	k++;
}while(k<10);

System::assert(k==10,"4.do-while k!=10");