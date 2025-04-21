#include "Version.h"
#include <sstream>

namespace F2SE::Plugin {

Version::Version(uint32_t major, uint32_t minor, uint32_t patch)
    : _major(major), _minor(minor), _patch(patch) {
}

Version::Version(const std::string& version) {
    std::istringstream ss(version);
    std::string token;

    // Parse major version
    if (std::getline(ss, token, '.')) {
        _major = std::stoul(token);
    } else {
        _major = 0;
    }

    // Parse minor version
    if (std::getline(ss, token, '.')) {
        _minor = std::stoul(token);
    } else {
        _minor = 0;
    }

    // Parse patch version
    if (std::getline(ss, token)) {
        _patch = std::stoul(token);
    } else {
        _patch = 0;
    }
}

bool Version::operator==(const Version& other) const {
    return _major == other._major && _minor == other._minor && _patch == other._patch;
}

bool Version::operator!=(const Version& other) const {
    return !(*this == other);
}

bool Version::operator<(const Version& other) const {
    if (_major != other._major) {
        return _major < other._major;
    }
    if (_minor != other._minor) {
        return _minor < other._minor;
    }
    return _patch < other._patch;
}

bool Version::operator<=(const Version& other) const {
    return *this < other || *this == other;
}

bool Version::operator>(const Version& other) const {
    return !(*this <= other);
}

bool Version::operator>=(const Version& other) const {
    return !(*this < other);
}

std::string Version::ToString() const {
    std::ostringstream ss;
    ss << _major << '.' << _minor << '.' << _patch;
    return ss.str();
}

uint32_t Version::GetMajor() const {
    return _major;
}

uint32_t Version::GetMinor() const {
    return _minor;
}

uint32_t Version::GetPatch() const {
    return _patch;
}

} // namespace F2SE::Plugin 