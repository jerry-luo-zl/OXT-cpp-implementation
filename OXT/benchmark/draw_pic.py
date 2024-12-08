import matplotlib.pyplot as plt
import ast

def draw(ax,  x_values, y1, y2, color_, x_label, y_label, y1_legend, y2_legend):
    ax.plot(x_values, y1, label=y1_legend, color=color_)  # 绘制每条折线
    ax.scatter(x_values, y1, marker='x')  # 显示点，形状为叉号
    
    if y2 != []:
        ax.plot(x_values, y2, label=y2_legend, color = color_)  # 绘制每条折线
        ax.scatter(x_values, y2, marker='x')  # 显示点，形状为叉号
    
    ax.set_xlabel(x_label)  # 设置横坐标标签
    ax.set_ylabel(y_label)  # 设置纵坐标标签
    ax.legend()  # 显示图例
    '''
    if x_range:
        ax.xlim(x_range)  # 设置横坐标范围

    if y_range:
        ax.ylim(y_range)  # 设置纵坐标范围
    '''
    
def readFile(path):
    y = []
    try:
        with open(path, 'r') as file:
            lines = file.readlines()
            x_values = ast.literal_eval(lines[0].strip())
        
            for l in lines:
                y.append(ast.literal_eval(l.strip()))
    except FileNotFoundError:
        print(f"File '{file_path}' not found.")
    return y

S1_vaPath = "/media/lzl/solid/code/project/benchmark/S1_rec40000_mvol1000v_a.txt"
S2_vaPath = "/media/lzl/solid/code/project/benchmark/S2_rec40000_mvol1000v_a.txt"
S3_vaPath = "/media/lzl/solid/code/project/benchmark/S3_rec40000_mvol1000v_a.txt"
OXT_vaPath = "/media/lzl/solid/code/project/benchmark/OXT_rec40000_mvol1000v_a.txt"

S1_avPath = "/media/lzl/solid/code/project/benchmark/S1_rec40000_mvol1000a_v.txt"
S2_avPath = "/media/lzl/solid/code/project/benchmark/S2_rec40000_mvol1000a_v.txt"
S3_avPath = "/media/lzl/solid/code/project/benchmark/S3_rec40000_mvol1000a_v.txt"
OXT_avPath = "/media/lzl/solid/code/project/benchmark/OXT_rec40000_mvol1000a_v.txt"

S1_volPath = "/media/lzl/solid/code/project/benchmark/S1_rec40000_mvol_var.txt"
S2_volPath = "/media/lzl/solid/code/project/benchmark/S2_rec40000_mvol_var.txt"
S3_volPath = "/media/lzl/solid/code/project/benchmark/S3_rec40000_mvol_var.txt"

benchmarkPath = "/media/lzl/solid/code/project/benchmark/"
S1_va_serverTimePath = benchmarkPath + "S1_va_serverTime.txt"
S2_va_serverTimePath = benchmarkPath + "S2_va_serverTime.txt"
S3_va_serverTimePath = benchmarkPath + "S3_va_serverTime.txt"
OXT_va_serverTimePath = benchmarkPath + "OXT_va_serverTime.txt"

S1_av_serverTimePath = benchmarkPath + "S1_av_serverTime.txt"
S2_av_serverTimePath = benchmarkPath + "S2_av_serverTime.txt"
S3_av_serverTimePath = benchmarkPath + "S3_av_serverTime.txt"
OXT_av_serverTimePath = benchmarkPath + "OXT_av_serverTime.txt"

S1_vol_serverTimePath = benchmarkPath + "S1_vol_serverTime.txt"
S2_vol_serverTimePath = benchmarkPath + "S2_vol_serverTime.txt"
S3_vol_serverTimePath = benchmarkPath + "S3_vol_serverTime.txt"


S1_va = readFile(S1_vaPath)
S2_va = readFile(S2_vaPath)
S3_va = readFile(S3_vaPath)
OXT_va = readFile(OXT_vaPath)

S1_av = readFile(S1_avPath)
S2_av = readFile(S2_avPath)
S3_av = readFile(S3_avPath)
OXT_av = readFile(OXT_avPath)

S1_vol = readFile(S1_volPath)
S2_vol = readFile(S2_volPath)
S3_vol = readFile(S3_volPath)

