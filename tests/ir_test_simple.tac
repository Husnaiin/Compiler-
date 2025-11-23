# Global Variables
.global int x
.global int y

# Function: add (returns int)
.function add
# Parameters: a, b
    t0 = a + b
    return t0
.end_function

# Function: main (returns int)
.function main
    param x
    param y
    t0 = call add, 2
    result = t0
    print result
    t1 = result > 10
    if (!t1) goto L0
    print 1
    goto L1
L0:
    print 0
L1:
    i = 0
L2:
    t2 = i < 5
    if (!t2) goto L3
    print i
    t3 = i + 1
    i = t3
    goto L2
L3:
    return 0
.end_function

