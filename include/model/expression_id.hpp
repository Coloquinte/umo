
#ifndef __UMO_EXPRESSION_ID_HPP__
#define __UMO_EXPRESSION_ID_HPP__

#include <functional>

namespace umoi {
class ExpressionId {
  public:
    // Construct from variable, "not" bit and "minus" bit
    ExpressionId(std::uint32_t var, bool isNot, bool isMinus) {
        v = var << 2;
        if (isNot)
            v |= 0x01;
        if (isMinus)
            v |= 0x02;
    }

    // Test "not" bit
    bool isNot() const { return v & 0x01; }
    ExpressionId getNot() const { return fromRaw(v ^ 0x01); }

    // Test "minus" bit
    bool isMinus() const { return v & 0x02; }
    ExpressionId getMinus() const { return fromRaw(v ^ 0x02); }

    // Index of the variable
    std::uint32_t var() const { return v >> 2; }

    // Raw integer representation
    std::uint32_t raw() const { return v; }

    // Construct from raw integer representation
    static ExpressionId fromRaw(std::uint32_t raw) { return ExpressionId(raw); }

    bool operator==(const ExpressionId &o) const { return v == o.v; }
    bool operator!=(const ExpressionId &o) const { return v != o.v; }
    bool operator<=(const ExpressionId &o) const { return v <= o.v; }
    bool operator>=(const ExpressionId &o) const { return v >= o.v; }
    bool operator<(const ExpressionId &o) const { return v < o.v; }
    bool operator>(const ExpressionId &o) const { return v > o.v; }

  private:
    explicit ExpressionId(std::uint32_t raw) { v = raw; }

  private:
    std::uint32_t v;
};
} // namespace umoi

namespace std {
template <> struct hash<umoi::ExpressionId> {
    std::size_t operator()(const umoi::ExpressionId &v) const {
        std::hash<std::uint32_t> hasher;
        return hasher(v.raw());
    };
};
} // namespace std

#endif
