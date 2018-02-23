import numpy as np
import matplotlib.pyplot as plot

a= np.loadtxt('out.dat')
for i in range(50):
	b=a[i,:]
	b=b[1::2]
	plot.hist(b,bins=50)
plot.show()
