
#ifndef __UMO_EXPRESSION_ID_HPP__
#define __UMO_EXPRESSION_ID_HPP__

#include <functional>

namespace umoi {
class ExpressionId {
  public:
    ExpressionId(std::uint32_t var, bool isNot, bool isOpp) {
        v = var << 2;
        if (isNot) v |= 0x01;
        if (isOpp) v |= 0x02;
    }
    bool isNot() const {
        return v & 0x01;
    }
    bool isOpp() const {
        return v & 0x02;
    }
    std::uint32_t ind() const {
        return v >> 2;
    }
    std::uint32_t raw() const {
        return v;
    }
    static ExpressionId fromRaw(std::uint32_t raw) {
        return ExpressionId(raw);
    }

    bool operator==(const ExpressionId &o) { return v == o.v; }
    bool operator!=(const ExpressionId &o) { return v != o.v; }
    bool operator<=(const ExpressionId &o) { return v <= o.v; }
    bool operator>=(const ExpressionId &o) { return v >= o.v; }
    bool operator<(const ExpressionId &o) { return v < o.v; }
    bool operator>(const ExpressionId &o) { return v > o.v; }

  private:
    explicit ExpressionId(std::uint32_t raw) {
        v = raw;
    }

  private:
    std::uint32_t v;
};
}

namespace std {
template<>
struct hash<umoi::ExpressionId> {
    std::size_t operator()(const umoi::ExpressionId &v) const {
        std::hash<std::uint32_t> hasher;
        return hasher(v.raw());
    };
};
}

#endif

