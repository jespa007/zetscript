var start = Date.now();
for(var i=0; i < 10000000; ++i){i=i+1;}
var end = Date.now();

console.log("time:"+(end-start)+" ms");