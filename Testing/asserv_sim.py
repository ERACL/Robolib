import matplotlib.pyplot as plt
import numpy as np

def limit(number, limit):
    if number > limit:
        return limit
    elif number < -limit:
        return -limit
    else:
        return number

Te = 20
T = 5000
N = int(T / Te)
Km = 0.01
Kp = 0.2
Ki = 0.0008
taum = 100
mmPerEncode = 0.61
minRegisteredPower = 5
maxAllowedPowerDerivative = 0.2
maxAllowedPower = 50
integralControlDistance = 15 / mmPerEncode
targetDiff = 1 / mmPerEncode

E = 500 / mmPerEncode
S = np.zeros(N + 1)
sumDeltaD = 0
VC = np.zeros(N + 1)
V = np.zeros(N + 1)

integralControl = False

for i in range(1, N+1):
    deltaD = (E - S[i-1])
    if (abs(deltaD) < integralControlDistance and integralControl == False):
        print("beep")
        integralControl = True
    if integralControl:
        sumDeltaD += deltaD * Te

    if abs(deltaD) < targetDiff:
        VC[i] = 0
    else:
        VC[i] = limit(VC[i-1] + limit(Kp*deltaD + Ki*sumDeltaD - VC[i-1], maxAllowedPowerDerivative * Te), maxAllowedPower)

    if (abs(VC[i]) < minRegisteredPower):
        V[i] = 1 / (1 + (taum / Te)) * taum * V[i-1] / Te
    else:
        V[i] = 1 / (1 + (taum / Te)) * (Km * VC[i] + taum * V[i-1] / Te)
        print(str(VC[i] - VC[i-1])[:6] +'\t'+ str((V[i] - V[i-1]) / Km)[:6])

    S[i] = S[i-1] + V[i] * Te

plt.subplot(131)
plt.plot(np.linspace(0, T, N),np.ones(N)*E*mmPerEncode, 'b')
plt.plot(np.linspace(0, T, N),S[1:]*mmPerEncode, 'r')
plt.subplot(132)
plt.plot(np.linspace(0, T, N),VC[1:], 'b')
plt.plot(np.linspace(0, T, N),V[1:]/Km, 'r')
plt.subplot(133)
plt.plot(np.linspace(0, T, N),VC[1:] - VC[:N], 'b')
plt.plot(np.linspace(0, T, N),(V[1:] - V[:N])/Km, 'r')

print()
print(S[i]*mmPerEncode)
plt.show()
