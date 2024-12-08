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
        
            for l in lines:
                y.append(ast.literal_eval(l.strip()))
    except FileNotFoundError:
        print(f"File '{file_path}' not found.")
    return y

def listAdd(l1, l2):
    return [x + y for x, y in zip(l1, l2)]
 
 
benchmarkPath = "/media/lzl/solid/code/project/benchmark/"

S1_vaPath = "/media/lzl/solid/code/project/benchmark/S1_va_commu.txt"
S2_vaPath = "/media/lzl/solid/code/project/benchmark/S2_va_commu.txt"
S3_vaPath = "/media/lzl/solid/code/project/benchmark/S3_va_commu.txt"
OXT_vaPath = "/media/lzl/solid/code/project/benchmark/OXT_va_commu.txt"

S1_avPath = "/media/lzl/solid/code/project/benchmark/S1_av_commu.txt"
S2_avPath = "/media/lzl/solid/code/project/benchmark/S2_av_commu.txt"
S3_avPath = "/media/lzl/solid/code/project/benchmark/S3_av_commu.txt"
OXT_avPath = "/media/lzl/solid/code/project/benchmark/OXT_av_commu.txt"

S1_volPath = "/media/lzl/solid/code/project/benchmark/S1_vol_commu.txt"
S2_volPath = "/media/lzl/solid/code/project/benchmark/S2_vol_commu.txt"
S3_volPath = "/media/lzl/solid/code/project/benchmark/S3_vol_commu.txt"



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


x1_label = 'selectivity of v'
y1_label = 'Commun. overhead(bytes)'

x2_label = 'volume'
y2_label = 'Commun. overhead(bytes)'

# 创建一个包含四个子图的图形
fig, axs = plt.subplots(2, 2, figsize=(10, 8))
# 在每个子图中绘制折线图和点
for i, ax in enumerate(axs.flat):
    if i == 0: # 画v and a and a_v
        x_values = [107, 201, 310, 406, 508, 621, 711, ]
        draw(ax, x_values, listAdd(S1_va[0], S1_va[1]), listAdd(S1_av[0], S1_av[1]), 'r', x1_label, y1_label, 'S1-v and a', 'S1-a and v')
        draw(ax, x_values, listAdd(S2_va[0], S2_va[1]), listAdd(S2_av[0], S2_av[1]),'g', x1_label, y1_label, 'S2-v and a', 'S2-a and v')
        draw(ax, x_values, listAdd(S3_va[0], S3_va[1]), listAdd(S3_av[0], S3_av[1]), 'b', x1_label, y1_label, 'S3-v and a', 'S3-a and v')
        draw(ax, x_values, listAdd(OXT_va[0], OXT_va[1]), listAdd(OXT_av[0], OXT_av[1]), 'cyan', x1_label, y1_label, 'OXT-v and a', 'OXT-a and v')
        #ax.set_yticks(range(200000000, 400000000, 100000000))
        #ax.set_xticks(range(100, 700, 100))
        #ax.set_yticks(y_sticks[i])
        ax.set_title('Commun. overhead-sterm selectivity')
    if i == 1:
        x_values = [500, 1000, 2000, 3000, 4000, 5000, 6000, ]
        draw(ax, x_values, listAdd(S1_vol[0], S1_vol[1]), [], 'r', x2_label, y2_label, 'S1', '')
        draw(ax, x_values, listAdd(S2_vol[0], S2_vol[1]), [], 'g', x2_label, y2_label, 'S2', '')
        draw(ax, x_values, listAdd(S3_vol[0], S3_vol[1]), [], 'b', x2_label, y2_label, 'S3', '')
        
        #ax.set_xticks(range(100, 700, 100))
        ax.set_title('Commun. overhead-volume')

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