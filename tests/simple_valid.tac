# Function: add (returns int)
.function add
# Parameters: a, b
    t0 = a + b
    return t0
.end_function

# Function: multiply (returns float)
.function multiply
# Parameters: x, y
    t0 = x * y
    return t0
.end_function

# Function: max (returns int)
.function max
# Parameters: a, b
    t0 = a > b
    if (!t0) goto L0
    return a
    goto L1
L0:
    return b
L1:
.end_function

# Function: factorial (returns int)
.function factorial
# Parameters: n
    result = 1
    i = 1
L0:
    t0 = i <= n
    if (!t0) goto L1
    t1 = result * i
    result = t1
    t2 = i + 1
    i = t2
    goto L0
L1:
    return result
.end_function

# Function: isPositive (returns bool)
.function isPositive
# Parameters: x
    t0 = x > 0
    return t0
.end_function

# Function: main (returns void)
.function main
    x = 10
    y = 20
    param x
    param y
    t0 = call add, 2
    sum = t0
    pi = 3.14
    param pi
    param 2.0
    t1 = call multiply, 2
    doubled = t1
    param x
    param y
    t2 = call max, 2
    bigger = t2
    param 5
    t3 = call factorial, 1
    fact = t3
    param x
    t4 = call isPositive, 1
    positive = t4
    print sum
    print doubled
    print bigger
    print fact
    print positive
    return
.end_function

