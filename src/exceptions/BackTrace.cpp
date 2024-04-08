/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <exceptions/BackTrace.h>
#if defined(_GLIBCXX_HAVE_STACKTRACE)
    #include <stacktrace>
#elif defined(__x86_64__)
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <cstdlib>
    #include <filesystem>
#endif // _GLIBCXX_HAVE_STACKTRACE, __x86_64__

#define MAX_DEPTH 32


namespace rsp::exceptions {

std::ostream& operator <<(std::ostream &o, const StackEntry &arEntry)
{
    o << arEntry.mFileName << "  " << arEntry.mFunction << " (" << arEntry.mLineNumber << ")";
    return o;
}

std::ostream& operator <<(std::ostream &o, const BackTrace &arBackTrace)
{
    for (auto& e : arBackTrace.mStackEntries) {
        o << e << "\n";
    }
    return o;
}


BackTrace::BackTrace(size_t aEntriesToDiscard)
{
#if defined(_GLIBCXX_HAVE_STACKTRACE)
    for (auto &st : std::stacktrace::current()) {
        mStackEntries.emplace_back(st.source_file(), st.description(), std::to_string(st.source_line()));
    }
#elif defined(__x86_64__)
    using namespace abi;

    void* trace[MAX_DEPTH]{};
    char **strings;
    int stack_depth = backtrace(trace, MAX_DEPTH);

    strings = backtrace_symbols(trace, stack_depth);
    if (strings == nullptr) {
        return;
    }

    for (size_t j = aEntriesToDiscard; j < size_t(stack_depth); j++) {
        const char* symbol_name = strings[j];
        std::string s(symbol_name);

        std::string file;
        std::string function;
        std::string offset;

        size_t start = s.find_first_of('(', 0);
        if (start != std::string::npos) {
            file = s.substr(0, start);
            file = std::filesystem::path(file).stem();

            size_t end = s.find_first_of("+)", start);
            std::string mangled = s.substr(start+1, end-1-start);
            function = BackTrace::demangle(mangled);
            offset = s.substr(end, s.length());
        }

        mStackEntries.emplace_back(file, function, offset);
    }

    free(strings);
#endif // _GLIBCXX_HAVE_STACKTRACE, __x86_64__
}

#if !defined(_GLIBCXX_HAVE_STACKTRACE) && defined(__x86_64__)
std::string BackTrace::demangle(const std::string &arMangled)
{
    std::string result;
    int status;
    char* demangled = abi::__cxa_demangle(arMangled.c_str(), nullptr, nullptr, &status);
    if(status == 0 && demangled) {
        result = demangled;
    }
    else {
        result = arMangled;
    }
    if (demangled) {
        free(demangled);
    }
    return result;
}
#endif // !_GLIBCXX_HAVE_STACKTRACE, __x86_64__

} /* namespace rsp::exceptions */

