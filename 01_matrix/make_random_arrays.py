import numpy as np

# Init shapes
M = np.random.randint(1, 6)
N = np.random.randint(1, 6)
K = np.random.randint(1, 6)

# Init matrices
A = 10 * np.random.random((M, N)) - 5
B = 10 * np.random.random((N, K)) - 5

# Compute matrix multiplication using numpy
C = np.matmul(A, B)

# Write A and B matrices data in file
with open('01_matrix/data.txt', 'w') as f:
    f.write(str(M)+'\n')
    f.write(str(N)+'\n')
    f.write(str(K)+'\n')
    f.write(" ".join(map(str, np.ravel(A.T)))+'\n')
    f.write(" ".join(map(str, np.ravel(B.T))))

# Write matrix mul result in file
with open('01_matrix/pyresult.txt', 'w') as f:
    f.write(" ".join(map(str, np.ravel(C.T))))