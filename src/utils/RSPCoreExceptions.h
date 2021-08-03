#ifndef RSPCOREEXCEPTIONS_H
#define RSPCOREEXCEPTIONS_H

#include <exception>
#include <stdexcept>
#include <system_error>

class RSPCoreException : public std::runtime_error {
   public:
    explicit RSPCoreException(const char* aMsg) : std::runtime_error("") {
        mMsg = aMsg;
    }

    const char* what() const noexcept override {
        return mMsg.c_str();
    }

   protected:
    std::string mMsg;
};

class NotImplementedException : public RSPCoreException {
   public:
    explicit NotImplementedException(const char* aMsg) : RSPCoreException("Not Implemented yet") {}
};

#endif  // RSPCOREEXCEPTIONS_H