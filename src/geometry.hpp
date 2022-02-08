#ifndef SRC_GEOMETRY_HPP_
#define SRC_GEOMETRY_HPP_

#include <iostream>
#include <string>
#include <exception>
#include <limits>

#ifdef LIBNFP_USE_RATIONAL
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/number.hpp>
#endif
#include <boost/geometry.hpp>
#include <boost/geometry/util/math.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#ifdef LIBNFP_USE_RATIONAL
namespace bm = boost::multiprecision;
#endif
namespace bg = boost::geometry;
namespace trans = boost::geometry::strategy::transform;

namespace libnfporb {
#ifdef NFP_DEBUG
#define DEBUG_VAL(x) std::cerr << x << std::endl
#define DEBUG_MSG(title, value) std::cerr << title << ": " << value << std::endl
#else
#define DEBUG_VAL(x)
#define DEBUG_MSG(title, value)
#endif

using std::string;

static constexpr long double NFP_EPSILON = 0.00000001;

class LongDouble {
private:
	long double val_;
	public:
	LongDouble() :
			val_(0) {
	}

	LongDouble(const long double& val) :
			val_(val) {
	}

	void setVal(const long double& v) {
		val_ = v;
	}

	long double val() const {
		return val_;
	}

	LongDouble operator/(const LongDouble& other) const {
		return this->val_ / other.val_;
	}

	LongDouble operator*(const LongDouble& other) const {
		return this->val_ * other.val_;
	}

	LongDouble operator-(const LongDouble& other) const {
		return this->val_ - other.val_;
	}

	LongDouble operator-() const {
		return this->val_ * -1;
	}

	LongDouble operator+(const LongDouble& other) const {
		return this->val_ + other.val_;
	}

	void operator/=(const LongDouble& other) {
		this->val_ = this->val_ / other.val_;
	}

	void operator*=(const LongDouble& other) {
		this->val_ = this->val_ * other.val_;
	}

	void operator-=(const LongDouble& other) {
		this->val_ = this->val_ - other.val_;
	}

	void operator+=(const LongDouble& other) {
		this->val_ = this->val_ + other.val_;
	}

	bool operator==(const int& other) const {
		return this->operator ==(static_cast<long double>(other));
	}

	bool operator==(const LongDouble& other) const {
		return this->operator ==(other.val());
	}

	bool operator==(const long double& other) const {
		return this->val() == other;
	}

	bool operator!=(const int& other) const {
		return !this->operator ==(other);
	}

	bool operator!=(const LongDouble& other) const {
		return !this->operator ==(other);
	}

	bool operator!=(const long double& other) const {
		return !this->operator ==(other);
	}

	bool operator<(const int& other) const {
		return this->operator <(static_cast<long double>(other));
	}

	bool operator<(const LongDouble& other) const {
		return this->operator <(other.val());
	}

	bool operator<(const long double& other) const {
		return this->val() < other;
	}

	bool operator<(const double& other) const {
		return this->val() < other;
	}

	bool operator>(const int& other) const {
		return this->operator >(static_cast<long double>(other));
	}

	bool operator>(const LongDouble& other) const {
		return this->operator >(other.val());
	}

	bool operator>(const long double& other) const {
		return this->val() > other;
	}

	bool operator>=(const int& other) const {
		return this->operator >=(static_cast<long double>(other));
	}

	bool operator>=(const LongDouble& other) const {
		return this->operator >=(other.val());
	}

	bool operator>=(const long double& other) const {
		return this->val() >= other;
	}

	bool operator<=(const int& other) const {
		return this->operator <=(static_cast<long double>(other));
	}

	bool operator<=(const LongDouble& other) const {
		return this->operator <=(other.val());
	}

	bool operator<=(const long double& other) const {
		return this->val() <= other;
	}
};
}

namespace std {
template<>
struct numeric_limits<libnfporb::LongDouble>
{
	static constexpr bool is_specialized = true;

	static constexpr long double
	min() noexcept {
		return std::numeric_limits<long double>::min();
	}

	static constexpr long double
	max() noexcept {
		return std::numeric_limits<long double>::max();
	}

#if __cplusplus >= 201103L
	static constexpr long double
	lowest() noexcept {
		return -std::numeric_limits<long double>::lowest();
	}
#endif

	static constexpr int digits = std::numeric_limits<long double>::digits;
	static constexpr int digits10 = std::numeric_limits<long double>::digits10;
	#if __cplusplus >= 201103L
	static constexpr int max_digits10
	= std::numeric_limits<long double>::max_digits10;
	#endif
	static constexpr bool is_signed = true;
	static constexpr bool is_integer = false;
	static constexpr bool is_exact = false;
	static constexpr int radix = std::numeric_limits<long double>::radix;

	static constexpr long double
	epsilon() noexcept {
		return libnfporb::NFP_EPSILON;
	}

