var start=System::clock();

while((System::clock()-start) < 1){
	var subtime_start=System::clock()
	while((System::clock()-subtime_start) < 0.1){}
	Console::outln("Ellapsed seconds: {0}",System::clock()-start);
}