import numpy as np
import matplotlib.pyplot as plt

# Create a figure of size 8x6 inches, 80 dots per inch
plt.figure(figsize=(8, 6), dpi=80)

X = []
Y = []

with open('errored685.txt','r') as fin:
    for line in fin:
        t = line.split()
        X.append( t[0] )
        Y.append( t[1] )

print( len( X ) )
print(X)

plt.plot(X, Y)
plt.plot(X, Y, 'ro')


# Show result on screen
plt.show()
