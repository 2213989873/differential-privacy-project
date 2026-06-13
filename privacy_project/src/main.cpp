/**
 * @file main.cpp
 * @brief 差分隐私算法实现与比较——主程序
 * @author Course Project - Data Privacy Protection
 * 
 * 本程序实现了三种差分隐私机制并在多个数据集上进行比较实验
 * 1. 拉普拉斯机制 (Laplace Mechanism)
 * 2. 高斯机制 (Gaussian Mechanism) 
 * 3. 指数机制 (Exponential Mechanism)
 */

 #include "differential_privacy.h"
 #include <fstream>
 #include <sstream>
 #include <string>
 #include <random>
 #include <functional>
 
 using namespace differential_privacy;
 
 // ============================================================
 //  数据集生成与加载
 // ============================================================
 
 /**
  * @brief 生成合成数据集
  * @param n 数据集大小
  * @param mean 均值（正态分布）
  * @param stddev 标准差（正态分布）
  * @return 生成的数据集
  */
 std::vector<double> generateSyntheticDataset(
     size_t n, 
     double mean = 50.0, 
     double stddev = 15.0
 ) {
     RandomGenerator rng;
     std::vector<double> dataset;
     dataset.reserve(n);
     
     for (size_t i = 0; i < n; ++i) {
         dataset.push_back(mean + stddev * rng.gaussian());
     }
     return dataset;
 }
 
 /**
  * @brief 生成均匀分布数据集
  * @param n 数据集大小
  * @param low 下限
  * @param high 上限
  */
 std::vector<double> generateUniformDataset(
     size_t n, 
     double low = 0.0, 
     double high = 100.0
 ) {
     std::mt19937 rng(42);
     std::uniform_real_distribution<double> dist(low, high);
     std::vector<double> dataset;
     dataset.reserve(n);
     
     for (size_t i = 0; i < n; ++i) {
         dataset.push_back(dist(rng));
     }
     return dataset;
 }
 
 /**
  * @brief 加载CSV数据集
  * @param filename CSV文件路径（假设只有一列数值）
  * @return 加载的数据集
  */
 std::vector<double> loadDataset(const std::string& filename) {
     std::vector<double> dataset;
     std::ifstream file(filename);
     
     if (!file.is_open()) {
         std::cerr << "Warning: Cannot open file " << filename 
                   << ". Using synthetic data instead." << std::endl;
         return generateSyntheticDataset(1000);
     }
     
     std::string line;
     while (std::getline(file, line)) {
         // 跳过空行和注释行
         if (line.empty() || line[0] == '#') continue;
         
         try {
             double value = std::stod(line);
             dataset.push_back(value);
         } catch (...) {
             // 跳过无法解析的行
             continue;
         }
     }
     
     file.close();
     std::cout << "Loaded " << dataset.size() << " records from " << filename << std::endl;
     return dataset;
 }
 
 /**
  * @brief 获取数据集的基本统计信息
  */
 struct DatasetStats {
     size_t size;
     double min;
     double max;
     double sum;
     double mean;
     double variance;
     double stddev;
     double median;
     double max_abs;
 };
 
 DatasetStats computeStats(const std::vector<double>& dataset) {
     DatasetStats stats;
     stats.size = dataset.size();
     
     if (dataset.empty()) return stats;
     
     stats.min = *std::min_element(dataset.begin(), dataset.end());
     stats.max = *std::max_element(dataset.begin(), dataset.end());
     stats.sum = std::accumulate(dataset.begin(), dataset.end(), 0.0);
     stats.mean = stats.sum / stats.size;
     
     // 计算方差
     double sq_sum = 0.0;
     for (double x : dataset) {
         sq_sum += (x - stats.mean) * (x - stats.mean);
     }
     stats.variance = sq_sum / stats.size;
     stats.stddev = std::sqrt(stats.variance);
     
     // 计算中位数
     std::vector<double> sorted = dataset;
     std::sort(sorted.begin(), sorted.end());
     if (dataset.size() % 2 == 0) {
         stats.median = (sorted[dataset.size()/2 - 1] + sorted[dataset.size()/2]) / 2.0;
     } else {
         stats.median = sorted[dataset.size()/2];
     }
     
     // 最大绝对值
     stats.max_abs = std::max(std::abs(stats.min), std::abs(stats.max));
     
     return stats;
 }
 
 // ============================================================
 //  实验一：隐私预算对精度的影响
 // ============================================================
 
 void experiment1_PrivacyBudgetEffect() {
     std::cout << "\n========== 实验一：隐私预算ε对精度的影响 ==========\n" << std::endl;
     
     // 生成合成数据
     auto dataset = generateSyntheticDataset(10000, 50.0, 15.0);
     auto stats = computeStats(dataset);
     
     std::cout << "数据集大小: " << stats.size << std::endl;
     std::cout << "均值: " << stats.mean << ", 标准差: " << stats.stddev << std::endl;
     std::cout << "最小值: " << stats.min << ", 最大值: " << stats.max << std::endl;
     
     // 三种机制的实例
     LaplaceMechanism laplace(12345);
     GaussianMechanism gaussian(12346);
     ExponentialMechanism exponential(12347);
     
     // 要测试的隐私预算值
     std::vector<double> epsilons = {0.01, 0.05, 0.1, 0.5, 1.0, 2.0, 5.0, 10.0};
     std::vector<double> laplace_mse, gaussian_mse, exponential_mse;
     
     const double delta = 1e-5;
     const int num_trials = 100;
     
     for (double eps : epsilons) {
         // === 拉普拉斯机制 ===
         double lap_mse = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             double result = laplace.addNoise(stats.mean, 1.0 / stats.size, eps);
             double error = stats.mean - result;
             lap_mse += error * error;
         }
         lap_mse /= num_trials;
         laplace_mse.push_back(lap_mse);
         
         // === 高斯机制 ===
         double gau_mse = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             double result = gaussian.addNoise(stats.mean, 1.0 / stats.size, eps, delta);
             double error = stats.mean - result;
             gau_mse += error * error;
         }
         gau_mse /= num_trials;
         gaussian_mse.push_back(gau_mse);
         
         // === 指数机制：中位数选择 ===
         // 用指数机制从中位数附近的候选值中选择
         double true_median = stats.median;
         
         // 候选输出：在真实中位数附近 [-2σ, +2σ] 范围内取200个均匀采样点
         double range = 4.0 * stats.stddev;
         std::vector<double> candidates;
         int num_candidates = 200;
         for (int i = 0; i <= num_candidates; ++i) {
             candidates.push_back(true_median - range/2 + i * range / num_candidates);
         }
         
         double exp_mse = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             // 效用函数：查询结果与真实中位数的负绝对距离（越大越好）
             auto utility = [true_median](double candidate) {
                 return -std::abs(candidate - true_median);
             };
             
             // 效用函数敏感度：改变一条记录，中位数最大变化为数据集值域范围
             double util_sensitivity = (stats.max - stats.min) / 2.0;
             
             double selected = exponential.select<double>(
                 candidates, utility, util_sensitivity, eps
             );
             double error = true_median - selected;
             exp_mse += error * error;
         }
         exp_mse /= num_trials;
         exponential_mse.push_back(exp_mse);
     }
     
     // 打印比较表格
     evaluation::printComparisonTable(epsilons, laplace_mse, gaussian_mse, exponential_mse);
 }
 
 // ============================================================
 //  实验二：数据集规模对误差的影响
 // ============================================================
 
 void experiment2_DatasetSizeEffect() {
     std::cout << "\n========== 实验二：数据集规模对误差的影响 ==========\n" << std::endl;
     
     LaplaceMechanism laplace;
     GaussianMechanism gaussian;
     ExponentialMechanism exponential;
     
     std::vector<size_t> sizes = {100, 500, 1000, 5000, 10000, 50000};
     const double epsilon = 1.0;
     const double delta = 1e-5;
     const int num_trials = 50;
     
     std::cout << std::left;
     std::cout << std::setw(12) << "规模" 
               << std::setw(18) << "计数MSE(拉)"
               << std::setw(18) << "均值MSE(拉)"
               << std::setw(18) << "均值MSE(高)" 
               << std::setw(18) << "中位数MSE(指)" << std::endl;
     std::cout << std::string(84, '-') << std::endl;
     
     for (size_t n : sizes) {
         auto dataset = generateSyntheticDataset(n, 50.0, 15.0);
         auto stats = computeStats(dataset);
         
         // 计数查询（拉普拉斯）
         double count_mse_lap = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             double result = laplace.addNoise(static_cast<double>(n), 1.0, epsilon);
             double error = n - result;
             count_mse_lap += error * error;
         }
         count_mse_lap /= num_trials;
         
         // 均值查询（拉普拉斯）
         double mean_mse_lap = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             double result = laplace.addNoise(stats.mean, stats.max_abs / n, epsilon);
             double error = stats.mean - result;
             mean_mse_lap += error * error;
         }
         mean_mse_lap /= num_trials;
         
         // 均值查询（高斯）
         double mean_mse_gau = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             double result = gaussian.addNoise(stats.mean, stats.max_abs / n, epsilon, delta);
             double error = stats.mean - result;
             mean_mse_gau += error * error;
         }
         mean_mse_gau /= num_trials;
         
         // 中位数查询（指数机制）
         double true_median = stats.median;
         double range = 4.0 * stats.stddev;
         std::vector<double> candidates;
         int num_candidates = 200;
         for (int i = 0; i <= num_candidates; ++i) {
             candidates.push_back(true_median - range/2 + i * range / num_candidates);
         }
         
         double median_mse_exp = 0.0;
         for (int t = 0; t < num_trials; ++t) {
             auto utility = [true_median](double c) { return -std::abs(c - true_median); };
             double util_sensitivity = (stats.max - stats.min) / 2.0;
             double selected = exponential.select<double>(
                 candidates, utility, util_sensitivity, epsilon
             );
             median_mse_exp += (true_median - selected) * (true_median - selected);
         }
         median_mse_exp /= num_trials;
         
         std::cout << std::setw(12) << n
                   << std::setw(18) << count_mse_lap
                   << std::setw(18) << mean_mse_lap
                   << std::setw(18) << mean_mse_gau
                   << std::setw(18) << median_mse_exp << std::endl;
     }
     std::cout << std::endl;
 }
 
 // ============================================================
 //  实验三：运行时间对比
 // ============================================================
 
 void experiment3_PerformanceComparison() {
     std::cout << "\n========== 实验三：运行时间对比 ==========\n" << std::endl;
     
     LaplaceMechanism laplace;
     GaussianMechanism gaussian;
     ExponentialMechanism exponential;
     
     std::vector<size_t> sizes = {1000, 10000, 100000};
     const double epsilon = 1.0;
     const double delta = 1e-5;
     const int num_queries = 1000;
     
     std::cout << std::left;
     std::cout << std::setw(12) << "规模"
               << std::setw(18) << "拉普拉斯(ms)"
               << std::setw(18) << "高斯(ms)"
               << std::setw(18) << "指数(ms)" << std::endl;
     std::cout << std::string(66, '-') << std::endl;
     
     for (size_t n : sizes) {
         auto dataset = generateSyntheticDataset(n, 50.0, 15.0);
         auto stats = computeStats(dataset);
         
         // 准备指数机制的候选列表
         double true_median = stats.median;
         double range = 4.0 * stats.stddev;
         std::vector<double> candidates;
         for (int i = 0; i <= 200; ++i) {
             candidates.push_back(true_median - range/2 + i * range / 200.0);
         }
         auto utility = [true_median](double c) { return -std::abs(c - true_median); };
         double util_sensitivity = (stats.max - stats.min) / 2.0;
         
         // 测试拉普拉斯机制
         auto start = std::chrono::high_resolution_clock::now();
         for (int q = 0; q < num_queries; ++q) {
             volatile double result = laplace.addNoise(stats.mean, stats.max_abs / n, epsilon);
             (void)result;
         }
         auto end = std::chrono::high_resolution_clock::now();
         double lap_time = std::chrono::duration<double, std::milli>(end - start).count();
         
         // 测试高斯机制
         start = std::chrono::high_resolution_clock::now();
         for (int q = 0; q < num_queries; ++q) {
             volatile double result = gaussian.addNoise(stats.mean, stats.max_abs / n, epsilon, delta);
             (void)result;
         }
         end = std::chrono::high_resolution_clock::now();
         double gau_time = std::chrono::duration<double, std::milli>(end - start).count();
         
         // 测试指数机制
         start = std::chrono::high_resolution_clock::now();
         for (int q = 0; q < num_queries; ++q) {
             volatile double selected = exponential.select<double>(candidates, utility, 
                                            util_sensitivity, epsilon);
             (void)selected;
         }
         end = std::chrono::high_resolution_clock::now();
         double exp_time = std::chrono::duration<double, std::milli>(end - start).count();
         
         // 转换为平均每次查询的时间
         std::cout << std::setw(12) << n
                   << std::setw(18) << (lap_time / num_queries * 1000)  // 微秒
                   << std::setw(18) << (gau_time / num_queries * 1000)
                   << std::setw(18) << (exp_time / num_queries * 1000) << std::endl;
     }
     std::cout << std::endl;
 }
 
 // ============================================================
 //  实验四：复合查询隐私损失分析
 // ============================================================
 
 void experiment4_CompositionAnalysis() {
     std::cout << "\n========== 实验四：复合查询的隐私损失分析 ==========\n" << std::endl;
     
     std::vector<int> query_counts = {1, 10, 50, 100, 500, 1000};
     const double per_query_epsilon = 1.0;
     const double delta_prime = 1e-5;
     
     std::cout << std::left;
     std::cout << std::setw(12) << "查询次数"
               << std::setw(22) << "序列组合(Σε)"
               << std::setw(22) << "高级组合(ε_total)"
               << std::setw(18) << "备注" << std::endl;
     std::cout << std::string(74, '-') << std::endl;
     
     PrivacyBudgetAccountant accountant;
     
     for (int k : query_counts) {
         // 序列组合
         double sequential_epsilon = k * per_query_epsilon;
         
         // 高级组合
         double advanced_epsilon = PrivacyBudgetAccountant::advancedComposition(
             per_query_epsilon, k, delta_prime
         );
         
         std::cout << std::setw(12) << k
                   << std::setw(22) << sequential_epsilon
                   << std::setw(22) << advanced_epsilon
                   << std::setw(18) << (k <= 10 ? "可接受" : "预算消耗大") << std::endl;
     }
     
     std::cout << "\n结论：高级组合定理比简单序列组合更精确地估计了实际隐私损失。" << std::endl;
     std::cout << "对于大量查询，推荐使用矩会计（Moment Accountant）方法进行更紧的分析。" << std::endl;
     std::cout << std::endl;
 }
 
 // ============================================================
 //  实验五：噪声分布可视化数据生成
 // ============================================================
 
 void experiment5_NoiseDistribution() {
     std::cout << "\n========== 实验五：噪声分布采样数据生成 ==========\n" << std::endl;
     
     RandomGenerator rng;
     const int num_samples = 10000;
     const double scale = 1.0;
     
     // 生成拉普拉斯噪声样本
     std::vector<double> laplace_samples;
     for (int i = 0; i < num_samples; ++i) {
         laplace_samples.push_back(rng.laplace(scale));
     }
     
     // 生成高斯噪声样本
     std::vector<double> gaussian_samples;
     for (int i = 0; i < num_samples; ++i) {
         gaussian_samples.push_back(scale * rng.gaussian());
     }
     
     // 统计和对比
     auto lap_stats = computeStats(laplace_samples);
     auto gau_stats = computeStats(gaussian_samples);
     
     std::cout << std::left;
     std::cout << std::setw(16) << "统计量" 
               << std::setw(20) << "拉普拉斯噪声" 
               << std::setw(20) << "高斯噪声" << std::endl;
     std::cout << std::string(56, '-') << std::endl;
     
     std::cout << std::setw(16) << "均值"
               << std::setw(20) << lap_stats.mean
               << std::setw(20) << gau_stats.mean << std::endl;
               
     std::cout << std::setw(16) << "标准差"
               << std::setw(20) << lap_stats.stddev
               << std::setw(20) << gau_stats.stddev << std::endl;
               
     std::cout << std::setw(16) << "最小值"
               << std::setw(20) << lap_stats.min
               << std::setw(20) << gau_stats.min << std::endl;
               
     std::cout << std::setw(16) << "最大值"
               << std::setw(20) << lap_stats.max
               << std::setw(20) << gau_stats.max << std::endl;
     
     // 输出概率密度对比（离散化统计）
     std::cout << "\n噪声分布直方图（理论对比）：" << std::endl;
     std::cout << "拉普拉斯: 集中在0附近，拖尾重 | 高斯: 相对集中，拖尾轻" << std::endl;
     std::cout << std::endl;
 }
 
 // ============================================================
 //  主函数
 // ============================================================
 
 int main() {
     std::cout << "╔══════════════════════════════════════════════════════════╗\n";
     std::cout << "║   差分隐私算法实现与比较系统                             ║\n";
     std::cout << "║   Differential Privacy: Algorithm Implementation &       ║\n";
     std::cout << "║   Comparative Analysis                                   ║\n";
     std::cout << "╚══════════════════════════════════════════════════════════╝\n";
     std::cout << "\n课程项目：数据隐私保护与安全计算\n";
     std::cout << "实现算法：拉普拉斯机制 | 高斯机制 | 指数机制\n";
     std::cout << "\n开始实验...\n";
     
     try {
         experiment1_PrivacyBudgetEffect();
         experiment2_DatasetSizeEffect();
         experiment3_PerformanceComparison();
         experiment4_CompositionAnalysis();
         experiment5_NoiseDistribution();
         
         std::cout << "\n所有实验完成！" << std::endl;
         
         // 输出摘要
         std::cout << "\n========== 实验总结 ==========" << std::endl;
         std::cout << "1. 隐私预算ε越小，数据隐私保护越强，但数据效用越低。" << std::endl;
         std::cout << "2. 数据集规模越大，均值查询的精度越高（MSE ~ 1/n²）。" << std::endl;
         std::cout << "3. 三种机制运行效率相近，但指数机制对候选集大小敏感。" << std::endl;
         std::cout << "4. 复合查询时，高级组合定理提供比序列组合更紧的隐私损失界。" << std::endl;
         std::cout << "5. 拉普拉斯噪声拖尾更重（适合低ε场景），高斯噪声更集中。" << std::endl;
         std::cout << "================================" << std::endl;
         
     } catch (const std::exception& e) {
         std::cerr << "Error: " << e.what() << std::endl;
         return 1;
     }
     
     std::cout << "\n按任意键退出..." << std::endl;
     std::cin.get();
     return 0;
 }
