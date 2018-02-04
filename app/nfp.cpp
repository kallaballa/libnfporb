#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <set>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/geometries/register/point.hpp>

namespace bg = boost::geometry;
namespace trans = boost::geometry::strategy::transform;

using std::string;

typedef double coord_t;
const coord_t MAX_COORD = std::numeric_limits<coord_t>::max();
const coord_t MIN_COORD = std::numeric_limits<coord_t>::min();

class point_t {
public:
	point_t() : x_(0), y_(0) {
	}
	point_t(coord_t x, coord_t y) : x_(x), y_(y) {
	}
	bool marked_ = false;
	coord_t x_;
	coord_t y_;

	point_t operator-(const point_t& other) const {
		return {this->x_ - other.x_, this->y_ - other.y_};
	}

  bool operator==(const point_t&  other) const {
      return  (this->x_ == other.x_) && (this->y_ == other.y_);
  }

  bool operator!=(const point_t&  other) const {
      return !this->operator ==(other);
  }

	bool operator<(const point_t&  other) const {
      return  (this->x_ < other.x_) || ((this->x_ == other.x_) && (this->y_ < other.y_));
  }
};

std::ostream& operator<<(std::ostream& os, const point_t& p) {
	os << "{" << p.x_ << "," << p.y_ << "}";
	return os;
}
const point_t INVALID_POINT = {MAX_COORD, MAX_COORD};
BOOST_GEOMETRY_REGISTER_POINT_2D(point_t, coord_t, cs::cartesian, x_, y_)

typedef bg::model::segment<point_t> segment_t;
typedef bg::model::polygon<point_t, false, true> polygon_t;
typedef bg::model::linestring<point_t> linestring_t;

typedef typename polygon_t::ring_type::size_type psize_t;

std::ostream& operator<<(std::ostream& os, const segment_t& seg) {
	os << "{" << seg.first << "," << seg.second << "}";
	return os;
}

bool operator<(const segment_t& lhs, const segment_t& rhs) {
	return lhs.first < rhs.first || ((lhs.first == rhs.first) && (lhs.second < rhs.second));
}

bool operator==(const segment_t& lhs, const segment_t& rhs) {
	return (lhs.first == rhs.first && lhs.second == rhs.second) || (lhs.first == rhs.second && lhs.second == rhs.first);
}

enum Alignment {
	LEFT,
	RIGHT,
	ON
};

point_t normalize(const point_t& pt) {
	point_t norm = pt;
	coord_t len = bg::length(segment_t{{0,0},pt});
	norm.x_ /= len;
	norm.y_ /= len;
	return norm;
}

Alignment get_aligment(const segment_t& seg, const point_t& pt){
	coord_t res = ((seg.second.x_ - seg.first.x_)*(pt.y_ - seg.first.y_)
			- (seg.second.y_ - seg.first.y_)*(pt.x_ - seg.first.x_));

	if(res > 0) {
		return LEFT;
	} else if (res < 0) {
		return RIGHT;
	} else {
		return ON;
	}
}

coord_t get_inner_angle(const point_t& joint, const point_t& end1, const point_t& end2) {
	coord_t dx21 = end1.x_-joint.x_;
	coord_t dx31 = end2.x_-joint.x_;
	coord_t dy21 = end1.y_-joint.y_;
	coord_t dy31 = end2.y_-joint.y_;
	coord_t m12 = sqrt( dx21*dx21 + dy21*dy21 );
	coord_t m13 = sqrt( dx31*dx31 + dy31*dy31 );
	return acos( (dx21*dx31 + dy21*dy31) / (m12 * m13) );
}

coord_t get_slope(const segment_t& seg, bool invert = false) {
  int d_x = seg.second.x_ - seg.first.x_;
  int d_y = seg.second.y_ - seg.first.y_;

  if(invert) {
    d_x = -d_x;
    d_y = -d_y;
  }

  return atan2(d_x, -d_y);
}

