import matplotlib.pyplot as plt
import numpy as np

Te = 20
T = 10000
N = int(T / Te)
Km = 0.01
Kp = 0.3
Ki = 0.003
taum = 100
mmPerEncode = 0.61
minRegisteredPower = 5
maxAllowedPowerDerivative = 0.2
integralControlDistance = 10 / mmPerEncode

E = 500 / mmPerEncode
S = np.zeros(N + 1)
sumDeltaD = 0
lastVc = 0
lastV = 0

for i in range(1, N+1):
    #S[i] = 1/taum*(Km*Kp*Te**2*E+Km*Te**2*(Ki*Te-Kp)*E-(Te-3*taum)*S[i-1]-(Km*Kp*Te**2-2*Te+3*taum)*S[i-2]-(Km*Ki*Te**3-Km*Kp*Te**2+Te-taum)*S[i-3])
    deltaD = (E - S[i-1])
    if (abs(deltaD) < integralControlDistance):
        sumDeltaD += deltaD * Te
    
    vc = Kp*deltaD + Ki * sumDeltaD    
    
    if vc - lastVc > maxAllowedPowerDerivative * Te:
        vc = lastVc + maxAllowedPowerDerivative * Te
    elif vc - lastVc < -maxAllowedPowerDerivative * Te:
        vc = lastVc - maxAllowedPowerDerivative * Te
    lastVc = vc
    
    if (abs(vc) < minRegisteredPower):
        v = 0
    else:
        if vc > 50:
            vc = 50
        elif vc < -50:
            vc = -50
        v = Te / (taum + Te) * (Km*vc + taum / Te * lastV)
    
    lastV = v
    S[i] = S[i-1] + v * Te

plt.plot(np.linspace(0, T, N),S[1:]*mmPerEncode)
print()
print(S[i]*mmPerEncode)
plt.show()