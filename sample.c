int main() {
    int a = 10;
    float b;
    b = 10.5;
    read b;              // Read a floating point value into b
    a = a + 5;           // Addition operator
    print a;
    
    int c = a % 3;       // Modulus operator
    print c;
    
    a++;                 // Postfix increment operator
    print a;
    
    if (a > b) {         // Using relational operator ">"
        int d = a * 2;   // Multiplication operator; declaration inside block

        if(d > 10){      //nested if else
           print d;
        }else{
           d = add(a,c);
        }

    } else {
        float d = b / 2; // Division operator; declaration inside block
        print d;
    }
    
    if (a == b) {         // Using relational operator "==" for equality test
        int e = a - 1;   // Subtraction operator
        print e;
    } else {
        int e = a + 1;   // Addition operator
        print e;
    }
    
    helper(a, b);        // Call to another function
    return 0;
}

void helper(int x, float y) {
    int result;
    result = x - 2;      // Simple arithmetic operation
    print result;
}

float add (float x, float y){
    return x+y;
}