// Given a CSV list of jobs with IDs, runtimes in seconds, and the next job ID
// pointer, detect chains of jobs and report for each chain the start, end,
// count, total runtime and average runtime, or detect malformed input.
//
// Ex:
//
// From standard input:
//
// job_id,runtime_in_seconds,next_job_id
// 1,120,3
// 3,300,0
// 2,60,3
// ...

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using Id        = std::uint64_t;
using Runtime   = std::uint64_t;
using DataTuple = std::tuple<Id, Runtime, Id>;

struct Job
{
    Id id{};
    Runtime runtime{};
    Id next_id{};
};

class Reporter {
   public:
    bool add_job(const Job& job) {
        Node& node = nodes_[job.id];
        if (node.has_job)
        {
            std::cerr << "duplicate job_id: " << job.id << '\n';
            return false;
        }

        node.job     = job;
        node.has_job = true;

        if (job.next_id != 0)
        {
            Node& next = nodes_[job.next_id];
            if (++next.indegree > 1)
            {
                std::cerr << "job_id " << job.next_id
                          << " has multiple parents\n";
                return false;
            }
        }
        return true;
    }

    bool get_chains() {
        if (nodes_.empty())
        {
            std::cerr << "no jobs provided\n";
            return false;
        }

        for (const auto& [id, node] : nodes_)
        {
            if (!node.has_job)
            {
                std::cerr << "job_id " << id << " referenced but missing\n";
                return false;
            }
        }

        std::vector<ChainStats> chains;
        chains.reserve(nodes_.size());

        for (auto& [id, node] : nodes_)
        {
            if (!node.has_job || node.indegree != 0 || node.visited) continue;
            auto stats = build_chain(id);
            if (!stats.has_value()) return false;
            chains.push_back(*stats);
        }

        for (auto& [id, node] : nodes_)
        {
            if (!node.has_job || node.visited) continue;
            auto stats = build_chain(id);
            if (!stats.has_value()) return false;
            chains.push_back(*stats);
        }

        if (chains.empty())
        {
            std::cerr << "no valid chains found\n";
            return false;
        }

        for (const auto& chain : chains)
        {
            const double avg = chain.count == 0
                                   ? 0.0
                                   : static_cast<double>(chain.total_runtime) /
                                         static_cast<double>(chain.count);
            std::cout << "chain start=" << chain.start << ", end="
                      << chain.end << ", count=" << chain.count
                      << ", total_runtime=" << chain.total_runtime
                      << ", average_runtime=" << avg << '\n';
        }

        return true;
    }

   private:
    struct Node {
        Job job{};
        bool has_job{ false };
        std::uint32_t indegree{ 0 };
        bool visiting{ false };
        bool visited{ false };
    };

    struct ChainStats {
        Id start{};
        Id end{};
        std::size_t count{};
        Runtime total_runtime{};
    };

    std::optional<ChainStats> build_chain(Id start) {
        ChainStats stats{ .start = start, .end = start, .count = 0, .total_runtime = 0 };
        std::vector<Id> path;
        path.reserve(nodes_.size());

        Id current = start;
        while (current != 0)
        {
            auto it = nodes_.find(current);
            if (it == nodes_.end() || !it->second.has_job)
            {
                std::cerr << "job_id " << current << " referenced but missing\n";
                clear_visiting(path);
                return std::nullopt;
            }

            Node& node = it->second;
            if (node.visiting)
            {
                std::cerr << "cycle detected involving job_id " << current
                          << '\n';
                clear_visiting(path);
                return std::nullopt;
            }

            if (node.visited)
            {
                if (!path.empty())
                {
                    stats.end = nodes_.at(path.back()).job.id;
                } else
                {
                    stats.end = node.job.id;
                }
                break;
            }

            node.visiting = true;
            path.push_back(current);

            ++stats.count;
            stats.total_runtime += node.job.runtime;
            stats.end = node.job.id;

            current = node.job.next_id;
        }

        for (Id id : path)
        {
            Node& node = nodes_.at(id);
            node.visiting = false;
            node.visited  = true;
        }

        return stats;
    }

    void clear_visiting(const std::vector<Id>& path) {
        for (Id id : path)
        {
            auto it = nodes_.find(id);
            if (it != nodes_.end()) it->second.visiting = false;
        }
    }

    std::unordered_map<Id, Node> nodes_{};
};

class Parser {
   public:
    [[nodiscard]]
    static bool parse_header(std::string_view line) {
        for (std::string_view expected : header_words)
        {
            auto comma_pos         = line.find(',');
            std::string_view token = (comma_pos == std::string_view::npos)
                                         ? line
                                         : line.substr(0, comma_pos);
            if (token != expected) return false;

            if (comma_pos == std::string_view::npos)
            {
                line = std::string_view{};
            } else
            {
                line.remove_prefix(comma_pos + 1);
            }
        }

        // no extra columns after the expected header words
        return line.empty();
    }

    static Id get_id(std::string_view tok) {
        Id num{ 0 };

        for (char c : tok)
        {
            if (c <= '0' && c >= '9')
            {
                std::cout << "input is tripping" << std::endl;
            }
            num = num * 10 + (c - '0');
        }
        return num;
    }

    static Runtime get_runtime(std::string_view tok) {
        Runtime num{ 0 };

        for (char c : tok)
        {
            if (c <= '0' && c >= '9')
            {
                std::cout << "input is tripping" << std::endl;
            }
            num = num * 10 + (c - '0');
        }
        return num;
    }

    [[deprecated(
        "Just use get_line_as_job instead.")]] [[nodiscard]] static DataTuple
    get_line_as_tuple(std::string_view line) {
        const char del = ',';

        std::string_view::size_type pos = line.find(del);
        std::string_view tok            = line.substr(0, pos);
        auto id                         = get_id(tok);

        line.remove_prefix(pos + 1);
        pos          = line.find(del);
        tok          = line.substr(0, pos);
        auto runtime = get_runtime(tok);

        line.remove_prefix(pos + 1);
        tok          = line;
        auto next_id = get_id(tok);

        return DataTuple{ id, runtime, next_id };
    }

    [[nodiscard]] static Job get_line_as_job(std::string_view line) {
        const char del                  = ',';
        std::string_view::size_type pos = line.find(del);
        std::string_view tok            = line.substr(0, pos);
        auto id                         = get_id(tok);

        line.remove_prefix(pos + 1);
        pos          = line.find(del);
        tok          = line.substr(0, pos);
        auto runtime = get_runtime(tok);

        line.remove_prefix(pos + 1);
        tok          = line;
        auto next_id = get_id(tok);

        return Job{ id, runtime, next_id };
    }

   private:
    static constexpr std::string_view header_words[] = { "job_id", "runtime",
                                                         "next" };
};

int main() {
#define STDIN
#ifdef STDIN
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    auto& input_stream = std::cin;
#endif

    // parse header (validate) and skip
    std::string line;
    if (!std::getline(input_stream, line))
    {
        std::cerr << "empty input" << std::endl;
        return EXIT_FAILURE;
    }

    const bool parse_result = Parser::parse_header(line);
    if (!parse_result)
    {
        std::cerr << "bad header fam" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "header parsed" << std::endl;

    Reporter reporter;

    Job job;

    while (std::getline(input_stream, line))
    {
        job = Parser::get_line_as_job(line);
        if (!reporter.add_job(job)) return EXIT_FAILURE;
    }

    if (!reporter.get_chains()) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}