#ifndef LVGLPP_PLATFORMFACTORY_H
#define LVGLPP_PLATFORMFACTORY_H

#include "Platform.h"

#include <memory>
#include <string>

namespace lvglpp::sys {

/**
 * Create the platform backend selected at build time.
 *
 * `kind` defaults to Auto (the compiled-in backend). Requesting a backend that
 * was not compiled in logs an error and returns nullptr.
 */
std::unique_ptr<Platform> createPlatform(int argc, char **argv, PlatformKind kind = PlatformKind::Auto);

const char *platformName(PlatformKind kind) noexcept;

/// argv[0] stem, used for <exe>.pak asset discovery.
std::string executableStem(int argc, char **argv);

} // namespace lvglpp::sys

#endif // LVGLPP_PLATFORMFACTORY_H
