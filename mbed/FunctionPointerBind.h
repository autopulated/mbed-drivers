/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBED_FUNCTIONPOINTERBIND_H__
#define MBED_FUNCTIONPOINTERBIND_H__

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <assert.h>
#include "FunctionPointerBase.h"

#ifndef EVENT_STORAGE_SIZE
#define EVENT_STORAGE_SIZE 32
#endif

#define MBED_STATIC_ASSERT(MBED_STATIC_ASSERT_FAILED,MSG)\
    switch(0){\
        case 0:case (MBED_STATIC_ASSERT_FAILED): \
        break;}

namespace mbed{

template<typename R>
class FunctionPointerBind : FunctionPointerBase<R> {
public:
    // Call the Event
    inline R call() {
        return FunctionPointerBase<R>::call(static_cast<void *>(_storage));
    }
    FunctionPointerBind() {

    }
    FunctionPointerBind(const FunctionPointerBind<R> & fp) :
        FunctionPointerBase<R>::FunctionPointerBase(fp)
    {
        this->_ops->copy_args(this->_storage, (void *)fp._storage);
    }
    ~FunctionPointerBind() {
        this->_ops->destructor(this->_storage);
    }

    template<typename S>
    FunctionPointerBind<R> & bind(S * argStruct, FunctionPointerBase<R>* fp, ...) {
        MBED_STATIC_ASSERT(sizeof(S) <= sizeof(_storage), ERROR: Arguments too large for FunctionPointerBind internal storage)
        this->copy(fp);
        assert(this->_ops != NULL);
        assert(this->_ops->constructor != NULL);
        if (argStruct) {
            this->_ops->copy_args(this->_storage, (void *)argStruct);
        } else {
            va_list args;
            va_start(args, fp);
            this->_ops->constructor(_storage, args);
            va_end(args);
        }
        return *this;
    }

private:
    uint32_t _storage[(EVENT_STORAGE_SIZE+sizeof(uint32_t)-1)/sizeof(uint32_t)];
};
typedef FunctionPointerBind<void> Event;
}

#endif // MBED_FUNCTIONPOINTERBIND_H__
