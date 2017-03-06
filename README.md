# ZG Script
	ZG script is a basic script engine for your c++ project.


Whitout optimization.



<b>for_test.zs</b> has this code,

var j=0;for(var i=0; i < 10000000; ++i){i*2;}

Generates this following asm,


[01:00]	LOAD	VAR(i)
[01:01]	LOAD	CONST(10000000)
[01:02]	LT	[01:00],[01:01]
[01:03]	JNT	[0004:-001]
[02:00]	LOAD	VAR(i)
[02:01]	LOAD	VAR(i)
[02:02]	LOAD	CONST(1)
[02:03]	ADD	[02:01],[02:02]
[02:04]	MOV	[02:00],[02:03]
[02:05]	POP_SCOPE(2)
[03:00]	LOAD	++VAR(i)
[03:01]	JMP	[0001:-001]




Total 12 instructions


Is performed 10000000 times within 1.5 seconds so,

(12*10000000)/1.6= 80000000 => 80MIPS ~ better than XXXX 

Comparison tables https://en.wikipedia.org/wiki/Instructions_per_second


#-------------------------------

JAVASCRIPT (https://www.w3schools.com/jsref/tryit.asp?filename=tryjsref_regexp_test2)

<b>for_test.js</b> has this code

var start = Date.now();
for(var i=0; i < 10000000; ++i)
{
	i=i+1;
}
var end = Date.now();

console.log("time:"+(end-start)+" ms");

Javascript times 0.013s => 1600/13 = x123 (faster)

#--------------------------------
LUA

<b>for_test.lua</b> has this code

for i=0,10000000 do 
  i=i+1; 
end



Lua 5.2 times 0.112s =>  1600/113 = x14,15 (faster)

#------------------------------

SQUIRREL

<b>for_test.nut</b> has this code

for(local i=0;i < 10000000;++i){i=i+1;}

Chaiscript times 0.163s => 1600/163 = x9.81 (faster)

#------------------------------

PYTHON

<b>for_test.py</b> has this code

for i in xrange(10000000):
	i=i+1

Python times 0.566s => 1600/602 = x2.66 (faster)

#------------------------------

CHAISCRIPT

<b>for_test.chai</b> has this code

for(var i=0; i < 10000000; ++i){i=i+1;}

Chaiscript times 20824s => so the zg is better 20824/1600 = x13,13 (slower)





