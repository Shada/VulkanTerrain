#pragma once

namespace Tobi
{

/// @brief Describes the status of the application lifecycle.
enum Status
{
    /// The application is running.
    STATUS_RUNNING,
    /// The application should exit as the user has requested it.
    STATUS_TEARDOWN
};

} // namespace Tobi