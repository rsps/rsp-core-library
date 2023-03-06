/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef __GNUC__

#include <stdio.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <ostream>
#include <sstream>
#include <exceptions/BackTrace.h>
#include <filesystem>

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
    using namespace abi;

    void* trace[MAX_DEPTH];
    char **strings;
    int stack_depth = backtrace(trace, MAX_DEPTH);

    strings = backtrace_symbols(trace, stack_depth);
    if (strings == NULL) {
        return;
    }

    for (int j = 0; j < stack_depth; j++) {
        const char* symbol_name = strings[j];
        std::string s(symbol_name);

        std::string file;
        std::string function;
        std::string offset;

        size_t start = s.find_first_of('(', 0);
        if (start != s.npos) {
            file = s.substr(0, start);
            file = std::filesystem::path(file).stem();

            size_t end = s.find_first_of("+)", start);
            std::string mangled = s.substr(start+1, end-1-start);
            function = demangle(mangled);
            offset = s.substr(end, s.length());
        }

        mStackEntries.emplace_back(file, function, offset);
    }

    free(strings);
}

std::string BackTrace::demangle(const std::string &arMangled) const
{
    std::string result = arMangled;
    int status;
    char* demangled = abi::__cxa_demangle(arMangled.c_str(), NULL, 0, &status);
    if(status == 0 && demangled) {
        result = demangled;
    }
    if (demangled) {
        free(demangled);
    }
    return result;
}

} /* namespace rsp::exceptions */

#endif /* __GNUC__ */

