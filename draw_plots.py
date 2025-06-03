#!/usr/bin/env python3

import matplotlib.pyplot as plt


def plot_test_k():
    plt.figure(figsize=(8, 6))
    plt.gca().set_yscale('log', base=10)
    k = [x for x in range(32, 72, 8)]
    
    addition = [[6369426.752, 5952380.952, 5917159.763, 5780346.821, 5714285.714],
                [3703703.704, 3731343.284, 3636363.636, 3773584.906, 3508771.930],
                [2645502.646, 2604166.667, 2512562.814, 2450980.392, 2433090.024],
                [3508771.930, 3448275.862, 3460207.612, 3508771.930, 3448275.862]]
    multiplication = [[320307.4952, 317460.3175, 304692.2608, 318877.5510, 298953.6622],
                      [125219.1335, 123578.8433, 121212.1212, 122925.6300, 119789.1711],
                      [69671.84561, 66903.05747, 65724.61387, 65389.39384, 64164.26051],
                      [110132.1586, 106791.9692, 105019.9538, 101688.0212, 106814.7832]]

    colors = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple", "tab:brown", "tab:pink", "tab:gray"]

    plt.plot(k, addition[0], color=colors[0], label="add(d=8)", linestyle="-", marker="^")
    plt.plot(k, addition[1], color=colors[1], label="add(d=16)", linestyle="-.", marker="o")
    plt.plot(k, addition[2], color=colors[2], label="add(d=24)", linestyle="--", marker="+")
    plt.plot(k, addition[3], color=colors[3], label="add(d0=4,d1=2)", linestyle=":", marker="*")

    plt.plot(k, multiplication[0], color=colors[4], label="mult(d=8)", linestyle="-", marker="^")
    plt.plot(k, multiplication[1], color=colors[5], label="mult(d=16)", linestyle="-.", marker="o")
    plt.plot(k, multiplication[2], color=colors[6], label="mult(d=24)", linestyle="--", marker="+")
    plt.plot(k, multiplication[3], color=colors[7], label="mult(d0=4,d1=2)", linestyle=":", marker="*")

    plt.xlabel("k", fontsize=14)
    plt.ylabel("Ops/s", fontsize=14)
    plt.xticks(k)
    plt.yticks([10**5, 10**6, 10**7])
    plt.grid()
    plt.legend(fontsize=10, loc="center", framealpha=1, bbox_to_anchor=(0.5, 0.53))
    plt.show()
    
    
def plot_test_d():
    plt.figure(figsize=(8, 6))
    plt.gca().set_yscale('log', base=10)
    
    d = [x for x in range(6, 18, 2)]
    addition = [6802721.088, 5681818.182, 4950495.05, 3278688.525, 2710027.1, 2493765.586]
    multiplication = [659630.6069, 305436.7746, 262191.9245, 195924.7649, 156396.6218, 125219.1335]
    
    plt.plot(d, addition, color="tab:orange", label="add(k=32)", linestyle="-", marker="o")
    plt.plot(d, multiplication, color="tab:blue", label="mult(k=32)", linestyle="--", marker="s")

    plt.xlabel("d", fontsize=14)
    plt.ylabel("Ops/s", fontsize=14)
    plt.xticks(d)
    plt.yticks([10 ** 5, 10 ** 6, 10 ** 7])
    plt.ylim(10**5, 2*10**7)
    plt.grid()
    plt.legend(fontsize=10, loc="upper left", framealpha=1)
    plt.show()