	static constexpr long double
	round_error() noexcept {
		return 0.5L;
	}

	static constexpr int min_exponent = std::numeric_limits<long double>::min_exponent;
	static constexpr int min_exponent10 = std::numeric_limits<long double>::min_exponent10;
	static constexpr int max_exponent = std::numeric_limits<long double>::max_exponent;
	static constexpr int max_exponent10 = std::numeric_limits<long double>::max_exponent10;

	static constexpr bool has_infinity = std::numeric_limits<long double>::has_infinity;
	static constexpr bool has_quiet_NaN = std::numeric_limits<long double>::has_quiet_NaN;
	static constexpr bool has_signaling_NaN = has_quiet_NaN;
	static constexpr float_denorm_style has_denorm
	= std::numeric_limits<long double>::has_denorm;
	static constexpr bool has_denorm_loss
	= std::numeric_limits<long double>::has_denorm_loss;

	static constexpr long double
	infinity() noexcept {
		return std::numeric_limits<long double>::infinity();
	}

	static constexpr long double
	quiet_NaN() noexcept {
		return std::numeric_limits<long double>::quiet_NaN();
	}

	static constexpr long double
	signaling_NaN() noexcept {
		return std::numeric_limits<long double>::signaling_NaN();
	}

	static constexpr long double
	denorm_min() noexcept {
		return std::numeric_limits<long double>::denorm_min();
	}

	static constexpr bool is_iec559
	= has_infinity && has_quiet_NaN && has_denorm == denorm_present;

	static constexpr bool is_bounded = true;
	static constexpr bool is_modulo = false;

	static constexpr bool traps = std::numeric_limits<long double>::traps;
	static constexpr bool tinyness_before =
			std::numeric_limits<long double>::tinyness_before;
	static constexpr float_round_style round_style =
			round_to_nearest;
};
}

namespace boost {
namespace numeric {
template<>
struct raw_converter<boost::numeric::conversion_traits<double, libnfporb::LongDouble>>
{
	typedef typename boost::numeric::conversion_traits<double, libnfporb::LongDouble>::result_type result_type;
	typedef typename boost::numeric::conversion_traits<double, libnfporb::LongDouble>::argument_type argument_type;

	static result_type low_level_convert(argument_type s) {
		return s.val();
	}
};
}
}

namespace libnfporb {

#ifndef LIBNFP_USE_RATIONAL
typedef LongDouble coord_t;
#else
typedef bm::number<bm::gmp_rational, bm::et_off> rational_t;
typedef rational_t coord_t;
#endif

bool equals(const LongDouble& lhs, const LongDouble& rhs);
#ifdef LIBNFP_USE_RATIONAL
bool equals(const rational_t& lhs, const rational_t& rhs);
#endif

const coord_t MAX_COORD = 999999999999999999;
const coord_t MIN_COORD = std::numeric_limits<coord_t>::min();

class point_t {
public:
	point_t() :
			x_(0), y_(0) {
	}
	point_t(coord_t x, coord_t y) :
			x_(x), y_(y) {
	}
	bool marked_ = false;
	coord_t x_;
	coord_t y_;

	point_t operator-(const point_t& other) const {
		point_t result = *this;
		bg::subtract_point(result, other);
		return result;
	}

	point_t operator+(const point_t& other) const {
		point_t result = *this;
		bg::add_point(result, other);
		return result;
	}

	bool operator<(const point_t& other) const {
		return boost::geometry::math::smaller(this->x_, other.x_) || (equals(this->x_, other.x_) && boost::geometry::math::smaller(this->y_, other.y_));
	}
};

inline long double toLongDouble(const LongDouble& c) {
	return c.val();
}

#ifdef LIBNFP_USE_RATIONAL
inline long double toLongDouble(const rational_t& c) {
	return bm::numerator(c).convert_to<long double>() / bm::denominator(c).convert_to<long double>();
}
std::ostream& operator<<(std::ostream& os, const rational_t& p) {
	os << toLongDouble(p);
	return os;
}
#endif

std::ostream& operator<<(std::ostream& os, const LongDouble& c) {
	os << toLongDouble(c);
	return os;
}

std::istream& operator>>(std::istream& is, LongDouble& c) {
	long double val;
	is >> val;
	c.setVal(val);
	return is;
}

std::ostream& operator<<(std::ostream& os, const point_t& p) {
	os << "{" << toLongDouble(p.x_) << "," << toLongDouble(p.y_) << "}";
	return os;
}


const point_t INVALID_POINT = { MAX_COORD, MAX_COORD };

typedef bg::model::segment<point_t> segment_t;
}

#ifdef LIBNFP_USE_RATIONAL
inline long double acos(const libnfporb::rational_t& r) {
	return acos(libnfporb::toLongDouble(r));
}
#endif

