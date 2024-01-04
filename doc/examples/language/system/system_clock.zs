while(System::clock() < 1){
	var start=System::clock()
	while((System::clock()-start) < 0.1){}
	Console::outln("Ellapsed seconds: {0}",System::clock());
}