def plot_test_d_extension():
    plt.figure(figsize=(8, 6))
    plt.gca().set_yscale('log', base=10)

    d = list(range(2, 26, 2))

    # 原始时间（ns）
    addition_k32 = [
        114.544, 138.004, 155.788, 189.522, 208.338, 241.657,
        293.519, 289.981, 348.245, 364.423, 379.104, 404.704
    ]
    multiplication_k32 = [
        665.174, 1018.51, 1531.87, 3072.95, 2837.77, 3667.95,
        4611.98, 7283.76, 6958.5, 8358.6, 9816.78, 13954.8
    ]

    addition_k64 = [
        90.5459, 127.891, 165.843, 204.488, 217.053, 241.076,
        260.148, 278.667, 310.085, 337.006, 363.552, 398.04
    ]
    multiplication_k64 = [
        631.914, 940.713, 1490.35, 3262.3, 3020.63, 3886.82,
        4806.26, 7464.14, 7072.84, 8980.24, 10262.2, 14426.1
    ]

    # 换算成 Ops/s
    def to_ops(times_ns):
        return [1e9 / t for t in times_ns]

    add32 = to_ops(addition_k32)
    mult32 = to_ops(multiplication_k32)
    add64 = to_ops(addition_k64)
    mult64 = to_ops(multiplication_k64)

    plt.plot(d, add32, label="add (k=32)", linestyle="-", marker="o", color="tab:orange")
    plt.plot(d, mult32, label="mult (k=32)", linestyle="--", marker="s", color="tab:blue")
    plt.plot(d, add64, label="add (k=64)", linestyle="-", marker="o", color="tab:green")
    plt.plot(d, mult64, label="mult (k=64)", linestyle="--", marker="s", color="tab:red")

    plt.xlabel("d", fontsize=14)
    plt.ylabel("Ops/s", fontsize=14)
    plt.xticks(d)
    plt.yticks([1e5, 1e6, 1e7])
    plt.ylim(1e5, 2e7)
    plt.grid()
    plt.legend(fontsize=10, loc="upper right", framealpha=1)
    plt.title("Galois Ring Addition and Multiplication Performance")
    plt.show()

def plot_test_d_towering():
    plt.figure(figsize=(8, 6))
    # plt.gca().set_xscale('log', base=2)
    plt.gca().set_yscale('log', base=10)
    d = ["(8, 2)", "(4, 4)", "(2, 8)", "(4, 2, 2)", "(2, 4 ,2)", "(2, 2, 4)", "(2, 2, 2, 2)"]
    addition = [2538071.066, 2192982.456, 1543209.877, 1385041.551, 1042752.868, 862068.9655, 554323.7251]
    multiplication = [37771.48253, 25963.23606, 9607.163101, 14350.08467, 5044.008978, 4515.36579, 2589.680641]

    plt.plot(d, addition, color="green", label="add(k=32)", linestyle="-", marker="^")
    plt.plot(d, multiplication, color="blue", label="mult(k=32)", linestyle="-", marker="^")

    plt.legend(fontsize=14)
    plt.xlabel("d", fontsize=14)
    plt.ylabel("Ops/s", fontsize=14)

    # plt.title("Updatable PSI Run Time", fontsize=20)

    plt.legend(fontsize=10, loc="upper left", framealpha=1)
    # plt.xlim(24, 68)
    # plt.ylim(10**5, 2*10**7)
    # plt.xticks(d)
    plt.yticks([10**x for x in range(3, 8)])
    plt.grid()
    plt.show()

def plot_ops():
    import numpy as np
    # 数据
    data = np.array([199.121, 3955, 438860, 1930.52])
    scaled_data = 1000000000 / data  # 计算比例

    # 配色方案
    colors = ['#1f77b4', '#2ca02c', '#17becf', '#98df8a'] 

    # 创建柱形图
    plt.figure(figsize=(8, 6))
    plt.bar(range(len(scaled_data)), scaled_data, color=colors, width=0.3, alpha=1, label='k = 64, d = 8, dl = 24', zorder=3)

    # 设置对数刻度的Y轴
    plt.yscale('log')
    plt.ylim(1e3, 1e7)  # 设置Y轴范围，以10^3到10^7为显示范围
    plt.yticks([1e3, 1e4, 1e5, 1e6, 1e7], labels=[r'$10^3$', r'$10^4$', r'$10^5$', r'$10^6$', r'$10^7$'])

    # 设置X轴标签
    plt.xticks(range(len(scaled_data)), labels=['add', 'mult', 'inv', 'lift'])

    # 添加标题和标签
    plt.legend(fontsize=14)
    plt.xlabel("operation", fontsize=14)
    plt.ylabel("Ops/s", fontsize=14)


    # 显示图形
    plt.grid(True, linestyle='--', linewidth=0.7, zorder=0)
    plt.show()



