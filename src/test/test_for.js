var start = Date.now();
var j=0;for(var i=0; i < 10000000; ++i)
{
	i*2;
}
var end = Date.now();

console.log("time:"+(end-start)+" ms");