/* Copyright (c) 2016-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Context.hpp"
#include "../platform/Platform.hpp"

#include "model/Vertex.hpp"

namespace Tobi
{

Context::Context()
    : platform(Platform::create())
{
    LOGI("CONSTRUCTING Context\n");
}

Context::~Context()
{
    LOGI("DECONSTRUCTING Context\n");
}

Result Context::initialize()
{
    LOGI("START INITIALIZING Context\n");
    auto result = platform->initialize();
    if (FAILED(result))
    {
        LOGE("Failed to initialize Platform\n");
    }

    // attach context to the platform ?? does it need the context in any way?

    // "onPlatformUpdate" things here

    LOGI("FINISHED INITIALIZING Context\n");
    return RESULT_SUCCESS;
}

} // namespace Tobi
