// Ported from the Python version.

class Tree {
  constructor(item, depth) {
    
    this.item = item
    if (depth > 0) {
      var item2 = item + item
      depth = depth - 1
      this.left = new Tree(item2 - 1, depth)
      this.right = new Tree(item2, depth)
    }
  }

  check() {
    if (this.left == undefined) {
      return this.item
    }
    return this.item + this.left.check() - this.right.check()
  }
}

var minDepth = 4
var maxDepth = 12
var stretchDepth = maxDepth + 1;

var start = System::clock();

Console::outln("stretch tree of depth {0} check: "+
    "{1}",stretchDepth,(new Tree(0, stretchDepth)).check())

var longLivedTree = new Tree(0, maxDepth)

// iterations = 2 ** maxDepth
var iterations = 1
for (var d=0; d < maxDepth;d++) {
  iterations = iterations * 2
}

var depth = minDepth
while (depth < stretchDepth) {
  var check = 0
  for (var i=0; i < iterations;i++) {
    check = check + (new Tree(i, depth)).check() + (new Tree(-i, depth)).check()
  }

  Console::outln("{0} trees of depth {1} check: {2}",iterations * 2,depth,check)
  iterations = iterations / 4
  depth = depth + 2
}

Console::outln(
    "long lived tree of depth {0} check: {1}",maxDepth,longLivedTree.check())
Console::outln("elapsed: {0}",System::clock() - start)