S1_va_serverTime = readFile(S1_va_serverTimePath)
S2_va_serverTime = readFile(S2_va_serverTimePath)
S3_va_serverTime = readFile(S3_va_serverTimePath)
OXT_va_serverTime = readFile(OXT_va_serverTimePath)

S1_av_serverTime = readFile(S1_av_serverTimePath)
S2_av_serverTime = readFile(S2_av_serverTimePath)
S3_av_serverTime = readFile(S3_av_serverTimePath)
OXT_av_serverTime = readFile(OXT_av_serverTimePath)


S1_vol_serverTime = readFile(S1_vol_serverTimePath)
S2_vol_serverTime = readFile(S2_vol_serverTimePath)
S3_vol_serverTime = readFile(S3_vol_serverTimePath)

x1_label = 'selectivity of v'
y1_label = 'time(ns)'

x2_label = 'volume'
y2_label = 'time(ns)'

# 创建一个包含四个子图的图形
fig, axs = plt.subplots(2, 2, figsize=(10, 8))
# 在每个子图中绘制折线图和点
for i, ax in enumerate(axs.flat):
    if i == 0: # 画v and a and a_v
        x_values = S1_va[0]
        draw(ax, x_values, S1_va[3], S1_av[3], 'r', x1_label, y1_label, 'S1-v and a', 'S1-a and v')
        draw(ax, x_values, S2_va[1], S2_av[1],'g', x1_label, y1_label, 'S2-v and a', 'S2-a and v')
        draw(ax, x_values, S3_va[1], S3_av[1], 'b', x1_label, y1_label, 'S3-v and a', 'S3-a and v')
        draw(ax, x_values, OXT_va[1], OXT_av[1], 'cyan', x1_label, y1_label, 'OXT-v and a', 'OXT-a and v')
        #ax.set_yticks(range(200000000, 400000000, 100000000))
        #ax.set_xticks(range(100, 700, 100))
        #ax.set_yticks(y_sticks[i])
        ax.set_title('Client lantency-sterm selectivity')
    if i == 1:
        x_values = S1_vol[0]
        draw(ax, x_values, S1_vol[1], [], 'r', x2_label, y2_label, 'S1', '')
        draw(ax, x_values, S2_vol[1], [], 'g', x2_label, y2_label, 'S2', '')
        draw(ax, x_values, S3_vol[1], [], 'b', x2_label, y2_label, 'S3', '')
        
        #ax.set_xticks(range(100, 700, 100))
        ax.set_title('Client lantency-volume')
    if i == 2:
        x_values = S1_va[0]
        draw(ax, x_values, S1_va_serverTime[0], S1_av_serverTime[0], 'r', x1_label, y1_label, 'S1-v and a', 'S1-a and v')
        draw(ax, x_values, S2_va_serverTime[0], S2_av_serverTime[0], 'g', x1_label, y1_label, 'S2-v and a', 'S2-a and v')
        draw(ax, x_values, S3_va_serverTime[0], S3_av_serverTime[0], 'b', x1_label, y1_label, 'S3-v and a', 'S3-a and v')
        draw(ax, x_values, OXT_va_serverTime[0], OXT_av_serverTime[0], 'cyan', x1_label, y1_label, 'OXT-v and a', 'OXT-a and v')
        #ax.set_xticks(range(100, 700, 100))
        ax.set_title('Server time-sterm selectivity')
    if i == 3:
        x_values = S1_vol[0]
        draw(ax, x_values, S1_vol_serverTime[0], [], 'r', x2_label, y2_label, 'S1', '')
        draw(ax, x_values, S2_vol_serverTime[0], [], 'g', x2_label, y2_label, 'S2', '')
        draw(ax, x_values, S3_vol_serverTime[0], [], 'b', x2_label, y2_label, 'S3', '')
        #ax.set_xticks(range(100, 700, 100))
        ax.set_title('Server time-volume')

'''
y_sticks = [
    range(0, 25000000, 5000000),
    range(5000000, 20000000, 5000000),
    range(10000000, 25000000, 5000000),
    range(10000000, 25000000, 5000000),
]
'''
    
    
plt.tight_layout()  # 调整布局以防止重叠

#plt.grid(True)  # 显示网格
plt.show()  # 显示图表