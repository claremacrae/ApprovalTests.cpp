#include <string>
#include <vector>

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cassert>

namespace Detail {

    // C++14 compatibility
    // See https://en.cppreference.com/w/cpp/types/enable_if
    template<bool B, class T=void>
    using enable_if_t = typename std::enable_if<B, T>::type;

    // See https://en.cppreference.com/w/cpp/utility/integer_sequence
    template<std::size_t... Is>
    struct index_sequence {};

    template<std::size_t N, std::size_t... Is>
    struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};

    template<std::size_t... Is>
    struct make_index_sequence<0, Is...> : index_sequence<Is...> {};
    // End of C++14 compatibility

    // Return the size of a tuple - constexpr for use as a template argument
    // See https://en.cppreference.com/w/cpp/utility/tuple/tuple_size
    template<class Tuple>
    constexpr std::size_t tuple_size() {
        return std::tuple_size<typename std::remove_reference<Tuple>::type>::value;
    }

    template<class Tuple>
    using make_tuple_idxs = make_index_sequence<tuple_size<Tuple>()>;

    // C++17 compatibility
    // See https://en.cppreference.com/w/cpp/utility/apply
    template <class F, class Tuple, std::size_t... I>
    constexpr auto apply_impl(F&& f, Tuple&& t, index_sequence<I...>)
    -> decltype(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...))
    {
        return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <class F, class Tuple>
    auto apply(F&& f, Tuple&& t)
    -> decltype(apply_impl(std::forward<F>(f), std::forward<Tuple>(t), make_tuple_idxs<Tuple>{}))
    {
        apply_impl(std::forward<F>(f), std::forward<Tuple>(t), make_tuple_idxs<Tuple>{});
    }
    // End of C++17 compatibility

    template <class Tuple, class F, std::size_t... Is>
    void for_each_impl(Tuple&& t, F&& f, index_sequence<Is...>) {
        (void)std::initializer_list<int>{
                (std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(t))), 0)...
        };
    }

    template <class Tuple, class F>
    void for_each(Tuple&& t, F&& f) {
        for_each_impl(std::forward<Tuple>(t), std::forward<F>(f), make_tuple_idxs<Tuple>{});
    }

    template<class Tuple, class F, std::size_t... Is>
    auto transform_impl(Tuple&& t, F&& f, index_sequence<Is...>)
    -> decltype(std::make_tuple(std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(t)))...))
    {
        return std::make_tuple(std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(t)))...);
    }

    template<class F, class Tuple>
    auto transform(Tuple&& t, F&& f = {})
    -> decltype(transform_impl(std::forward<Tuple>(t), std::forward<F>(f), make_tuple_idxs<Tuple>{}))
    {
        return transform_impl(std::forward<Tuple>(t), std::forward<F>(f), make_tuple_idxs<Tuple>{});
    }

    template<class Predicate>
    struct find_if_body {
        const Predicate& pred;
        std::size_t& index;
        std::size_t currentIndex = 0;
        bool found = false;

        find_if_body(const Predicate& p, std::size_t& i) : pred(p), index(i) {}

        template<typename T>
        void operator()(T&& value) {
            if (found) return;
            if (pred(std::forward<T>(value))) {
                index = currentIndex;
                found = true;
            }
            ++currentIndex;
        }
    };

    template<class Predicate, class Tuple>
    std::size_t find_if(Tuple&& tuple, Predicate pred = {}) {
        std::size_t idx = tuple_size<Tuple>();
        for_each(std::forward<Tuple>(tuple), find_if_body<Predicate>(pred, idx));
        return idx;
    }

    template<class Predicate, class Tuple>
    bool any_of(Tuple&& tuple, Predicate pred = {}) {
        return find_if(std::forward<Tuple>(tuple), pred) != tuple_size<Tuple>();
    }

    struct is_range_empty {
        template<class T>
        bool operator()(const T& range) const {
            using std::begin;
            using std::end;
            return begin(range) == end(range);
        }
    };

    // Transform an iterator into a value reference which will then be passed to the visitor function:
    struct dereference_iterator {
        template<class It>
        auto operator()(It&& it) const -> decltype(*std::forward<It>(it)) {
            return *std::forward<It>(it);
        }
    };

    // Increment outermost iterator. If it reaches its end, we're finished and do nothing.
    template<class Its, std::size_t I = tuple_size<Its>()-1>
    enable_if_t<I == 0>
    increment_iterator(Its& it, const Its&, const Its&) {
        ++std::get<I>(it);
    }

    // Increment inner iterator. If it reaches its end, we reset it and increment the previous iterator.
    template<class Its, std::size_t I = tuple_size<Its>()-1>
    enable_if_t<I != 0>
    increment_iterator(Its& its, const Its& begins, const Its& ends) {
        if (++std::get<I>(its) == std::get<I>(ends)) {
            std::get<I>(its) = std::get<I>(begins);
            increment_iterator<Its, I-1>(its, begins, ends);
        }
    }
} // namespace Detail

