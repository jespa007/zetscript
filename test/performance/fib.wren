class Fib {
  static get(n) {
    if (n < 2) return n
    return get(n - 1) + get(n - 2)
  }
}

var start = System.clock
for (i in 1..10) {
  var start = System.clock
  System.print("fib: %(Fib.get(34)) time: %(System.clock - start)")
}