struct TouchingPoint {
	enum Type {
		VERTEX,
		A_ON_B,
		B_ON_A
	};
	Type type_;
	psize_t A_;
	psize_t B_;
};

struct TranslationVector {
	point_t vector_;
	segment_t edge_;
	bool fromA_;

	bool operator<(const TranslationVector& other) const {
		return this->vector_ < other.vector_ || ((this->vector_ == other.vector_) && (this->edge_ < other.edge_));
	}
};

std::ostream& operator<<(std::ostream& os, const TranslationVector& tv) {
	os << "{" << tv.edge_ << " -> " << tv.vector_ << "}";
	return os;
}

template <typename Geometry>
void write_svg(std::string const& filename,typename std::vector<Geometry> const& geometries) {
    typedef typename boost::geometry::point_type<Geometry>::type point_type;
    std::ofstream svg(filename.c_str());

    boost::geometry::svg_mapper<point_type> mapper(svg, 100, 100, "width=\"200mm\" height=\"200mm\" viewBox=\"-250 -250 500 500\"");
    for(auto g : geometries) {
    	mapper.add(g);
    	mapper.map(g, "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
    }
}

void read_polygon(const string& filename, polygon_t& p) {
	std::ifstream t(filename);

	std::string str;
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
							std::istreambuf_iterator<char>());

	str.pop_back();
	bg::read_wkt(str, p);
	bg::correct(p);
}

std::vector<psize_t> find_minimum_y(const polygon_t& p) {
	std::vector<psize_t> result;
	coord_t min = MAX_COORD;
	auto& po = p.outer();
	for(psize_t i = 0; i < p.outer().size(); ++i) {
		if(po[i].y_ < min) {
			result.clear();
			min = po[i].y_;
			result.push_back(i);
		} else if (po[i].y_ == min) {
			result.push_back(i);
		}
	}
	return result;
}

std::vector<psize_t> find_maximum_y(const polygon_t& p) {
	std::vector<psize_t> result;
	coord_t max = MIN_COORD;
	auto& po = p.outer();
	for(psize_t i = 0; i < p.outer().size(); ++i) {
		if(po[i].y_ > max) {
			result.clear();
			max = po[i].y_;
			result.push_back(i);
		} else if (po[i].y_ == max) {
			result.push_back(i);
		}
	}
	return result;
}

psize_t find_point(const polygon_t::ring_type& ring, const point_t& pt) {
	for(psize_t i = 0; i < ring.size(); ++i) {
		if(ring[i] == pt)
			return i;
	}
	return std::numeric_limits<psize_t>::max();
}

std::vector<TouchingPoint> findTouchingPoints(const polygon_t::ring_type& ringA, const polygon_t::ring_type& ringB) {
	std::vector<TouchingPoint> touchers;
	for(psize_t i = 0; i < ringA.size() - 1; i++) {
		psize_t nextI = i+1;
		for(psize_t j = 0; j < ringB.size() - 1; j++) {
			psize_t nextJ = j+1;
			if(bg::intersects(ringA[i], ringB[j])) {
				touchers.push_back({TouchingPoint::VERTEX, i, j});
			} else if (!bg::intersects(ringA[nextI], ringB[j]) && bg::intersects(segment_t(ringA[i],ringA[nextI]), ringB[j])) {
				touchers.push_back({TouchingPoint::B_ON_A, nextI, j});
			} else if (!bg::intersects(ringB[nextJ], ringA[i]) && bg::intersects(segment_t(ringB[j],ringB[nextJ]), ringA[i])) {
				touchers.push_back({TouchingPoint::A_ON_B, i, nextJ});
			}
		}
	}
	return touchers;
}

