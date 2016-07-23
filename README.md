# ZG Script
	ZG script is a basic script engine for your c++ project.


This code

var j=0;for(var i=0; i < 10000000; ++i){i*2;}

Generates this following asm,

[02:00]	LOAD	VAR(i)
[02:01]	LOAD	CONST(10000000)
[02:02]	LT	[02:00],[02:01]
[02:03]	JNT	[0005]
[03:00]	LOAD	VAR(i)
[03:01]	LOAD	CONST(2)
[03:02]	MUL	[03:00],[03:01]
[04:00]	LOAD	++VAR(i)
[04:01]	JMP	[0002]

Total 9 instructions


Is performed 10000000 times within 6,8 seconds so,

(9*10000000)/6,8= 13235294 => 13MIPS ~ GBA and better than 486DX 11MHz (1991) 


 
