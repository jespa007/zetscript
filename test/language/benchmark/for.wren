class Sum {
  static test_sum() {
		var list = []
		for (i in 0...1000000) list.add(i)
		
		var sum = 0
		for (i in list) sum = sum + i
		return sum
	}
}


for (i in 1..10) {
  var start = System.clock
  System.print("sum: %(Sum.test_sum()) time: %(System.clock - start)")
}

