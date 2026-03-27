Um_list = [66.5, 65.5, 62.7, 58.5, 53.4, 47.8, 37.0, 32.3, 28.1, 24.5]
x_list = [0.01, 0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09]
BmCe_list = []
BmJi_list = []
Er_list = []

for i, (Um,x) in enumerate(zip(Um_list, x_list)):
    BmCe = Um * 5.958 * (10**(-4))/ (6100 * (11.7**2)) * (10**8)
    BmJi = 12.57 * (10**(-7)) * 6100 * 0.01 * 0.1/ (2**(1/2) * (0.01 + x**2)**(3/2)) * (10**3)
    Er = abs(BmCe - BmJi) / BmJi * 100
    
    BmCe_list.append(BmCe)
    BmJi_list.append(BmJi)
    Er_list.append(Er)

import matplotlib.pyplot as plt
plt.plot(x_list, BmCe_list, label='BmCe')
plt.plot(x_list, BmJi_list, label='BmJi', linestyle='--')
plt.legend()
plt.show()
