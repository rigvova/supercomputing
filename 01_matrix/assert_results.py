import numpy as np

# Load results from C code and numpy
with open('01_matrix/cresult.txt', 'r') as f:
    c_arr = np.array(list(map(float, f.readline().split())))
with open('01_matrix/pyresult.txt', 'r') as f:
    py_arr = np.array(list(map(float, f.readline().split())))

# Check if there are any differences in the arrays' values
close = (~np.isclose(c_arr, py_arr)).sum() 
if close == 0:
    print("All values are correct")
else:
    print("Error:")
    print("Values from C :", c_arr(~np.isclose(c_arr, py_arr)))
    print("Values from Py:", py_arr(~np.isclose(c_arr, py_arr)))