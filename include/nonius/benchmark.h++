// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Benchmark

#ifndef NONIUS_BENCHMARK_HPP
#define NONIUS_BENCHMARK_HPP

#include <nonius/clock.h++>
#include <nonius/configuration.h++>
#include <nonius/environment.h++>
#include <nonius/execution_plan.h++>
#include <nonius/detail/measure.h++>
#include <nonius/detail/benchmark_function.h++>
#include <nonius/detail/repeat.h++>
#include <nonius/detail/run_for_at_least.h++>

#include <boost/chrono.hpp>

#include <algorithm>
#include <functional>
#include <string>

namespace nonius {
    namespace detail {
        constexpr auto warmup_iterations = 10000;
        constexpr auto warmup_time = boost::chrono::milliseconds(100);
        constexpr auto minimum_ticks = 1000;
    } // namespace detail

    struct benchmark {
        std::string name;
        detail::benchmark_function function;

        void operator()(int k) const {
            detail::repeat(std::ref(function))(k);
        }

        template <typename Clock>
        execution_plan<FloatDuration<Clock>> prepare(configuration cfg, environment<FloatDuration<Clock>> env) const {
            auto min_time = env.clock_resolution.mean * detail::minimum_ticks;
            auto run_time = std::min(min_time, decltype(min_time)(detail::warmup_time));
            auto&& test = detail::run_for_at_least<Clock>(boost::chrono::duration_cast<Duration<Clock>>(run_time), 1, *this);
            int new_iters = std::ceil(min_time * test.iterations / test.elapsed);
            return { new_iters, test.elapsed / test.iterations * new_iters * cfg.samples };
        }

        template <typename Clock>
        std::vector<FloatDuration<Clock>> run(configuration cfg, environment<FloatDuration<Clock>> env, execution_plan<FloatDuration<Clock>> plan) const {
            // warmup a bit
            detail::run_for_at_least<Clock>(boost::chrono::duration_cast<Duration<Clock>>(detail::warmup_time), detail::warmup_iterations, detail::repeat(now<Clock>{}));

            std::vector<FloatDuration<Clock>> times;
            times.reserve(cfg.samples);
            std::generate_n(std::back_inserter(times), cfg.samples, [this, env, plan]{
                    auto t = detail::measure<Clock>(*this, plan.iterations_per_sample).elapsed;
                    return ((t - env.clock_cost.mean) / plan.iterations_per_sample);
            });
            return times;
        }
    };
} // namespace nonius

#endif // NONIUS_BENCHMARK_HPP
