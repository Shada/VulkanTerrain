#pragma once

namespace Tobi
{

/// @brief Describes the status of the application lifecycle.
enum TobiStatus
{
    /// The application is running.
    TOBI_STATUS_RUNNING,
    /// The application should exit as the user has requested it.
    TOBI_STATUS_TEARDOWN
};

} // namespace Tobi