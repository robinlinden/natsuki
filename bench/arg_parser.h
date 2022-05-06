// SPDX-FileCopyrightText: 2022 Robin Lindén <dev@robinlinden.eu>
//
// SPDX-License-Identifier: MIT

#ifndef NATSUKI_BENCH_ARG_PARSER_H_
#define NATSUKI_BENCH_ARG_PARSER_H_

#include <charconv>
#include <concepts>
#include <functional>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace bench {

class ArgParser {
public:
    template<std::integral ArgT>
    [[nodiscard]] ArgParser &argument(std::string_view long_option, ArgT &out) {
        long_[long_option] = [&out](std::string_view argument) {
            auto [ptr, ec] = std::from_chars(argument.data(), argument.data() + argument.size(), out);
            if (ec != std::errc{}) {
                throw std::system_error(std::make_error_code(ec));
            }
        };
        return *this;
    }

    [[nodiscard]] ArgParser &argument(std::string_view arg, bool &was_passed) {
        store_true_[arg] = [&was_passed]() {
            was_passed = true;
        };
        return *this;
    }

    [[nodiscard]] ArgParser &positional(std::string &out) {
        positional_.push_back([&out](std::string_view argument) {
            out = std::string{argument};
        });
        return *this;
    }

    void parse(int argc, char **argv) {
        for (int i = 1; i < argc; ++i) {
            auto arg = std::string_view{argv[i]};
            if (long_.contains(arg)) {
                if (i + 1 == argc) {
                    std::stringstream ss;
                    ss << "Missing count after " << arg << '\n';
                    throw std::invalid_argument(ss.str());
                }
                long_.at(arg)(argv[i + 1]);
                ++i;
                continue;
            }

            if (store_true_.contains(arg)) {
                store_true_.at(arg)();
                continue;
            }

            int maybe_positional = i + static_cast<int>(positional_.size()) - argc;
            if (maybe_positional >= 0 && maybe_positional < static_cast<int>(positional_.size())) {
                positional_[maybe_positional](arg);
                continue;
            }

            std::stringstream ss;
            ss << "Unhandled argument " << arg << '\n';
            throw std::invalid_argument(ss.str());
        }
    }

private:
    std::map<std::string_view, std::function<void(std::string_view)>> long_;
    std::map<std::string_view, std::function<void()>> store_true_;
    std::vector<std::function<void(std::string_view)>> positional_;
};

} // namespace bench

#endif