inline long double acos(const libnfporb::LongDouble& ld) {
	return acos(libnfporb::toLongDouble(ld));
}

#ifdef LIBNFP_USE_RATIONAL
inline long double sqrt(const libnfporb::rational_t& r) {
	return sqrt(libnfporb::toLongDouble(r));
}
#endif

inline long double sqrt(const libnfporb::LongDouble& ld) {
	return sqrt(libnfporb::toLongDouble(ld));
}

BOOST_GEOMETRY_REGISTER_POINT_2D(libnfporb::point_t, libnfporb::coord_t, cs::cartesian, x_, y_)

namespace boost {
namespace geometry {
namespace math {
namespace detail {

template<>
struct square_root<libnfporb::LongDouble>
{
	typedef libnfporb::LongDouble return_type;

	static inline libnfporb::LongDouble apply(libnfporb::LongDouble const& a) {
		return std::sqrt(a.val());
	}
};

#ifdef LIBNFP_USE_RATIONAL
template <>
struct square_root<libnfporb::rational_t>
{
	typedef libnfporb::rational_t return_type;

	static inline libnfporb::rational_t apply(libnfporb::rational_t const& a)
	{
		return std::sqrt(libnfporb::toLongDouble(a));
	}
};
#endif

template<>
struct abs<libnfporb::LongDouble>
{
	static libnfporb::LongDouble apply(libnfporb::LongDouble const& value) {
		libnfporb::LongDouble const zero = libnfporb::LongDouble();
		return value.val() < zero.val() ? -value.val() : value.val();
	}
};

template<>
struct equals<libnfporb::LongDouble, false>
{
	template<typename Policy>
	static inline bool apply(libnfporb::LongDouble const& lhs, libnfporb::LongDouble const& rhs, Policy const& policy) {
		if (lhs.val() == rhs.val())
			return true;

		return bg::math::detail::abs<libnfporb::LongDouble>::apply(lhs.val() - rhs.val()) <= policy.apply(lhs.val(), rhs.val()) * libnfporb::NFP_EPSILON;
	}
};

template<>
struct smaller<libnfporb::LongDouble>
{
	static inline bool apply(libnfporb::LongDouble const& lhs, libnfporb::LongDouble const& rhs) {
		if (lhs.val() == rhs.val() || bg::math::detail::abs<libnfporb::LongDouble>::apply(lhs.val() - rhs.val()) <= libnfporb::NFP_EPSILON * std::fmax(bg::math::detail::abs<libnfporb::LongDouble>::apply(lhs.val()).val(),
				bg::math::detail::abs<libnfporb::LongDouble>::apply(rhs.val()).val()))
			return false;

		return lhs < rhs;
	}
};
}
}
}
}

