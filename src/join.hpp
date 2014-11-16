#pragma once

#include <string>
#include <sstream>

namespace util
{
	using std::stringstream;

    template <typename It>
    string join(const char* joint, It begin, const It& end) {
        if (begin == end)
            return "";
        stringstream ss;
        ss << *begin;
        ++begin;
        for (; begin != end; ++begin)
            ss << joint << *begin;
        //return std::move(ss.str());
        return ss.str();
    }

    template <typename Range>
    string join(const char* joint, const Range& range) {
        return join(joint, range.begin(), range.end());
    }

    template <typename Range>
    string join(const Range& range) {
        return join(", ", range.begin(), range.end());
    }

    template <typename It, typename Filter>
    string mapjoin(const char* joint, It begin, const It& end, const Filter& filter) {
        if (begin == end)
            return "";
        stringstream ss;
        ss << filter(*begin);
        ++begin;
        for (; begin != end; ++begin)
            ss << joint << filter(*begin);
        return ss.str();
    }

    template <typename It, typename Pred>
    string join_if(const char* joint, It begin, const It& end, const Pred& pred) {
        if (begin == end)
            return "";
        bool f = false;
        stringstream ss;
        for (; begin != end; ++begin) {
            if (pred(*begin)) {
                if (f)
                    ss << joint;
                ss << *begin;
                f = true;
            }
        }
        return ss.str();
    }

}
