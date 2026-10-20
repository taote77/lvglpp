#include "ws_util.h"

#include <cstdint>
#include <vector>

namespace wsgw {

namespace {

inline uint32_t rotateLeft(uint32_t value, int bits)
{
    return (value << bits) | (value >> (32 - bits));
}

void sha1(const uint8_t *data, std::size_t size, uint8_t out[20])
{
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    std::vector<uint8_t> message(data, data + size);

    uint64_t bitLength = static_cast<uint64_t>(size) * 8;

    message.push_back(0x80);
    while (message.size() % 64 != 56)
    {
        message.push_back(0x00);
    }
    for (int i = 7; i >= 0; --i)
    {
        message.push_back(static_cast<uint8_t>((bitLength >> (i * 8)) & 0xFF));
    }

    for (std::size_t chunk = 0; chunk < message.size(); chunk += 64)
    {
        uint32_t w[80];

        for (int i = 0; i < 16; ++i)
        {
            const uint8_t *p = &message[chunk + i * 4];
            w[i] = (static_cast<uint32_t>(p[0]) << 24) |
                   (static_cast<uint32_t>(p[1]) << 16) |
                   (static_cast<uint32_t>(p[2]) << 8) |
                   static_cast<uint32_t>(p[3]);
        }
        for (int i = 16; i < 80; ++i)
        {
            w[i] = rotateLeft(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        for (int i = 0; i < 80; ++i)
        {
            uint32_t f;
            uint32_t k;

            if (i < 20)
            {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            }
            else if (i < 40)
            {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            }
            else if (i < 60)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            }
            else
            {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = rotateLeft(a, 5) + f + e + k + w[i];
            e             = d;
            d             = c;
            c             = rotateLeft(b, 30);
            b             = a;
            a             = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    uint32_t state[5] = {h0, h1, h2, h3, h4};
    for (int i = 0; i < 5; ++i)
    {
        out[i * 4 + 0] = static_cast<uint8_t>((state[i] >> 24) & 0xFF);
        out[i * 4 + 1] = static_cast<uint8_t>((state[i] >> 16) & 0xFF);
        out[i * 4 + 2] = static_cast<uint8_t>((state[i] >> 8) & 0xFF);
        out[i * 4 + 3] = static_cast<uint8_t>(state[i] & 0xFF);
    }
}

} // namespace

std::string base64Encode(const void *data, std::size_t size)
{
    static constexpr char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const auto *bytes = static_cast<const uint8_t *>(data);
    std::string out;
    out.reserve(((size + 2) / 3) * 4);

    for (std::size_t i = 0; i < size; i += 3)
    {
        uint32_t value = static_cast<uint32_t>(bytes[i]) << 16;
        bool     hasSecond = i + 1 < size;
        bool     hasThird  = i + 2 < size;

        if (hasSecond) value |= static_cast<uint32_t>(bytes[i + 1]) << 8;
        if (hasThird) value |= static_cast<uint32_t>(bytes[i + 2]);

        out += alphabet[(value >> 18) & 0x3F];
        out += alphabet[(value >> 12) & 0x3F];
        out += hasSecond ? alphabet[(value >> 6) & 0x3F] : '=';
        out += hasThird ? alphabet[value & 0x3F] : '=';
    }

    return out;
}

std::string webSocketAcceptKey(std::string_view clientKey)
{
    constexpr std::string_view kGuid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    std::string input(clientKey);
    input += kGuid;

    uint8_t digest[20];
    sha1(reinterpret_cast<const uint8_t *>(input.data()), input.size(), digest);

    return base64Encode(digest, sizeof(digest));
}

} // namespace wsgw
