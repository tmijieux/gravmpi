#!/usr/bin/env python2
import numpy as np
import matplotlib.pyplot as plt

l = raw_input().strip()
X = []
Y = []
while l != "":
    a,b = map(float, l.split())
    X.append(a)
    try:
        Z
    except:
        Z = b
    Y.append(Z / b)
    l =raw_input().strip()

print Y
plt.axis('equal')
plt.plot(X, Y, label="speed up")
plt.plot(X, X, label="X=Y")
plt.legend()
plt.xlabel("#proc")
plt.ylabel("speedup")
plt.show()
