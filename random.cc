#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <map>
#include <print>
#include <random>
#include <ranges>
#include <thread>

class XorBitant {
private:
    std::uint64_t m_state { 0xDEADBEEFBABECAFE };

public:
    using result_type = std::uint32_t;

    // Constructor to seed the generator
    explicit XorBitant(std::uint64_t seed = 0xDEADBEEFBABECAFE)
        : m_state(seed)
    {
    }

    // Required: min value
    static constexpr result_type min()
    {
        return 0;
    }

    // Required: max value
    static constexpr result_type max()
    {
        return std::numeric_limits<std::uint32_t>::max();
    }

    // Generate next random number
    result_type operator()()
    {
        std::uint64_t x = m_state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 5;
        m_state = x;
        return static_cast<std::uint32_t>(x);
    }
};

// Chi-squared test for uniformity
double chi_squared_test(const std::map<int, int>& hist, int expected_count)
{
    double chi_squared = 0.0;
    for (const auto& [value, count] : hist) {
        double diff = count - expected_count;
        chi_squared += (diff * diff) / expected_count;
    }
    return chi_squared;
}

// Calculate entropy (higher is better for randomness)
double calculate_entropy(const std::map<int, int>& hist, int total_samples)
{
    double entropy = 0.0;
    for (const auto& [value, count] : hist) {
        if (count > 0) {
            double probability = static_cast<double>(count) / total_samples;
            entropy -= probability * std::log2(probability);
        }
    }
    return entropy;
}

// Calculate standard deviation of counts (lower is better for uniformity)
double calculate_std_dev(const std::map<int, int>& hist, double expected_count)
{
    double sum_squared_diff = 0.0;
    for (const auto& [value, count] : hist) {
        double diff = count - expected_count;
        sum_squared_diff += diff * diff;
    }
    return std::sqrt(sum_squared_diff / hist.size());
}

int main()
{

    // testing out seed variations
    std::vector<std::pair<std::string, std::uint64_t>> seeds = {
        { "Single clock", std::chrono::steady_clock::now().time_since_epoch().count() },
        { "Double clock", std::chrono::steady_clock::now().time_since_epoch().count() ^ std::chrono::system_clock::now().time_since_epoch().count() },
        { "Triple (with thread)", std::chrono::steady_clock::now().time_since_epoch().count() ^ std::chrono::system_clock::now().time_since_epoch().count() ^ std::hash<std::thread::id> {}(std::this_thread::get_id()) },
        { "Fixed seed", 0xDEADBEEFBABECAFE }
    };

    constexpr int num_samples = 10'000;
    constexpr double expected_mean = 0.0;
    constexpr double expected_std_dev = 5.0;

    std::println("Testing {} different seeding methods with {} samples each:\n", seeds.size(), num_samples);
    std::println("Expected: Mean = {:.2f}, Std Dev = {:.2f}\n", expected_mean, expected_std_dev);

    for (const auto& [name, seed] : seeds) {
        XorBitant xor_gen { seed };
        std::normal_distribution dist(expected_mean, expected_std_dev);

        // Generate samples and calculate statistics
        double sum = 0.0;
        double sum_squared = 0.0;
        std::map<int, int> hist;

        for (int i = 0; i < num_samples; ++i) {
            double value = dist(xor_gen);
            ++hist[static_cast<int>(std::round(value))];
            sum += value;
            sum_squared += value * value;
        }

        double actual_mean = sum / num_samples;
        double variance = (sum_squared / num_samples) - (actual_mean * actual_mean);
        double actual_std_dev = std::sqrt(variance);

        double mean_error = std::abs(actual_mean - expected_mean);
        double std_dev_error = std::abs(actual_std_dev - expected_std_dev);
        double chi_squared = chi_squared_test(hist, num_samples / hist.size());

        std::println("Seed: {}", name);
        std::println("  Actual Mean: {:.4f} (error: {:.4f})", actual_mean, mean_error);
        std::println("  Actual Std Dev: {:.4f} (error: {:.4f})", actual_std_dev, std_dev_error);
        std::println("  Chi-squared: {:.2f}", chi_squared);
        std::println("  Quality Score: {:.2f}% \n",
            100.0 * (1.0 - std::min(1.0, (mean_error + std_dev_error) / 2.0)));
    }

    // Show histogram for best seed (triple)
    std::uint64_t best_seed = seeds[2].second;
    XorBitant xor_gen { best_seed };
    std::map<int, int> hist;
    std::normal_distribution dist(expected_mean, expected_std_dev);

    // plot ascii histogram using the `hist` map and the generated XorBitant numbers
    std::ranges::for_each(std::views::iota(0, num_samples),
        [&xor_gen, &hist, &dist](auto) { ++hist[static_cast<int>(std::round(dist(xor_gen)))]; });
    std::println("\nHistogram of normal distribution, with PRNG by XorBitant (triple seed):");
    std::uint16_t scaling_factor { 10 };
    std::ranges::for_each(hist, [scaling_factor](const auto& pair) {
        std::print("{:3} | ", pair.first);
        for (int i = 0; i < pair.second / scaling_factor; ++i)
            std::print("*");
        std::println(" ({})", pair.second);
    });

    // Shuffle a range/vector
    std::println("\n\nOriginal:");
    auto rng = std::views::iota(0, 10) | std::ranges::to<std::vector<int>>();
    std::ranges::for_each(rng, [](const int el) { std::print("{} ", el); });

    std::println("\nAfter shuffling with XorBitant:");
    std::ranges::shuffle(rng, xor_gen);
    std::ranges::for_each(rng, [](const int el) { std::print("{} ", el); });

    // construct a vector of XorBitant structs using ranges and ranges::to<std::vector<XorBitant>>
    auto xor_vector = std::views::iota(0, 5)
        | std::views::transform([&seeds](auto i) { return XorBitant { seeds[2].second + (std::numeric_limits<std::uint32_t>::max() - i * (1 << 16)) }; })
        | std::ranges::to<std::vector<XorBitant>>();
    std::ranges::for_each(xor_vector, [](XorBitant& el) { std::print("{:#} ", el()); });

    return EXIT_SUCCESS;
}