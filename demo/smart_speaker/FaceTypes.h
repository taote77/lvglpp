#ifndef LVGLPP_FACE_TYPES_H
#define LVGLPP_FACE_TYPES_H

#include <cstdint>

namespace lvglpp::demo
{

/**
 * Voice-assistant state shown by the face. Mirrors the six states used by the
 * smart-speaker firmware, without depending on its voice component.
 */
enum class VoiceState : std::uint8_t
{
    Init = 0,
    Idle,
    Listening,
    Thinking,
    Speaking,
    Error,
};

}    // namespace lvglpp::demo

#endif    // LVGLPP_FACE_TYPES_H
