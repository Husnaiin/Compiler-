# Global Variables
.global int global_x
.global int global_y

# Function: add (returns int)
.function add
# Parameters: a, b
    t0 = a + b
    return t0
.end_function

# Function: add (returns int)
.function add
# Parameters: a, b, c
    return 0
.end_function

# Function: main (returns int)
.function main
    nested = local_x
    local_x = global_x
    param global_x
    param global_y
    t0 = call add, 2
    local_y = t0
    return 0
.end_function

