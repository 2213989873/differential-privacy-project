# 差分隐私算法实现与比较分析

## 课程项目：数据隐私保护与安全计算

---

## 项目简介

本项目实现了差分隐私（Differential Privacy, DP）的三种核心机制，并在多个数据集上进行了性能比较实验：

1. **拉普拉斯机制 (Laplace Mechanism)** — 满足 ε-差分隐私
2. **高斯机制 (Gaussian Mechanism)** — 满足 (ε,δ)-差分隐私
3. **指数机制 (Exponential Mechanism)** — 适用于非数值型输出

---

## 项目结构

```
privacy_project/
├── src/                          # 源代码目录
│   ├── differential_privacy.h    # 差分隐私核心算法头文件
│   └── main.cpp                  # 主程序（包含5个实验）
├── dataset/                      # 数据集目录
│   ├── synthetic_normal_10000.txt # 合成数据集(正态分布)
│   └── adult_sample.txt          # UCI Adult数据集样本
├── report/                       # 项目报告
│   └── report.md                 # 完整的调查报告
└── readme/
    └── README.md                 # 本文件
```

---

## 编译与运行

### 环境要求

- **操作系统**: Windows 10+ / Linux / macOS
- **编译器**: 支持 C++17 标准的编译器
  - Windows: MSVC (Visual Studio 2019+) 或 MinGW-w64 (g++ 8+)
  - Linux: g++ 7+ 或 clang++ 6+
  - macOS: Apple Clang 12+

### 编译方法

#### 方法一：使用 g++ 编译

```bash
# 进入源码目录
cd privacy_project/src

# 编译
g++ -std=c++17 -O2 main.cpp -o dp_experiment.exe

# 运行
./dp_experiment.exe
```

#### 方法二：使用 MSVC (Visual Studio)

```bash
# 打开 Developer Command Prompt
cl /EHsc /std:c++17 /O2 main.cpp /Fe:dp_experiment.exe
dp_experiment.exe
```

#### 方法三：使用 CMake (可选)

```bash
# 在项目根目录创建 build 文件夹
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
make
./dp_experiment.exe
```

---

## 实验内容

程序运行后自动执行以下5个实验：

### 实验1：隐私预算ε对精度的影响
- 测试 ε = {0.01, 0.05, 0.1, 0.5, 1.0, 2.0, 5.0, 10.0} 下的MSE
- 比较三种机制在相同ε下的性能

### 实验2：数据集规模对误差的影响
- 测试 n = {100, 500, 1000, 5000, 10000, 50000} 下的误差变化
- 分析计数、均值、中位数查询的差异

### 实验3：运行时间对比
- 对比三种机制在不同数据规模下的执行效率
- 测试每个查询的平均耗时（微秒级）

### 实验4：复合查询隐私损失分析
- 比较序列组合定理与高级组合定理
- 分析多次查询累积的隐私损失

### 实验5：噪声分布分析
- 生成拉普拉斯噪声和高斯噪声样本
- 统计分析两者的分布特征差异

---

## 输出示例

程序输出格式示例：

```
========== 实验一：隐私预算ε对精度的影响 ==========

数据集大小: 10000
均值: 50.123, 标准差: 14.987

========== 差分隐私算法比较 ==========
ε               拉普拉斯MSE      高斯MSE         指数MSE        
----------------------------------------------------------------
0.01            99.8521         52.3416         245.7834
0.1             1.0234          0.5342          2.8934
1.0             0.0102          0.0054          0.0423
...
```

---

## 核心API说明

### 拉普拉斯机制 (LaplaceMechanism)
```cpp
LaplaceMechanism laplace;
double result = laplace.addNoise(queryResult, sensitivity, epsilon);
```

### 高斯机制 (GaussianMechanism)
```cpp
GaussianMechanism gaussian;
double result = gaussian.addNoise(queryResult, sensitivity, epsilon, delta);
```

### 指数机制 (ExponentialMechanism)
```cpp
ExponentialMechanism exponential;
// 定义效用函数
auto utility = [](double candidate) { return -abs(candidate - trueValue); };
double selected = exponential.select(candidates, utility, sensitivity, epsilon);
```

---

## 自定义数据集

如需使用自己的数据集，请按以下格式创建文本文件：

```
# 注释行以 # 开头
value1
value2
value3
...
```

然后在 `main.cpp` 中修改加载代码：

```cpp
auto myData = loadDataset("../dataset/your_dataset.txt");
```

---

## 参考文献

1. Dwork, C., et al. (2006). Calibrating noise to sensitivity in private data analysis. TCC.
2. Dwork, C., & Roth, A. (2014). The algorithmic foundations of differential privacy. Foundations and Trends in Theoretical Computer Science.
3. McSherry, F., & Talwar, K. (2007). Mechanism design via differential privacy. FOCS.
4. Abadi, M., et al. (2016). Deep learning with differential privacy. CCS.

---

## 团队成员

| 成员 | 职责 |
|------|------|
| [姓名] | 文献调研、拉普拉斯机制实现、报告撰写 |
| [姓名] | 高斯机制实现、实验设计、数据分析 |
| [姓名] | 指数机制实现、数据集准备、README |

*（单人项目可删除此表）*