std::set<TranslationVector> findFeasibleTranslationVectors(polygon_t::ring_type& ringA, polygon_t::ring_type& ringB, const std::vector<TouchingPoint>& touchers) {
	//use a set to automatically filter duplicate vectors
	std::set<TranslationVector> potentialVectors;
	std::vector<std::pair<segment_t,segment_t>> touchEdges;

	for (psize_t i = 0; i < touchers.size(); i++) {
		point_t& vertexA = ringA[touchers[i].A_];

		// adjacent A vertices
		signed long prevAindex = touchers[i].A_ - 1;
		signed long nextAindex = touchers[i].A_ + 1;

		prevAindex = (prevAindex < 0) ? ringA.size() - 2 : prevAindex; // loop
		nextAindex = (static_cast<psize_t>(nextAindex) >= ringA.size()) ? 1 : nextAindex; // loop

		point_t& prevA = ringA[prevAindex];
		point_t& nextA = ringA[nextAindex];

		// adjacent B vertices
		point_t& vertexB = ringB[touchers[i].B_];

		signed long prevBindex = touchers[i].B_ - 1;
		signed long nextBindex = touchers[i].B_ + 1;

		prevBindex = (prevBindex < 0) ? ringB.size() - 2 : prevBindex; // loop
		nextBindex = (static_cast<psize_t>(nextBindex) >= ringB.size()) ? 1 : nextBindex; // loop

		point_t& prevB = ringB[prevBindex];
		point_t& nextB = ringB[nextBindex];

		if (touchers[i].type_ == TouchingPoint::VERTEX) {
			segment_t a1 = { vertexA, nextA };
			segment_t a2 = { prevA, vertexA };
			segment_t b1 = { vertexB, nextB };
			segment_t b2 = { prevB, vertexB };

			//swap the segment elements so that always the first point is the touching point
			//also make the second segment always a segment of ringB
			touchEdges.push_back({a1, b1});
			touchEdges.push_back({a1, {b2.second, b2.first}});
			touchEdges.push_back({{a2.second, a2.first}, b1});
			touchEdges.push_back({{a2.second, a2.first}, {b2.second, b2.first}});

			write_svg<segment_t>("touchers" + std::to_string(i) + ".svg", {a1,a2,b1,b2});

			//TODO test parallel edges for floating point stability
			Alignment al;
			//a1 and b1 meet at start vertex
			al = get_aligment(a1, b1.second);
			if(al == LEFT) {
				potentialVectors.insert({b1.first - b1.second, b1, false});
			} else if(al == RIGHT) {
				potentialVectors.insert({a1.second - a1.first, a1, true});
			} else {
				potentialVectors.insert({a1.second - a1.first, a1, true});
			}

			//a1 and b2 meet at start and end
			al = get_aligment(a1, b2.first);
			if(al == LEFT) {
				//no feasible translation
			} else if(al == RIGHT) {
				potentialVectors.insert({a1.second - a1.first, a1, true});
			} else {
				potentialVectors.insert({a1.second - a1.first, a1, true});
			}

			//a2 and b1 meet at end and start
			al = get_aligment(a2, b1.second);
			if(al == LEFT) {
				//no feasible translation
			} else if(al == RIGHT) {
				potentialVectors.insert({b1.first - b1.second, b1});
			} else {
				potentialVectors.insert({a2.second - a2.first, a2});
			}
		} else if (touchers[i].type_ == TouchingPoint::B_ON_A) {
			touchEdges.push_back({{vertexB, vertexA}, {vertexB, prevB}});
			touchEdges.push_back({{vertexB, vertexA}, {vertexB, nextB}});
			touchEdges.push_back({{vertexB, prevA}, {vertexB, prevB}});
			touchEdges.push_back({{vertexB, nextA}, {vertexB, nextB}});
			potentialVectors.insert({{ vertexA.x_ - vertexB.x_, vertexA.y_ - vertexB.y_ }, {vertexB, vertexA}, true});
		} else if (touchers[i].type_ == TouchingPoint::A_ON_B) {
			//TODO testme
			touchEdges.push_back({{vertexA, prevA}, {vertexA, vertexB}});
			touchEdges.push_back({{vertexA, nextA}, {vertexA, vertexB}});
			touchEdges.push_back({{vertexA, prevA}, {vertexA, prevB}});
			touchEdges.push_back({{vertexA, nextA}, {vertexA, prevB}});

			potentialVectors.insert({{  vertexA.x_ - vertexB.x_, vertexA.y_ - vertexB.y_}, {vertexA, vertexB}, false});
		}
	}

	//discard immediately intersecting translations
	std::set<TranslationVector> vectors;
	for(const auto& v : potentialVectors) {
		bool discarded = false;
		for(const auto& sp : touchEdges) {
			const point_t& transStart = sp.first.first;
			point_t transEnd;
			trans::translate_transformer<coord_t, 2, 2> translate(v.vector_.x_, v.vector_.y_);
			boost::geometry::transform(transStart, transEnd, translate);
			segment_t transSeg(transStart, transEnd);
			Alignment a1 = get_aligment(transSeg, sp.first.second);
			Alignment a2 = get_aligment(transSeg, sp.second.second);
			if(a1 == a2 && a1 != ON) {
				// both segments are either left or right of the translation segment
				coord_t df = get_inner_angle(transStart, transEnd, sp.first.second);
				if(std::isnan(df)) {
					df = 0;
				}
				coord_t ds = get_inner_angle(transStart, transEnd, sp.second.second);
				if(std::isnan(ds)) {
					ds = 0;
				}
				point_t normEdge = normalize(v.edge_.second - v.edge_.first);
				point_t normTrans = normalize(transEnd - transStart);

				if(normEdge == normTrans) {
					if(ds > df) {
						discarded = true;
						break;
					}
				} else {
					if(df <= ds) {
						discarded = true;
						break;
					}
				}
			}
		}
		if(!discarded)
			vectors.insert(v);
	}
	return vectors;
}