def plot_checks():
    import numpy as np
    # 数据
    data = np.array([199.121, 3955, 438860])
    scaled_data = 1000000000 / data  # 计算比例

    # 配色方案
    colors = ['#1f77b4', '#2ca02c', '#17becf', '#98df8a'] 

    # 创建柱形图
    plt.figure(figsize=(8, 6))
    plt.bar(range(len(scaled_data)), scaled_data, color=colors, width=0.3, alpha=1, label='k = 64, d = 8, dl = 24', zorder=3)

    # 设置对数刻度的Y轴
    plt.yscale('log')
    plt.ylim(1e3, 1e7)  # 设置Y轴范围，以10^3到10^7为显示范围
    plt.yticks([1e3, 1e4, 1e5, 1e6], labels=[r'$10^3$', r'$10^4$', r'$10^5$', r'$10^6$'])

    # 设置X轴标签
    plt.xticks(range(len(scaled_data)), labels=['sacrifice', 'inner prod', 'compressed'])

    # 添加标题和标签
    plt.legend(fontsize=14)
    plt.xlabel("operation", fontsize=14)
    plt.ylabel("Ops/s", fontsize=14)


    # 显示图形
    plt.grid(True, linestyle='--', linewidth=0.7, zorder=0)
    plt.show()


def plot_ka_variants():
    plt.figure(figsize=(12, 7))
    plt.gca().set_yscale('log', base=10)

    d = list(range(4, 26, 2))  # d = 4, 6, ..., 24

    # GR<16,d> 时间（单位：ns）
    plain_16 = [73407, 1515, 3121, 3716, 4098, 4728, 8204, 7881, 9382, 11138, 15608]
    ka_one_iter_16 = [1273, 1625, 3871, 3358, 4637, 6098, 9632, 9431, 11386, 13379, 19206]
    ka_recursive_16 = [2006, 3575, 6589, 8099, 11400, 14747, 19887, 21900, 29444, 36310, 39223]
    ka_recursive_dummy_16 = [1887, 6105, 6697, 17482, 17514, 18043, 19883, 54926, 54696, 55380, 58409]

    # GR<32,d> 时间（单位：ns）
    plain_32 = [94758, 1405, 2848, 3357, 3533, 4569, 7273, 6971, 8326, 11064, 14463]
    ka_one_iter_32 = [989, 1539, 3143, 3210, 4296, 5519, 8629, 8571, 10404, 14150, 17423]
    ka_recursive_32 = [1854, 3380, 6140, 7877, 10644, 14206, 17885, 20075, 26779, 36243, 36680]
    ka_recursive_dummy_32 = [1723, 5722, 6144, 16703, 16666, 16857, 23359, 49709, 51596, 52355, 57946]

    # GR<64,d> 时间（单位：ns）
    plain_64 = [94661, 1518, 3126, 3493, 3921, 4820, 8369, 7984, 9085, 10572, 14784]
    ka_one_iter_64 = [1304, 1676, 3442, 3305, 4355, 5583, 9138, 8974, 10554, 12044, 16783]
    ka_recursive_64 = [1834, 3322, 6325, 7464, 11422, 14218, 19032, 20917, 26881, 34049, 38368]
    ka_recursive_dummy_64 = [1705, 6275, 6448, 16928, 16831, 17498, 18930, 52580, 50523, 50334, 53356]

    def to_ops(times_ns):
        return [1e9 / t for t in times_ns]

    # 每种方法分别绘制 k=16, 32, 64 三条线
    def plot_series(label, data16, data32, data64, color, marker):
        plt.plot(d, to_ops(data16), label=f"{label} (k=16)", linestyle=":", marker=marker, color=color)
        plt.plot(d, to_ops(data32), label=f"{label} (k=32)", linestyle="-", marker=marker, color=color)
        plt.plot(d, to_ops(data64), label=f"{label} (k=64)", linestyle="--", marker=marker, color=color)

    plot_series("PLAIN", plain_16, plain_32, plain_64, "tab:orange", "o")
    plot_series("KA_ONE_ITER", ka_one_iter_16, ka_one_iter_32, ka_one_iter_64, "tab:blue", "s")
    plot_series("KA_RECURSIVE", ka_recursive_16, ka_recursive_32, ka_recursive_64, "tab:green", "^")
    plot_series("KA_RECURSIVE_DUMMY", ka_recursive_dummy_16, ka_recursive_dummy_32, ka_recursive_dummy_64, "tab:red", "x")

    plt.xlabel("d", fontsize=14)
    plt.ylabel("Ops/s (log scale)", fontsize=14)
    plt.xticks(d)
    plt.yticks([1e4, 1e5, 1e6, 1e7])
    plt.ylim(1e4, 1e7)
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.legend(fontsize=10, loc="upper right", framealpha=1)
    plt.title("Multiplication Method Performance Comparison", fontsize=15)
    plt.tight_layout()
    plt.show()

    
    
# plot_test_k()
# plot_test_d()
# plot_test_d_extension()
# plot_test_d_towering()
# plot_ops()
# plot_checks()

plot_ka_variants()


