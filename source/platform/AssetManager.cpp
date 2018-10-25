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

#include "AssetManager.hpp"
#include <stdio.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

namespace Tobi
{

AssetManager &OS::getAssetManager()
{
    static AssetManager manager;
    return manager;
}

AssetManager::AssetManager()
{
    pid_t pid = getpid();
    char buf[PATH_MAX];

    static const char *exts[] = {"exe", "file", "path/a.out"};
    for (auto ext : exts)
    {
        std::string linkPath = "/proc/";
        linkPath += std::to_string(pid);
        linkPath += '/';
        linkPath += ext;

        ssize_t ret = readlink(linkPath.c_str(), buf, sizeof(buf) - 1);
        if (ret >= 0)
        {
            buf[ret] = '\0';
            basePath = buf;

            auto pos = basePath.find_last_of('/');
            if (pos == std::string::npos)
                basePath = ".";
            else
                basePath = basePath.substr(0, pos);

            LOGI("Found application base directory: \"%s\".\n", basePath.c_str());
            return;
        }
    }

    LOGE("Could not find application path based on /proc/$pid interface. Will "
         "use working directory instead.\n");
    basePath = ".";
}

Result AssetManager::readBinaryFile(const char *pPath, void **pData, size_t *pSize)
{
    auto fullpath = basePath + "/assets/" + pPath;

    FILE *file = fopen(fullpath.c_str(), "rb");
    if (!file)
        return RESULT_ERROR_IO;

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);

    *pData = malloc(len);
    if (!*pData)
    {
        fclose(file);
        return RESULT_ERROR_OUT_OF_MEMORY;
    }

    *pSize = len;
    if (fread(*pData, 1, *pSize, file) != *pSize)
    {
        free(pData);
        fclose(file);
        return RESULT_ERROR_IO;
    }

    fclose(file);
    return RESULT_SUCCESS;
}

} // namespace Tobi