TranslationVector findNextTranslationVector(const polygon_t::ring_type& rA,	const polygon_t::ring_type& rB, const std::set<TranslationVector>& tvs, const TranslationVector& last) {
	if(last.vector_ != INVALID_POINT) {
		point_t later;
		if(last.vector_ == (last.edge_.second - last.edge_.first)) {
			later = last.edge_.second;
		} else {
			later = last.edge_.first;
		}

		psize_t laterI = find_point(rA, later);
		assert(laterI != std::numeric_limits<psize_t>::max());
		point_t previous = later;
		point_t next;
		std::vector<segment_t> viableEdges;
		for(psize_t i = laterI + 1; i < rA.size() + laterI; ++i) {
			if(i >= rA.size())
				next = rA[i % rA.size() + 1];
			else
				next = rA[i];

			viableEdges.push_back({previous, next});
			previous = next;
		}

		for(const auto& ve: viableEdges) {
			for(auto& tv : tvs) {
				if((tv.fromA_ && (tv.vector_ == (ve.second - ve.first) || tv.vector_ == (ve.first - ve.second)))) {
					polygon_t::ring_type translated;
					trans::translate_transformer<coord_t, 2, 2> translate(tv.vector_.x_, tv.vector_.y_);
					boost::geometry::transform(rB, translated, translate);
					std::vector<point_t> intersectRes;
					bg::intersection(rA,translated, intersectRes);
					if(!intersectRes.empty() && (!bg::covered_by(translated, rA) && !bg::covered_by(rA, translated)))  {
						return tv;
					}
				} else if(!tv.fromA_) {
					point_t later;
					if(tv.vector_ == (tv.edge_.second - tv.edge_.first)) {
						later = last.edge_.second;
					} else {
						later = last.edge_.first;
					}

					if(later == ve.first || later == ve.second) {
						polygon_t::ring_type translated;
						trans::translate_transformer<coord_t, 2, 2> translate(tv.vector_.x_, tv.vector_.y_);
						boost::geometry::transform(rB, translated, translate);
						std::vector<point_t> intersectRes;
						bg::intersection(rA,translated, intersectRes);
						if(!intersectRes.empty() && (!bg::covered_by(translated, rA) && !bg::covered_by(rA, translated)))  {
							return tv;
						}
					}
				}
			}
		}
		assert(false);
		return TranslationVector();
	} else {
		std::vector<TranslationVector> notDisconnectingTranslation;
		for (auto& tv : tvs) {
			polygon_t::ring_type translated;
			trans::translate_transformer<coord_t, 2, 2> translate(tv.vector_.x_, tv.vector_.y_);
			boost::geometry::transform(rB, translated, translate);
			std::vector<point_t> intersectRes;
			bg::intersection(rA,translated, intersectRes);
			if(!intersectRes.empty() && (!bg::covered_by(translated, rA) && !bg::covered_by(rA, translated)))  {
					notDisconnectingTranslation.push_back(tv);
			}
		}

		assert(!notDisconnectingTranslation.empty());

		coord_t len;
		coord_t maxLen = MIN_COORD;
		TranslationVector longest;
		for(auto& tv : notDisconnectingTranslation) {
			len = bg::length(segment_t{{0,0},tv.vector_});
			if(len > maxLen) {
				maxLen = len;
				longest = tv;
			}
		}

		return longest;
	}
}