namespace libnfporb {
bool equals(const LongDouble& lhs, const LongDouble& rhs) {
	if (lhs.val() == rhs.val())
		return true;

	return bg::math::detail::abs<libnfporb::LongDouble>::apply(lhs.val() - rhs.val()) <= libnfporb::NFP_EPSILON * std::fmax(
			bg::math::detail::abs<libnfporb::LongDouble>::apply(lhs.val()).val(),
			bg::math::detail::abs<libnfporb::LongDouble>::apply(rhs.val()).val());
}

inline bool smaller(const LongDouble& lhs, const LongDouble& rhs) {
	return boost::geometry::math::detail::smaller<LongDouble>::apply(lhs, rhs);
}

inline bool larger(const LongDouble& lhs, const LongDouble& rhs) {
	return smaller(rhs, lhs);
}

#ifdef LIBNFP_USE_RATIONAL
inline bool smaller(const rational_t& lhs, const rational_t& rhs) {
	return lhs < rhs;
}

inline bool larger(const rational_t& lhs, const rational_t& rhs) {
	return smaller(rhs, lhs);
}

bool equals(const rational_t& lhs, const rational_t& rhs) {
	return lhs == rhs;
}
#endif

bool equals(const point_t& lhs, const point_t& rhs) {
	return equals(lhs.x_, rhs.x_) && equals(lhs.y_, rhs.y_);
}

bool equals(const segment_t& lhs, const segment_t& rhs) {
	return equals(lhs.first, rhs.first) && equals(lhs.second, rhs.second);
}
std::ostream& operator<<(std::ostream& os, const segment_t& seg) {
	os << "{" << seg.first << "," << seg.second << "}";
	return os;
}

bool operator<(const segment_t& lhs, const segment_t& rhs) {
	return lhs.first < rhs.first || (equals(lhs.first, rhs.first) && (lhs.second < rhs.second));
}

typedef bg::model::polygon<point_t, false, true> polygon_t;
typedef std::vector<polygon_t::ring_type> nfp_t;
typedef bg::model::linestring<point_t> linestring_t; //FIXME do we still need it?

typedef typename polygon_t::ring_type::size_type psize_t;

typedef bg::model::d2::point_xy<long double> pointf_t;
typedef bg::model::segment<pointf_t> segmentf_t;
typedef bg::model::polygon<pointf_t, false, true> polygonf_t;

point_t normalize(const point_t& vec) {
	point_t norm = vec;
	coord_t len = bg::length(segment_t { { 0, 0 }, vec });

	if (len == 0.0L)
		return {0,0};

	norm.x_ /= len;
	norm.y_ /= len;

	return norm;
}

point_t flip(const point_t& vec) {
	point_t flipped = vec;

	flipped.x_ *= -1;
	flipped.y_ *= -1;

	return flipped;
}

bool is_parallel(const segment_t& s1, const segment_t& s2) {
	coord_t dx1 = s1.first.x_ - s2.first.x_;
	coord_t dy1 = s1.first.y_ - s2.first.y_;
	coord_t dx2 = s1.second.x_ - s2.second.x_;
	coord_t dy2 = s1.second.y_ - s2.second.y_;

	return dx1 == dx2 && dy2 == dy1;
}

enum Alignment {
	LEFT,
	RIGHT,
	ON
};

Alignment get_alignment(const segment_t& seg, const point_t& pt) {
	coord_t res = ((seg.second.x_ - seg.first.x_) * (pt.y_ - seg.first.y_)
			- (seg.second.y_ - seg.first.y_) * (pt.x_ - seg.first.x_));

	if (equals(res, 0)) {
		return ON;
	} else if (larger(res, 0)) {
		return LEFT;
	} else {
		return RIGHT;
	}
}

long double get_inner_angle(const point_t& joint, const point_t& end1, const point_t& end2) {
	coord_t dx21 = end1.x_ - joint.x_;
	coord_t dx31 = end2.x_ - joint.x_;
	coord_t dy21 = end1.y_ - joint.y_;
	coord_t dy31 = end2.y_ - joint.y_;
	coord_t m12 = sqrt((dx21 * dx21 + dy21 * dy21));
	coord_t m13 = sqrt((dx31 * dx31 + dy31 * dy31));
	if (m12 == 0.0L || m13 == 0.0L)
		return 0;
	return acos((dx21 * dx31 + dy21 * dy31) / (m12 * m13));
}

std::vector<psize_t> find_minimum_x(const polygon_t& p) {
	std::vector<psize_t> result;
	coord_t min = MAX_COORD;
	auto& po = p.outer();
	for (psize_t i = 0; i < p.outer().size() - 1; ++i) {
		if (smaller(po[i].x_, min)) {
			result.clear();
			min = po[i].x_;
			result.push_back(i);
		} else if (equals(po[i].x_, min)) {
			result.push_back(i);
		}
	}
	return result;
}

std::vector<psize_t> find_maximum_x(const polygon_t& p) {
	std::vector<psize_t> result;
	coord_t max = MIN_COORD;
	auto& po = p.outer();
	for (psize_t i = 0; i < p.outer().size() - 1; ++i) {
		if (larger(po[i].x_, max)) {
			result.clear();
			max = po[i].x_;
			result.push_back(i);
		} else if (equals(po[i].x_, max)) {
			result.push_back(i);
		}
	}
	return result;
}

std::vector<psize_t> find_minimum_y(const polygon_t& p) {
	std::vector<psize_t> result;
	coord_t min = MAX_COORD;
	auto& po = p.outer();
	for (psize_t i = 0; i < p.outer().size() - 1; ++i) {
		if (smaller(po[i].y_, min)) {
			result.clear();
			min = po[i].y_;
			result.push_back(i);
		} else if (equals(po[i].y_, min)) {
			result.push_back(i);
		}
	}
	return result;
}

std::vector<psize_t> find_maximum_y(const polygon_t& p) {
	std::vector<psize_t> result;
	coord_t max = MIN_COORD;
	auto& po = p.outer();
	for (psize_t i = 0; i < p.outer().size() - 1; ++i) {
		if (larger(po[i].y_, max)) {
			result.clear();
			max = po[i].y_;
			result.push_back(i);
		} else if (equals(po[i].y_, max)) {
			result.push_back(i);
		}
	}
	return result;
}

psize_t find_point(const polygon_t::ring_type& ring, const point_t& pt) {
	for (psize_t i = 0; i < ring.size(); ++i) {
		if (equals(ring[i], pt))
			return i;
	}
	return std::numeric_limits<psize_t>::max();
}

/**
 * @brief Checks if a point exists in a NFP
 * @param pt The point to look for
 * @param nfp The NFP to search
 * @return true if the point was found
 */
bool in_nfp(const point_t& pt, const nfp_t& nfp) {
	for (const auto& r : nfp) {
		if (bg::touches(pt, r))
			return true;
	}

	return false;
}
}

#endif /* SRC_GEOMETRY_HPP_ */
