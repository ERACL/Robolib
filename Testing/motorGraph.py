import matplotlib.pyplot as plt

Vc = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
Vm = [0.408, 0.848, 1.830, 2.408, 2.778, 3.004, 3.194, 3.310, 3.370, 3.416, 3.530]
plt.title("Vitesse (Encode / ms) efd Puissance (robotC)")
plt.plot(Vc,Vm,'-x')
plt.show()
