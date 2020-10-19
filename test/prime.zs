function isprime(n)
{
    for (var i = 2; i < n; ++i){
        if (n % i == 0){
            return false;
        }
    }
    return true;
}

function primes(n)
{
    var count = 0;
    for (var i = 2; i <= n; ++i){
        if (isprime(i)){
            ++count;
        }
    }
    return count;
}

var start=System.clock();
print("primes: " + primes(200000) + " time:"+(System.clock()-start)+"ms");