// This is what actually loops over all the containers, one element at a time
// It is called with a template type F that writes the inputs, and runs the converter, which writes the result(s)
// all for one set of container values - when called by verifyAllCombinations()
// More generally, F must have an operator() that acts on one set of input values.
template<class F, class... Ranges>
void cartesian_product(F&& f, const Ranges&... ranges) {
    using std::begin;
    using std::end;

    if (Detail::any_of<Detail::is_range_empty>(std::forward_as_tuple(ranges...)))
        return;

    const auto begins = std::make_tuple(begin(ranges)...);
    const auto ends = std::make_tuple(end(ranges)...);

    for (auto its = begins; std::get<0>(its) != std::get<0>(ends); Detail::increment_iterator(its, begins, ends))
    {
        // *********************************************************************************
        // Command-clicking on transform in CLion 2019.2.4 hangs with CLion with high CPU,
        // popping up 'Resolving reference' and then being unresponsive,
        // needing to be force-quit
        //
        // Settings:
        // * 'Preferences / Languages and Frameworks /  C/C++ / Clangd / Enable clangd server' is turned off.
        // * Power-save Mode turned on.
        // * Running on Mac, XCode 10.3
        //
        // Note that it also hangs, without the 'Resolving reference' popup, if clangd server is on
        //
        // A possibly quicker way to detect the issue is to hold down the Command key and hover
        // over the various words below.
        // Hovering over most words gives a blue "hyperlink" underline,
        // but not when hovering over either apply or transform
        //
        // Renaming apply and transform to e.g. zzzz1 and zzzz2 doesn't fix the problem,
        // suggesting that the cause is not the coincidence of the names with std::
        // equivalents
        //
        // Same problem in CLion 2019.3 EAP - Build #CL-193.4386.19 - built on October 10, 2019
        // CLion 2019.1.4: Gives more errors, like std::get not recognised
        Detail::apply(std::forward<F>(f), Detail::transform<Detail::dereference_iterator>(its));
        // *********************************************************************************
    }
}

using Results = std::vector<std::string>;

// ------------------------------------------------------------------
// Tests which use a non-template object for accumulating results
// from CartesianProduct::cartesian_product(), to show the simplest
// way of using that function.
// ------------------------------------------------------------------

namespace
{
// A hard-coded struct for accumulating results
    struct AccumulateResults2StringsCommaSeparated
    {
        Results results;
        void operator()(std::string&& s1, std::string&& s2)
        {
            results.push_back(s1 + "," + s2);
        }
    };
}

int main()
{
    const std::vector<std::string> input1{"hello"};
    const std::vector<std::string> input2{"world"};
    AccumulateResults2StringsCommaSeparated results_store;

    cartesian_product(results_store, input1, input2);

    const Results expected{"hello,world"};
    assert(results_store.results == expected);
}

