

var s={
	i:0+1,
	j:"string1",
	k:[0,1,2,3],
	l:{
		i:2+3,
		j:"string2",
		k:[4,5,6,7],
		m:[
			{
				i:4+5,
				j:"string3",
				k:[8,9,10,11]
			},
			{
				i:6+7,
				j:"string4",
				k:[12,13,14,15]
			}
			]
	}
		
};

s.i=3;
print(""+(s.i++));
print(""+(s.i++));

