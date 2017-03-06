# ZG Script
	ZG script is a basic script engine for your c++ project.


Whitout optimization.



<b>for_test.zs</b> has this code,

var j=0;for(var i=0; i < 10000000; ++i){i*2;}

Generates this following asm,

[02:00]	LOAD	VAR(i)
[02:01]	LOAD	CONST(10000000)
[02:02]	LT	[02:00],[02:01]
[02:03]	JNT	[0005:-001]
[03:00]	LOAD	VAR(i)
[03:01]	LOAD	CONST(2)
[03:02]	MUL	[03:00],[03:01]
[03:03]	POP_SCOPE(2)
[04:00]	LOAD	++VAR(i)
[04:01]	JMP	[0002:-001]


Total 10 instructions


Is performed 10000000 times within 2,2 seconds so,

(10*10000000)/2,2= 45454545 => 45MIPS ~ better than 486DX2 66MHz (1992) 

Comparison tables https://en.wikipedia.org/wiki/Instructions_per_second


#-------------------------------

JAVASCRIPT (https://www.w3schools.com/jsref/tryit.asp?filename=tryjsref_regexp_test2)

<b>for_test.js</b> has this code

var start = Date.now();
var j=0;for(var i=0; i < 10000000; ++i)
{
	i*2;
}
var end = Date.now();

console.log("time:"+(end-start)+" ms");

Javascript times 0.047s => 2200/47 = x46.80 (faster)

#--------------------------------
LUA

<b>for_test.lua</b> has this code

j=0;
i=0;
for i=0,10000000 do 
  i=i*2; 
end



Lua 5.2 times 0.112s =>  2200/112 = x19.64 (faster)

#------------------------------

SQUIRREL

<b>for_test.nut</b> has this code

local j=0;for(local i=0;i < 10000000;++i){j*2;}

Chaiscript times 0.357s => 2200/357 = x6 (faster)

#------------------------------

CHAISCRIPT

<b>for_test.chai</b> has this code

var j=0;for(var i=0; i < 10000000; ++i){i*2;}

Chaiscript times 31.092s => so the zg is better 31092/2200 = x14,13





