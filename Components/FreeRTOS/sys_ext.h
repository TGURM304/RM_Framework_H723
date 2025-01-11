//
// Created by fish on 2024/9/7.
//

#pragma once

namespace OS {
    template <typename ResultType, typename ArgType, typename... Args>
    class TypeErasure {
      public:
        ArgType arg_;
        ResultType (*fun_) (ArgType, Args... args);
        TypeErasure(ResultType (*fun)(ArgType, Args... args), ArgType arg) : fun_(fun), arg_(arg) {}
        static ResultType Port(void *addr, Args... args) {
            auto *self = static_cast <TypeErasure*>(addr);
            return self->fun_(self->arg_, args...);
        }
    };
}
