import numpy as np

def temper(params, h):
    k = params['k']
    u0 = params['u0']
    l = params['l']
    T = params['T']
    tau = params['tau']

    arr = np.ones(len(np.linspace(0, l, l/h+1))) * u0
    arr[0] = 0
    arr[-1] = 0

    flag = 1
    temparr = np.zeros(arr.shape)
    for _ in np.linspace(0, T, T/tau):
        if flag: 
            for i in range(1, len(arr)-1):       
                temparr[i] = arr[i] + k * tau * (arr[i-1] - 2*arr[i] + arr[i+1]) / (h**2)
            flag = 0
        else:
            for i in range(1, len(arr)-1): 
                arr[i] = temparr[i] + k * tau * (temparr[i-1] - 2*temparr[i] + temparr[i+1]) / (h**2)
            flag = 1

    if flag:
        return arr
    else:
        return temparr


def u(x, t, params):
    k = params['k']
    u0 = params['u0']
    l = params['l']

    summ = 0
    for m in range(10000):
        summ += (np.exp(-k * np.pi**2 * (2*m+1)**2 * t / (l**2))
                 * np.sin(np.pi * x * (2*m + 1) / l)
                 / (2*m + 1))
    
    return 4*u0 * summ / np.pi

params = {
    'k': 1.0,
    'u0': 1.0,
    'l': 1.0,
    'tau': 0.0002,
    'T': 0.1,
}

h = 0.02
t = 0.1

li = []
for x in np.linspace(0, 1, 1/h + 1):
    li.append(u(x, t, params))

arr = temper(params, h)

print(len(li))
print('[' + ', '.join(['%.2f']*len(li)) % tuple(li) + ']')
print(len(arr))
print('[' + ', '.join(['%.2f']*len(arr)) % tuple(arr) + ']')