import matplotlib.pyplot as plt
import numpy as np 

x = np.arange(0,10,0.1)

y1 = 10 - x
y2 = 10 - x + np.sin(5*x)
y3 = 10 - x + np.sin(5*x) + np.sin(950000*x)

# color='green', linestyle='dashed', linewidth = 3,
#          marker='o', markerfacecolor='blue', markersize=12

plt.plot(x, y1, label = "Path Loss Alone", color='black')
plt.plot(x, y2, label = "Shadowing and path Loss", color='black', linestyle='dashed')
plt.plot(x, y3, label = "Multipath, Shadowing and path Loss", color='black', linestyle='dotted')

plt.xlabel('Log(distance)')
plt.ylabel('Signal Strength (dB)') 
plt.legend()
plt.show()
