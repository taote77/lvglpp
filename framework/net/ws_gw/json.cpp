#include "json.h"

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace wsgw {

namespace {
[[noreturn]] void typeError(const char *expected)
{
    throw std::runtime_error(std::string("wsgw::Json: value is not ") + expected);
}

constexpr int kMaxParseDepth = 128;
} // namespace

struct Json::Impl
{
    Type       type = Type::Null;
    bool       boolean = false;
    int64_t    integer = 0;
    double     number  = 0.0;
    std::string string;

    std::vector<Json>                          array;
    std::vector<std::pair<std::string, Json>> object;
};

// ---------------------------------------------------------------- constructors

Json::Json() : impl_(std::make_unique<Impl>())
{}

Json::Json(std::nullptr_t) : Json()
{}

Json::Json(bool value) : Json()
{
    impl_->type    = Type::Bool;
    impl_->boolean = value;
}

Json::Json(int64_t value) : Json()
{
    impl_->type    = Type::Int;
    impl_->integer = value;
}

Json::Json(double value) : Json()
{
    impl_->type   = Type::Double;
    impl_->number = value;
}

Json::Json(const char *value) : Json(std::string_view(value ? value : ""))
{}

Json::Json(std::string value) : Json()
{
    impl_->type   = Type::String;
    impl_->string = std::move(value);
}

Json::Json(std::string_view value) : Json()
{
    impl_->type = Type::String;
    impl_->string.assign(value.data(), value.size());
}

Json::Json(const Json &other)
    : impl_(other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr)
{}

Json::Json(Json &&other) noexcept = default;

Json &Json::operator=(const Json &other)
{
    if (this != &other)
    {
        impl_ = other.impl_ ? std::make_unique<Impl>(*other.impl_) : nullptr;
    }
    return *this;
}

Json &Json::operator=(Json &&other) noexcept = default;

Json::~Json() = default;

Json Json::makeObject()
{
    Json value;
    value.impl_->type = Type::Object;
    return value;
}

Json Json::makeArray()
{
    Json value;
    value.impl_->type = Type::Array;
    return value;
}

// ------------------------------------------------------------------ inspection

Json::Type Json::type() const
{
    return impl_ ? impl_->type : Type::Null;
}

bool Json::is_null() const
{
    return type() == Type::Null;
}

bool Json::is_bool() const
{
    return type() == Type::Bool;
}

bool Json::is_int() const
{
    return type() == Type::Int;
}

bool Json::is_double() const
{
    return type() == Type::Double;
}

bool Json::is_number() const
{
    Type t = type();
    return t == Type::Int || t == Type::Double;
}

bool Json::is_string() const
{
    return type() == Type::String;
}

bool Json::is_array() const
{
    return type() == Type::Array;
}

bool Json::is_object() const
{
    return type() == Type::Object;
}

// -------------------------------------------------------------------- accessors

bool Json::as_bool() const
{
    if (type() != Type::Bool) typeError("a bool");
    return impl_->boolean;
}

int64_t Json::as_int64() const
{
    if (type() == Type::Int) return impl_->integer;
    if (type() == Type::Double && std::isfinite(impl_->number) &&
        impl_->number >= static_cast<double>(std::numeric_limits<int64_t>::min()) &&
        impl_->number < -static_cast<double>(std::numeric_limits<int64_t>::min()))
    {
        return static_cast<int64_t>(impl_->number);
    }
    typeError("an integer");
}

double Json::as_double() const
{
    if (type() == Type::Int) return static_cast<double>(impl_->integer);
    if (type() == Type::Double) return impl_->number;
    typeError("a number");
}

const std::string &Json::as_string() const
{
    if (type() != Type::String) typeError("a string");
    return impl_->string;
}

std::string &Json::as_string()
{
    if (type() != Type::String) typeError("a string");
    return impl_->string;
}

Json &Json::as_object()
{
    if (type() != Type::Object) typeError("an object");
    return *this;
}

const Json &Json::as_object() const
{
    if (type() != Type::Object) typeError("an object");
    return *this;
}

Json &Json::as_array()
{
    if (type() != Type::Array) typeError("an array");
    return *this;
}

const Json &Json::as_array() const
{
    if (type() != Type::Array) typeError("an array");
    return *this;
}

Json &Json::operator[](const std::string &key)
{
    if (type() == Type::Null) impl_->type = Type::Object;
    if (type() != Type::Object) typeError("an object");

    for (auto &entry : impl_->object)
    {
        if (entry.first == key) return entry.second;
    }

    impl_->object.emplace_back(key, Json());
    return impl_->object.back().second;
}

Json &Json::operator[](const char *key)
{
    return (*this)[std::string(key ? key : "")];
}

const Json &Json::operator[](const std::string &key) const
{
    if (type() != Type::Object) typeError("an object");

    for (const auto &entry : impl_->object)
    {
        if (entry.first == key) return entry.second;
    }

    static const Json nullValue;
    return nullValue;
}

const Json &Json::operator[](const char *key) const
{
    return (*this)[std::string(key ? key : "")];
}

bool Json::contains(const std::string &key) const
{
    if (type() != Type::Object) return false;

    for (const auto &entry : impl_->object)
    {
        if (entry.first == key) return true;
    }
    return false;
}

bool Json::empty() const
{
    switch (type())
    {
    case Type::Null: return true;
    case Type::String: return impl_->string.empty();
    case Type::Array: return impl_->array.empty();
    case Type::Object: return impl_->object.empty();
    default: return false;
    }
}

std::size_t Json::size() const
{
    switch (type())
    {
    case Type::String: return impl_->string.size();
    case Type::Array: return impl_->array.size();
    case Type::Object: return impl_->object.size();
    default: return 0;
    }
}

Json &Json::push_back(Json value)
{
    if (type() == Type::Null) impl_->type = Type::Array;
    if (type() != Type::Array) typeError("an array");

    impl_->array.push_back(std::move(value));
    return *this;
}

// ----------------------------------------------------------------- serialization

void Json::escapeString(std::string_view value, std::string &out)
{
    out += '"';
    for (unsigned char c : value)
    {
        switch (c)
        {
        case '"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (c < 0x20)
            {
                char buffer[8];
                std::snprintf(buffer, sizeof(buffer), "\\u%04x", static_cast<unsigned>(c));
                out += buffer;
            }
            else
            {
                out += static_cast<char>(c);
            }
        }
    }
    out += '"';
}

void Json::serializeInto(const Json &value, std::string &out)
{
    switch (value.type())
    {
    case Type::Null: out += "null"; break;
    case Type::Bool: out += value.impl_->boolean ? "true" : "false"; break;
    case Type::Int: out += std::to_string(value.impl_->integer); break;
    case Type::Double:
    {
        double number = value.impl_->number;
        if (!std::isfinite(number))
        {
            out += "null";
            break;
        }
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        oss << std::setprecision(17) << number;
        out += oss.str();
        break;
    }
    case Type::String: escapeString(value.impl_->string, out); break;
    case Type::Array:
    {
        out += '[';
        bool first = true;
        for (const auto &element : value.impl_->array)
        {
            if (!first) out += ',';
            first = false;
            serializeInto(element, out);
        }
        out += ']';
        break;
    }
    case Type::Object:
    {
        out += '{';
        bool first = true;
        for (const auto &entry : value.impl_->object)
        {
            if (!first) out += ',';
            first = false;
            escapeString(entry.first, out);
            out += ':';
            serializeInto(entry.second, out);
        }
        out += '}';
        break;
    }
    }
}

std::string Json::serialize() const
{
    std::string out;
    serializeInto(*this, out);
    return out;
}

std::ostream &operator<<(std::ostream &os, const Json &value)
{
    return os << value.serialize();
}

// ----------------------------------------------------------------------- parser

namespace {

class Parser
{
public:
    explicit Parser(std::string_view text) : text_(text) {}

    Json parse()
    {
        skipWhitespace();
        Json value = parseValue(0);
        skipWhitespace();
        if (position_ != text_.size()) fail("unexpected trailing characters");
        return value;
    }

private:
    [[noreturn]] void fail(const char *message) const
    {
        throw std::runtime_error("wsgw::Json parse error at byte " +
                                 std::to_string(position_) + ": " + message);
    }

    bool atEnd() const { return position_ >= text_.size(); }

    char peek() const { return text_[position_]; }

    void skipWhitespace()
    {
        while (!atEnd())
        {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                ++position_;
            }
            else
            {
                break;
            }
        }
    }

    void expectLiteral(const char *literal)
    {
        std::size_t length = std::strlen(literal);
        if (text_.substr(position_, length) != literal) fail("invalid literal");
        position_ += length;
    }

    Json parseValue(int depth)
    {
        if (depth > kMaxParseDepth) fail("nesting too deep");
        skipWhitespace();
        if (atEnd()) fail("unexpected end of input");

        switch (peek())
        {
        case '{': return parseObject(depth);
        case '[': return parseArray(depth);
        case '"': return Json(parseString());
        case 't': expectLiteral("true"); return Json(true);
        case 'f': expectLiteral("false"); return Json(false);
        case 'n': expectLiteral("null"); return Json();
        default: return parseNumber();
        }
    }

    Json parseObject(int depth)
    {
        ++position_; // '{'
        Json object = Json::makeObject();
        skipWhitespace();
        if (!atEnd() && peek() == '}')
        {
            ++position_;
            return object;
        }

        for (;;)
        {
            skipWhitespace();
            if (atEnd() || peek() != '"') fail("expected an object key");
            std::string key = parseString();
            skipWhitespace();
            if (atEnd() || peek() != ':') fail("expected ':' after object key");
            ++position_;

            object[key] = parseValue(depth + 1);

            skipWhitespace();
            if (atEnd()) fail("unterminated object");
            if (peek() == ',')
            {
                ++position_;
                continue;
            }
            if (peek() == '}')
            {
                ++position_;
                return object;
            }
            fail("expected ',' or '}' in object");
        }
    }

    Json parseArray(int depth)
    {
        ++position_; // '['
        Json array = Json::makeArray();
        skipWhitespace();
        if (!atEnd() && peek() == ']')
        {
            ++position_;
            return array;
        }

        for (;;)
        {
            array.push_back(parseValue(depth + 1));
            skipWhitespace();
            if (atEnd()) fail("unterminated array");
            if (peek() == ',')
            {
                ++position_;
                continue;
            }
            if (peek() == ']')
            {
                ++position_;
                return array;
            }
            fail("expected ',' or ']' in array");
        }
    }

    static int hexValue(char c)
    {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    }

    uint32_t parseHex4()
    {
        if (position_ + 4 > text_.size()) fail("truncated \\u escape");

        uint32_t value = 0;
        for (int i = 0; i < 4; ++i)
        {
            int digit = hexValue(text_[position_++]);
            if (digit < 0) fail("invalid \\u escape");
            value = (value << 4) | static_cast<uint32_t>(digit);
        }
        return value;
    }

    static void appendUtf8(std::string &out, uint32_t codepoint)
    {
        if (codepoint <= 0x7F)
        {
            out += static_cast<char>(codepoint);
        }
        else if (codepoint <= 0x7FF)
        {
            out += static_cast<char>(0xC0 | (codepoint >> 6));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint <= 0xFFFF)
        {
            out += static_cast<char>(0xE0 | (codepoint >> 12));
            out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else
        {
            out += static_cast<char>(0xF0 | (codepoint >> 18));
            out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
    }

    std::string parseString()
    {
        ++position_; // opening quote
        std::string out;

        for (;;)
        {
            if (atEnd()) fail("unterminated string");
            unsigned char c = static_cast<unsigned char>(text_[position_++]);

            if (c == '"') return out;
            if (c < 0x20) fail("unescaped control character in string");
            if (c != '\\')
            {
                out += static_cast<char>(c);
                continue;
            }

            if (atEnd()) fail("truncated escape sequence");
            char escape = text_[position_++];
            switch (escape)
            {
            case '"': out += '"'; break;
            case '\\': out += '\\'; break;
            case '/': out += '/'; break;
            case 'b': out += '\b'; break;
            case 'f': out += '\f'; break;
            case 'n': out += '\n'; break;
            case 'r': out += '\r'; break;
            case 't': out += '\t'; break;
            case 'u':
            {
                uint32_t codepoint = parseHex4();
                if (codepoint >= 0xD800 && codepoint <= 0xDBFF &&
                    position_ + 1 < text_.size() && text_[position_] == '\\' &&
                    text_[position_ + 1] == 'u')
                {
                    std::size_t saved = position_;
                    position_ += 2;
                    uint32_t low = parseHex4();
                    if (low >= 0xDC00 && low <= 0xDFFF)
                    {
                        codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                    }
                    else
                    {
                        position_ = saved;
                        codepoint = 0xFFFD;
                    }
                }
                else if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
                {
                    codepoint = 0xFFFD;
                }
                appendUtf8(out, codepoint);
                break;
            }
            default: fail("invalid escape sequence");
            }
        }
    }

    double parseDouble(const std::string &token)
    {
        // Use a classic-locale stream so the result does not depend on the
        // process locale's decimal separator.
        std::istringstream stream(token);
        stream.imbue(std::locale::classic());

        double value = 0.0;
        stream >> value;
        if (stream.fail() || !stream.eof()) fail("number out of range");
        return value;
    }

    Json parseNumber()
    {
        std::size_t start = position_;

        if (!atEnd() && peek() == '-') ++position_;
        if (atEnd() || peek() < '0' || peek() > '9') fail("invalid number");

        if (peek() == '0')
        {
            ++position_;
            if (!atEnd() && peek() >= '0' && peek() <= '9') fail("leading zero in number");
        }
        else
        {
            while (!atEnd() && peek() >= '0' && peek() <= '9') ++position_;
        }

        bool isFloat = false;
        if (!atEnd() && peek() == '.')
        {
            isFloat = true;
            ++position_;
            if (atEnd() || peek() < '0' || peek() > '9') fail("invalid fraction");
            while (!atEnd() && peek() >= '0' && peek() <= '9') ++position_;
        }

        if (!atEnd() && (peek() == 'e' || peek() == 'E'))
        {
            isFloat = true;
            ++position_;
            if (!atEnd() && (peek() == '+' || peek() == '-')) ++position_;
            if (atEnd() || peek() < '0' || peek() > '9') fail("invalid exponent");
            while (!atEnd() && peek() >= '0' && peek() <= '9') ++position_;
        }

        std::string token(text_.substr(start, position_ - start));

        if (isFloat)
        {
            return Json(parseDouble(token));
        }

        errno           = 0;
        char *end       = nullptr;
        long long value = std::strtoll(token.c_str(), &end, 10);
        if (end != token.c_str() + token.size()) fail("invalid integer");
        if (errno == ERANGE)
        {
            // Fall back to double for values outside the int64 range.
            return Json(parseDouble(token));
        }
        return Json(static_cast<int64_t>(value));
    }

    std::string_view text_;
    std::size_t      position_ = 0;
};

} // namespace

Json Json::parse(std::string_view text)
{
    return Parser(text).parse();
}

} // namespace wsgw