//TODO deduplicate code
TranslationVector trimVector(const polygon_t::ring_type& rA, const polygon_t::ring_type& rB, const TranslationVector& tv) {
	coord_t shortest = bg::length(tv.edge_);
	TranslationVector trimmed = tv;
	for(const auto& ptA : rA) {
		point_t translated;
		//for polygon A we invert the translation
		trans::translate_transformer<coord_t, 2, 2> translate(-tv.vector_.x_, -tv.vector_.y_);
		boost::geometry::transform(ptA, translated, translate);
		linestring_t projection;
		segment_t segproj(ptA, translated);
		projection.push_back(ptA);
		projection.push_back(translated);
		std::vector<point_t> intersections;
		bg::intersection(rB, projection, intersections);

		//find shortest intersection
		coord_t len;
		segment_t segi;
		for(const auto& pti : intersections) {
			//bg::intersection is inclusive so we have to exclude exact point intersections
			bool cont = false;
			for(const auto& ptB : rB) {
				if(pti == ptB) {
					cont = true;
					break;
				}
			}
			if(cont)
				continue;
			segi = segment_t(ptA,pti);
			len = bg::length(segi);
			if(len < shortest) {
				trimmed.vector_ = ptA - pti;
				trimmed.edge_ = segi;
				shortest = len;
			}
		}
	}

	for(const auto& ptB : rB) {
		point_t translated;

		trans::translate_transformer<coord_t, 2, 2> translate(tv.vector_.x_, tv.vector_.y_);
		boost::geometry::transform(ptB, translated, translate);
		linestring_t projection;
		segment_t segproj(ptB, translated);
		projection.push_back(ptB);
		projection.push_back(translated);
		std::vector<point_t> intersections;
		bg::intersection(rA, projection, intersections);

		//find shortest intersection
		coord_t len;
		segment_t segi;
		for(const auto& pti : intersections) {
			//bg::intersection is inclusive so we have to exclude exact point intersections
			bool cont = false;
			for(const auto& ptA : rA) {
				if(pti == ptA) {
					cont = true;
					break;
				}
			}
			if(cont)
				continue;
			segi = segment_t(ptB,pti);
			len = bg::length(segi);
			if(len < shortest) {
				trimmed.vector_ = pti - ptB;
				trimmed.edge_ = segi;
				shortest = len;
			}
		}
	}

	return trimmed;
}

