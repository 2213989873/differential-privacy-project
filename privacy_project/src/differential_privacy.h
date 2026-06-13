/**
 * @file differential_privacy.h
 * @brief 差分隐私核心算法的C++实现——头文件
 * @author Course Project - Data Privacy Protection
 * 
 * 本文件定义了三种经典差分隐私机制：
 * 1. 拉普拉斯机制 (Laplace Mechanism)       - ε-差分隐私
 * 2. 高斯机制 (Gaussian Mechanism)           - (ε,δ)-差分隐私
 * 3. 指数机制 (Exponential Mechanism)        - ε-差分隐私（非数值输出）
 */

 #ifndef DIFFERENTIAL_PRIVACY_H
 #define DIFFERENTIAL_PRIVACY_H
 
 #define _USE_MATH_DEFINES
 #include <vector>
 #include <functional>
 #include <random>
 #include <chrono>
 #include <cmath>
 #include <algorithm>
 #include <iostream>
 #include <iomanip>
 #include <numeric>
 #include <string>
 #include <map>
 
 /**
  * @brief 差分隐私命名空间
  * 
  * 包含所有差分隐私相关的工具函数和机制实现
  */
 namespace differential_privacy {
 
     // ============================================================
     //  随机数生成器
     // ============================================================
     
     /**
      * @brief 全局随机数生成器类
      * 
      * 使用Mersenne Twister引擎保证高质量的随机数输出
      */
     class RandomGenerator {
     private:
         std::mt19937_64 engine;
         
     public:
         RandomGenerator() {
             // 使用高精度时钟种子
             auto seed = std::chrono::high_resolution_clock::now()
                         .time_since_epoch().count();
             engine.seed(static_cast<unsigned int>(seed));
         }
         
         explicit RandomGenerator(unsigned int seed) : engine(seed) {}
         
         /**
          * @brief 生成[0,1)之间的均匀分布随机数
          */
         double uniform() {
             std::uniform_real_distribution<double> dist(0.0, 1.0);
             return dist(engine);
         }
         
         /**
          * @brief 生成标准正态分布 N(0,1) 随机数
          * 使用Box-Muller变换
          */
         double gaussian() {
             double u1 = uniform();
             double u2 = uniform();
             return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
         }
         
         /**
          * @brief 生成拉普拉斯分布 Lap(b) 随机数
          * @param b 尺度参数（scale parameter）
          * 使用逆变换法：F^(-1)(p) = -b * sign(p-0.5) * ln(1-2|p-0.5|)
          */
         double laplace(double b) {
             double u = uniform() - 0.5;
             return -b * std::signbit(u) * std::log(1.0 - 2.0 * std::abs(u));
         }
     };
     
     // ============================================================
     //  敏感度计算
     // ============================================================
     
     /**
      * @brief 计算查询函数的全局敏感度
      * 
      * 对于不同类型的查询，敏感度计算方式不同：
      * - 计数查询：Δf = 1
      * - 求和查询：Δf = max|xi|
      * - 均值查询：Δf = max|xi| / n
      * - 分位数查询：Δf = max(x)
      */
     namespace sensitivity {
         
         /**
          * @brief 计数查询的敏感度恒为1
          */
         inline double count() { return 1.0; }
         
         /**
          * @brief 求和查询的敏感度
          * @param max_abs_value 数据中绝对值的最大值
          */
         inline double sum(double max_abs_value) { return max_abs_value; }
         
         /**
          * @brief 均值查询的敏感度
          * @param max_abs_value 数据中绝对值的最大值
          * @param n 数据集大小
          */
         inline double mean(double max_abs_value, size_t n) {
             return max_abs_value / n;
         }
         
         /**
          * @brief 通用敏感度（用户自定义）
          */
         inline double custom(double sensitivity_value) {
             return sensitivity_value;
         }
     }
     
     // ============================================================
     //  机制一：拉普拉斯机制（Laplace Mechanism）
     // ============================================================
     
     /**
      * @brief 拉普拉斯机制
      * 
      * 满足 ε-差分隐私 (ε-DP)
      * 机制：M(D) = f(D) + Lap(Δf/ε)
      * 
      * 适用场景：数值型查询，如计数、求和、均值
      */
     class LaplaceMechanism {
     private:
         RandomGenerator rng;
         
     public:
         LaplaceMechanism() = default;
         explicit LaplaceMechanism(unsigned int seed) : rng(seed) {}
         
         /**
          * @brief 对查询结果添加拉普拉斯噪声
          * @param query_result 真实查询结果 f(D)
          * @param sensitivity 查询函数的全局敏感度 Δf
          * @param epsilon 隐私预算 ε
          * @return 加了噪声后的查询结果
          */
         double addNoise(double query_result, double sensitivity, double epsilon) {
             if (epsilon <= 0.0) {
                 throw std::invalid_argument("Epsilon must be > 0");
             }
             double scale = sensitivity / epsilon;
             return query_result + rng.laplace(scale);
         }
         
         /**
          * @brief 批量添加噪声（适用于向量形式的查询结果）
          * @param query_results 真实查询结果向量
          * @param sensitivity 全局敏感度
          * @param epsilon 隐私预算 ε
          * @return 加了噪声后的查询结果向量
          * 
          * 注意：批量查询时隐私预算会按查询次数分配
          */
         std::vector<double> addNoiseBatch(
             const std::vector<double>& query_results,
             double sensitivity, 
             double epsilon
         ) {
             std::vector<double> noisy_results;
             noisy_results.reserve(query_results.size());
             
             // 按序列组合定理分配隐私预算
             double per_query_epsilon = epsilon / query_results.size();
             
             for (double val : query_results) {
                 noisy_results.push_back(addNoise(val, sensitivity, per_query_epsilon));
             }
             return noisy_results;
         }
         
         /**
          * @brief 拉普拉斯机制的概率密度函数
          * @param x 变量值
          * @param b 尺度参数
          * @param mu 位置参数（默认为0）
          */
         static double pdf(double x, double b, double mu = 0.0) {
             return 0.5 / b * std::exp(-std::abs(x - mu) / b);
         }
         
         /**
          * @brief 拉普拉斯机制的累积分布函数
          */
         static double cdf(double x, double b, double mu = 0.0) {
             if (x < mu) return 0.5 * std::exp((x - mu) / b);
             else return 1.0 - 0.5 * std::exp(-(x - mu) / b);
         }
     };
     
     // ============================================================
     //  机制二：高斯机制（Gaussian Mechanism）
     // ============================================================
     
     /**
      * @brief 高斯机制
      * 
      * 满足 (ε,δ)-差分隐私 [(ε,δ)-DP]
      * 机制：M(D) = f(D) + N(0, σ²)
      * 其中 σ² = 2·Δf²·ln(1.25/δ) / ε²
      * 
      * 适用场景：数值型查询，特别是需要多次复合查询的场景
      */
     class GaussianMechanism {
     private:
         RandomGenerator rng;
         
     public:
         GaussianMechanism() = default;
         explicit GaussianMechanism(unsigned int seed) : rng(seed) {}
         
         /**
          * @brief 计算满足 (ε,δ)-DP 所需的高斯噪声标准差
          * @param sensitivity 查询函数的全局敏感度 Δf
          * @param epsilon 隐私预算 ε
          * @param delta 松弛参数 δ（通常取 < 1/n²）
          * @return 高斯噪声的标准差 σ
          */
         static double computeSigma(double sensitivity, double epsilon, double delta) {
             if (epsilon <= 0.0 || delta <= 0.0 || delta >= 1.0) {
                 throw std::invalid_argument(
                     "Epsilon > 0 and 0 < delta < 1 required"
                 );
             }
             return sensitivity * std::sqrt(2.0 * std::log(1.25 / delta)) / epsilon;
         }
         
         /**
          * @brief 对查询结果添加高斯噪声
          * @param query_result 真实查询结果 f(D)
          * @param sensitivity 全局敏感度 Δf
          * @param epsilon 隐私预算 ε
          * @param delta 松弛参数 δ
          * @return 加了噪声后的查询结果
          */
         double addNoise(
             double query_result, 
             double sensitivity, 
             double epsilon, 
             double delta
         ) {
             double sigma = computeSigma(sensitivity, epsilon, delta);
             return query_result + sigma * rng.gaussian();
         }
         
         /**
          * @brief 批量添加高斯噪声
          */
         std::vector<double> addNoiseBatch(
             const std::vector<double>& query_results,
             double sensitivity,
             double epsilon,
             double delta
         ) {
             std::vector<double> noisy_results;
             noisy_results.reserve(query_results.size());
             
             double per_query_epsilon = epsilon / query_results.size();
             double per_query_delta = delta / query_results.size();
             
             for (double val : query_results) {
                 noisy_results.push_back(
                     addNoise(val, sensitivity, per_query_epsilon, per_query_delta)
                 );
             }
             return noisy_results;
         }
         
         /**
          * @brief 高斯概率密度函数
          */
         static double pdf(double x, double sigma, double mu = 0.0) {
             return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) *
                    std::exp(-0.5 * ((x - mu) / sigma) * ((x - mu) / sigma));
         }
     };
     
     // ============================================================
     //  机制三：指数机制（Exponential Mechanism）
     // ============================================================
     
     /**
      * @brief 指数机制
      * 
      * 满足 ε-差分隐私 (ε-DP)
      * 
      * 适用于输出空间为非数值型的场景。
      * 机制以概率正比于 exp(ε·u(D,o)/(2Δu)) 输出 o ∈ O
      * 其中 u 是效用函数，Δu 是效用函数的敏感度
      * 
      * 适用场景：模型选择、中位数选择、频繁项挖掘等
      */
     class ExponentialMechanism {
     private:
         RandomGenerator rng;
         
     public:
         ExponentialMechanism() = default;
         explicit ExponentialMechanism(unsigned int seed) : rng(seed) {}
         
         /**
          * @brief 使用指数机制从候选输出中选择一个
          * @tparam T 输出类型
          * @param candidates 候选输出列表
          * @param utility_func 效用函数 u(D, o)
          * @param sensitivity 效用函数的敏感度 Δu
          * @param epsilon 隐私预算 ε
          * @return 被选择的输出
          * 
          * 通过轮盘赌（Roulette Wheel）方法进行采样
          */
         template<typename T>
         T select(
             const std::vector<T>& candidates,
             std::function<double(const T&)> utility_func,
             double sensitivity,
             double epsilon
         ) {
             if (candidates.empty()) {
                 throw std::invalid_argument("Candidates list cannot be empty");
             }
             if (epsilon <= 0.0) {
                 throw std::invalid_argument("Epsilon must be > 0");
             }
             
             // 步骤1: 计算每个候选的权重
             std::vector<double> weights;
             weights.reserve(candidates.size());
             
             double max_weight = 0.0;
             for (const auto& candidate : candidates) {
                 double utility = utility_func(candidate);
                 double weight = std::exp(epsilon * utility / (2.0 * sensitivity));
                 weights.push_back(weight);
                 max_weight = std::max(max_weight, weight);
             }
             
             // 步骤2: 数值稳定性处理——减去最大权重再exp
             // 实际上我们直接计算exp值，但为了防止溢出
             // 在指数特别大时可以做归一化处理
             
             // 步骤3: 轮盘赌采样
             double total_weight = std::accumulate(weights.begin(), weights.end(), 0.0);
             
             if (total_weight <= 0.0) {
                 // 如果所有权重为0，均匀随机选择
                 size_t idx = static_cast<size_t>(rng.uniform() * candidates.size());
                 return candidates[idx];
             }
             
             double r = rng.uniform() * total_weight;
             double cumulative = 0.0;
             
             for (size_t i = 0; i < candidates.size(); ++i) {
                 cumulative += weights[i];
                 if (cumulative >= r) {
                     return candidates[i];
                 }
             }
             
             // 防止浮点误差导致的越界
             return candidates.back();
         }
         
         /**
          * @brief 获取所有候选输出的选择概率
          */
         template<typename T>
         std::vector<double> getProbabilities(
             const std::vector<T>& candidates,
             std::function<double(const T&)> utility_func,
             double sensitivity,
             double epsilon
         ) {
             std::vector<double> probabilities;
             probabilities.reserve(candidates.size());
             
             // 计算所有权重
             std::vector<double> weights;
             weights.reserve(candidates.size());
             
             for (const auto& candidate : candidates) {
                 double utility = utility_func(candidate);
                 double weight = std::exp(epsilon * utility / (2.0 * sensitivity));
                 weights.push_back(weight);
             }
             
             double total_weight = std::accumulate(weights.begin(), weights.end(), 0.0);
             
             for (double w : weights) {
                 probabilities.push_back(w / total_weight);
             }
             
             return probabilities;
         }
     };
     
     // ============================================================
     //  隐私预算会计（Privacy Budget Accountant）
     // ============================================================
     
     /**
      * @brief 隐私预算会计器
      * 
      * 用于追踪和管理差分隐私系统中的隐私预算消耗
      * 支持序列组合定理和高级组合定理
      */
     class PrivacyBudgetAccountant {
     private:
         double total_epsilon_used;
         double total_delta_used;
         int query_count;
         
     public:
         PrivacyBudgetAccountant() 
             : total_epsilon_used(0.0), total_delta_used(0.0), query_count(0) {}
         
         /**
          * @brief 记录一次查询消耗的隐私预算（序列组合）
          * @param epsilon 本次查询使用的ε
          * @param delta 本次查询使用的δ（高斯机制）
          */
         void recordQuery(double epsilon, double delta = 0.0) {
             // 序列组合定理：ε_total = Σε_i, δ_total = Σδ_i
             total_epsilon_used += epsilon;
             total_delta_used += delta;
             query_count++;
         }
         
         /**
          * @brief 使用高级组合定理计算已用隐私预算
          * 
          * 高级组合定理：对于k次查询，总隐私损失为：
          * ε_total = ε·√(2k·ln(1/δ')) + k·ε·(e^ε - 1)
          * 其中 δ' 是我们期望的最终松弛参数
          * 
          * @param per_query_epsilon 每次查询的ε
          * @param k 查询次数
          * @param delta_prime 期望的最终δ'
          * @return 总隐私预算消耗
          */
         static double advancedComposition(
             double per_query_epsilon, 
             int k, 
             double delta_prime
         ) {
             double term1 = per_query_epsilon * 
                            std::sqrt(2.0 * k * std::log(1.0 / delta_prime));
             double term2 = k * per_query_epsilon * (std::exp(per_query_epsilon) - 1.0);
             return term1 + term2;
         }
         
         /**
          * @brief 获取当前总ε消耗
          */
         double getTotalEpsilon() const { return total_epsilon_used; }
         
         /**
          * @brief 获取当前总δ消耗
          */
         double getTotalDelta() const { return total_delta_used; }
         
         /**
          * @brief 获取查询总次数
          */
         int getQueryCount() const { return query_count; }
         
         /**
          * @brief 重置会计器
          */
         void reset() {
             total_epsilon_used = 0.0;
             total_delta_used = 0.0;
             query_count = 0;
         }
         
         /**
          * @brief 检查是否超出隐私预算
          * @param budget_epsilon 总预算ε
          * @param budget_delta 总预算δ
          */
         bool isBudgetExceeded(double budget_epsilon, double budget_delta = 0.0) const {
             return (total_epsilon_used > budget_epsilon) ||
                    (total_delta_used > budget_delta);
         }
         
         /**
          * @brief 打印隐私预算使用报告
          */
         void printReport() const {
             std::cout << "=== Privacy Budget Report ===" << std::endl;
             std::cout << "Total Epsilon Used: " << std::fixed 
                       << std::setprecision(4) << total_epsilon_used << std::endl;
             std::cout << "Total Delta Used: " << total_delta_used << std::endl;
             std::cout << "Number of Queries: " << query_count << std::endl;
             std::cout << "=============================" << std::endl;
         }
     };
     
     // ============================================================
     //  工具函数：统计评估
     // ============================================================
     
     namespace evaluation {
         
         /**
          * @brief 计算均方误差 (MSE)
          * @param true_values 真实值
          * @param noisy_values 加噪声后的值
          */
         inline double meanSquaredError(
             const std::vector<double>& true_values,
             const std::vector<double>& noisy_values
         ) {
             if (true_values.size() != noisy_values.size() || true_values.empty()) {
                 throw std::invalid_argument("Vectors must have same non-zero size");
             }
             double sum = 0.0;
             for (size_t i = 0; i < true_values.size(); ++i) {
                 double diff = true_values[i] - noisy_values[i];
                 sum += diff * diff;
             }
             return sum / true_values.size();
         }
         
         /**
          * @brief 计算平均绝对误差 (MAE)
          */
         inline double meanAbsoluteError(
             const std::vector<double>& true_values,
             const std::vector<double>& noisy_values
         ) {
             if (true_values.size() != noisy_values.size() || true_values.empty()) {
                 throw std::invalid_argument("Vectors must have same non-zero size");
             }
             double sum = 0.0;
             for (size_t i = 0; i < true_values.size(); ++i) {
                 sum += std::abs(true_values[i] - noisy_values[i]);
             }
             return sum / true_values.size();
         }
         
         /**
          * @brief 多次实验计算平均MSE
          * @param num_trials 实验次数
          * @param func 执行一次查询的函数，返回 (真实值, 噪声值)
          */
         double averageMSE(
             int num_trials,
             std::function<std::pair<double, double>()> func
         ) {
             double total_mse = 0.0;
             for (int i = 0; i < num_trials; ++i) {
                 auto result = func();
                 double error = result.first - result.second;
                 total_mse += error * error;
             }
             return total_mse / num_trials;
         }
         
         /**
          * @brief 生成实验报告表格
          */
         void printComparisonTable(
             const std::vector<double>& epsilon_values,
             const std::vector<double>& laplace_mse,
             const std::vector<double>& gaussian_mse,
             const std::vector<double>& exponential_mse
         ) {
             const int w = 16;
             std::cout << "\n========== 差分隐私算法比较 ==========\n";
             std::cout << std::left;
             std::cout << std::setw(w) << "ε" 
                       << std::setw(w) << "拉普拉斯MSE" 
                       << std::setw(w) << "高斯MSE" 
                       << std::setw(w) << "指数MSE" << std::endl;
             std::cout << std::string(64, '-') << std::endl;
             
             for (size_t i = 0; i < epsilon_values.size(); ++i) {
                 std::cout << std::setw(w) << epsilon_values[i]
                           << std::setw(w) << laplace_mse[i]
                           << std::setw(w) << gaussian_mse[i]
                           << std::setw(w) << exponential_mse[i] << std::endl;
             }
             std::cout << "========================================\n" << std::endl;
         }
     }
     
 } // namespace differential_privacy
 
 #endif // DIFFERENTIAL_PRIVACY_H
