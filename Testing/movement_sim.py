import matplotlib.pyplot as plt
import numpy as np
from math import *

#Toutes les valeurs sont en USI
#Tullius : 1 encode ~ 0.75 mm ; V(P=50) = 0.375 m/s

#Constantes de simulation
T = 20
N = 10000
dt = T / N

#Constantes du robot
betweenWheels = 0.2
tauMotors = 0.2

#Point ciblé
XT = -1
YT = 1

#Variables de mouvement
X = np.zeros(N)
Y = np.zeros(N)
Ori = np.zeros(N)

VRc = np.zeros(N)
VLc = np.zeros(N)
VStr = np.zeros(N)
VRot = np.zeros(N)
VL = np.zeros(N)
VR = np.zeros(N)

A = np.zeros(N)
A_modPi = np.zeros(N)
D = np.zeros(N)
D_proj = np.zeros(N)

#Constantes d'asservissement
controlPeriod = 0.05
KPStr = 0.7
KIStr = 1
KPRot = 0.2
KIRot = 0.05
maxAccel = 0.6
maxSpeed = 0.3
dist_goodEnough = 0.005
dist_close = 0.1
angle_close = np.pi / 12
modelizeMotors = True

#Variables d'asservissement
i_D_proj = 0
i_A_modPi = 0

def limit(num, lim1, lim2 = None):
    if lim2 == None:
        if num > lim1: return lim1
        if num < -lim1: return -lim1
        return num
    else:
        if num < lim1: return lim1
        if num > lim2: return lim2
        return num

def mod(angle):
    while angle > np.pi:
        angle -= 2 * np.pi
    while angle < -np.pi:
        angle += 2 * np.pi
    return angle

for i in range(1, N-1):
    VRc[i] = VRc[i-1] #Par défaut, la vitesse de consigne est constante
    VLc[i] = VLc[i-1]
    VRot[i] = VRot[i-1] #Utile pour la visualisation
    VStr[i] = VStr[i-1]

    #Calcul des variables de position
    D[i] = sqrt((Y[i] - YT)**2 + (X[i] - XT)**2)

    A[i] = np.pi - Ori[i]
    if XT - X[i] + D[i] != 0:
        A[i] = mod(2 * atan((YT - Y[i]) / (XT - X[i] + D[i])) - Ori[i])

    D_proj[i] = cos(A[i]) * D[i]

    A_modPi[i] = A[i]
    if abs(A_modPi[i] + np.pi) < abs(A_modPi[i]): A_modPi[i] += np.pi
    if abs(A_modPi[i] - np.pi) < abs(A_modPi[i]): A_modPi[i] -= np.pi

    #Asservissement environ tous les controlPeriod
    if abs(i * dt % controlPeriod) < dt / 2:
        #Intégrations partielles : on ne commence à intégrer qu'à partir de seuils de distance ou d'angle

        i_D_proj += (D_proj[i] + D_proj[i-1]) / 2 * controlPeriod
        i_A_modPi += (A_modPi[i] + A_modPi[i-1]) / 2 * controlPeriod

        #Formules d'asservissement de la vitesse de consigne
        dVRot, dVStr = 0, 0
        if D[i] > dist_close:
            if abs(A[i]) > angle_close: #Phase 1 : Rotation simple, avancée nulle
                dVRot = KPRot * A[i] - VRot[i-1]
                i_A_modPi = 0
                dVStr = -VStr[i-1]
                i_D_proj = 0
            else: #Phase 2 : Rotation intégrée, avancée simple
                dVRot = KPRot * A_modPi[i] + KIRot * i_A_modPi - VRot[i-1]
                dVStr = KPStr * D_proj[i] - VStr[i-1]
                i_D_proj = 0
        elif D[i] > dist_goodEnough: #Phase 3 : Rotation intégrée, avancée intégrée
            dVRot = KPRot * A_modPi[i] + KIRot * i_A_modPi - VRot[i-1]
            dVStr = KPStr * D_proj[i] + KIStr * i_D_proj - VStr[i-1]
        else: #Phase 4 : Arrêt
            dVRot = -VRot[i-1]
            dVStr = -VStr[i-1]

        dVRot = limit(dVRot, maxAccel * 0.5 * controlPeriod)
        dVStr = limit(dVStr, maxAccel * controlPeriod - abs(dVRot))
        VRot[i] = limit(VRot[i-1] + dVRot, maxSpeed * 0.5)
        VStr[i] = limit(VStr[i-1] + dVStr, maxSpeed - abs(VRot[i]))

        VRc[i] = VStr[i] + VRot[i]
        VLc[i] = VStr[i] - VRot[i]

    #Modélisation des moteurs (premier ordre)
    if modelizeMotors:
        if abs(VRc[i]) < 0.03: VR[i] = 0
        else: VR[i] = dt / (dt + 2 * tauMotors) * (VRc[i] + VRc[i-1] + (2 * tauMotors / dt - 1) * VR[i-1])
        if abs(VLc[i]) < 0.03: VL[i] = 0
        else: VL[i] = dt / (dt + 2 * tauMotors) * (VLc[i] + VLc[i-1] + (2 * tauMotors / dt - 1) * VL[i-1])
    else:
        VR[i] = VRc[i]
        VL[i] = VLc[i]

    #Mise à jour de la position (en dehors de l'asservissement)
    X[i+1] = X[i] + (VL[i] + VL[i-1] + VR[i] + VR[i-1]) / 4 * dt * cos(Ori[i])
    Y[i+1] = Y[i] + (VL[i] + VL[i-1] + VR[i] + VR[i-1]) / 4 * dt * sin(Ori[i])
    Ori[i+1] = mod(Ori[i] + (VR[i] + VR[i-1] - VL[i] - VL[i-1]) * dt / betweenWheels / 4)

if True :
    plt.axes().set_aspect('equal', 'datalim')
    plt.plot(X[::150], Y[::150], 'r.')
    plt.plot(0, 0, 'bo')
    plt.plot(X[N-1], Y[N-1], 'bX')
    plt.plot(XT, YT, 'gX')

if True :
    plt.figure()
    Time = np.linspace(0, T, N-2)
    plt.plot(Time, ((VL + VR) / 2)[1:-1])
    plt.plot(Time, ((VR - VL) / 2)[1:-1])

plt.show()