int main(int argc, char** argv) {
	//TODO: tweak bg's floating point tolerance
	//TODO: remove superfluous points
	polygon_t pA;
	polygon_t pB;
	polygon_t nfp;

	read_polygon(argv[1], pA);
	read_polygon(argv[2], pB);
	assert(pA.outer().size() > 2);
	assert(pB.outer().size() > 2);
	write_svg<polygon_t>("start.svg", {pA, pB});
	std::cout << bg::wkt(pA) << std::endl;
	std::cout << bg::wkt(pB) << std::endl;

	//prevent double vertex connections at start because we might come back the same way we go which would end the nfp prematurely
	std::vector<psize_t> ptyaminI = find_minimum_y(pA);
	std::vector<psize_t> ptybmaxI = find_maximum_y(pB);
	point_t pAstart = pA.outer()[ptyaminI.front()];
	point_t pBstart = pB.outer()[ptybmaxI.front()];

	if(ptyaminI.size() > 1 && ptybmaxI.size() > 1) {
		//find right-most of A and left-most of B to prevent double connection at start
		coord_t maxX = MIN_COORD;
		psize_t iRightMost = 0;
		for(psize_t& ia : ptyaminI) {
			const point_t& candidateA = pA.outer()[ia];
			if(candidateA.x_ > maxX) {
				maxX = candidateA.x_;
				iRightMost = ia;
			}
		}

		coord_t minX = MAX_COORD;
		psize_t iLeftMost = 0;
		for(psize_t& ib : ptybmaxI) {
			const point_t& candidateB = pB.outer()[ib];
			if(candidateB.x_ < minX) {
				minX = candidateB.x_;
				iLeftMost = ib;
			}
		}
		pAstart = pA.outer()[iRightMost];
		pBstart = pB.outer()[iLeftMost];
	} else {
		pAstart = pA.outer()[ptyaminI.front()];
		pBstart = pB.outer()[ptybmaxI.front()];
	}

	point_t transB = {pAstart - pBstart};

	polygon_t pifsB;
	boost::geometry::transform(pB, pifsB, trans::translate_transformer<coord_t, 2, 2>(transB.x_, transB.y_));
	pB = std::move(pifsB);

	write_svg<polygon_t>("ifs.svg", {pA, pB});

	bool startAvailable = true;
	psize_t cnt = 0;
	point_t referenceStart = pB.outer().front();
	TranslationVector last;
	last.vector_ = INVALID_POINT;

	while(startAvailable) {
		//use first point of pB as reference
		nfp.outer().push_back(pB.outer().front());
		if(cnt == 4) {
			std::cerr << "bp" << std::endl;
		}
		std::vector<TouchingPoint> touchers = findTouchingPoints(pA.outer(), pB.outer());
		std::set<TranslationVector> transVectors = findFeasibleTranslationVectors(pA.outer(), pB.outer(), touchers);
		if(transVectors.empty()) {
			std::cerr << "empty" << std::endl;
		}
		assert(!transVectors.empty());

		std::cerr << "collected vectors: " << transVectors.size() << std::endl;
		for(auto pt : transVectors) {
			std::cerr << pt << std::endl;
		}

		TranslationVector next;
		if(transVectors.size() > 1)
			next = findNextTranslationVector(pA.outer(), pB.outer(), transVectors, last);
		else
			next = *transVectors.begin();
		std::cerr << "next: " << next << std::endl;
		last = next;
		TranslationVector trimmed = trimVector(pA.outer(), pB.outer(), next);
		std::cerr << "trimmed: " << trimmed << std::endl;

		polygon_t nextB;
		boost::geometry::transform(pB, nextB, trans::translate_transformer<coord_t, 2, 2>(trimmed.vector_.x_, trimmed.vector_.y_));
		pB = std::move(nextB);




		write_svg<polygon_t>("next" + std::to_string(cnt) + ".svg", {pA,pB});
		++cnt;
		if(referenceStart == pB.outer().front())
			startAvailable = false;
	}
	write_svg<polygon_t>("nfp.svg", {pA,pB,nfp});

	return 0;
}

