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

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>

#include "framework/Common.hpp"

namespace Tobi
{
/// @brief The asset manager reads data from a platform specific location.
/// This class is used internally to load binary data from disk.
class AssetManager
{
  public:
    AssetManager();
    AssetManager(const AssetManager &) = delete;
    AssetManager(AssetManager &&) = delete;
    AssetManager &operator=(const AssetManager &) & = delete;
    AssetManager &operator=(AssetManager &&) & = delete;
    ~AssetManager() = default;

    /// @brief Reads a binary file into typed container.
    /// @param[out] pOutput Output vector to write data into.
    /// The vector will be cleared before adding any data to the container.
    /// @param[out] pPath The path to read.
    /// @returns Error code
    template <typename T>
    inline Result readBinaryFile(std::vector<T> *pOutput, const char *pPath)
    {
        void *pData;
        size_t size;
        Result error = readBinaryFile(pPath, &pData, &size);
        if (error != RESULT_SUCCESS)
            return error;

        size_t numElements = size / sizeof(T);

        pOutput->clear();
        pOutput->insert(end(*pOutput), reinterpret_cast<T *>(pData), reinterpret_cast<T *>(pData) + numElements);
        free(pData);
        return RESULT_SUCCESS;
    }

    /// @brief Reads a binary file as a raw blob.
    /// @param pPath The path of the asset.
    /// @param[out] ppData allocated output data. Must be freed with `free()`.
    /// @param[out] pSize The size of the allocated data.
    /// @returns Error code
    Result readBinaryFile(const char *pPath, void **ppData, size_t *pSize);

  private:
    std::string basePath;
};

namespace OS
{

/// @brief Get the current monotonic time in seconds.
/// @returns Current time.
double getCurrentTime();

/// @brief Get the asset manager for the platform.
/// @returns The asset manager.
AssetManager &getAssetManager();

/// @brief Returns number of threads the CPU supports executing concurrently.
/// @returns Number of CPU threads.
uint32_t getNumberOfCpuThreads();
} // namespace OS

} // namespace Tobi
