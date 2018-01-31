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

	point_t operator-(const point_t& other){
		return {this->x_ - other.x_, this->y_ - other.y_};
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
typedef typename polygon_t::ring_type::size_type psize_t;

std::ostream& operator<<(std::ostream& os, const segment_t& seg) {
	os << "{" << seg.first << "," << seg.second << "}";
	return os;
}

enum Alignment {
	LEFT,
	RIGHT,
	ON
};

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

point_t find_minimum_y(const polygon_t& p) {
	point_t result = {MAX_COORD, MAX_COORD};
	for(auto pt : p.outer()) {
		if(pt.y_ < result.y_) {
			result = pt;
		}
	}
	return result;
}

point_t find_maximum_y(const polygon_t& p) {
	point_t result = {MIN_COORD, MIN_COORD};
	for(auto pt : p.outer()) {
		if(pt.y_ > result.y_) {
			result = pt;
		}
	}
	return result;
}

std::vector<TouchingPoint> findTouchingPoints(const std::vector<point_t>& ringA, const std::vector<point_t>& ringB) {
	std::vector<TouchingPoint> touchers;
	for(psize_t i = 0; i < ringA.size() - 1; i++) {
		size_t nextI = i+1;
		for(psize_t j = 0; j < ringB.size() - 1; j++) {
			size_t nextJ = j+1;
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

std::set<point_t> findTranslationVectors(const std::vector<point_t>& ringA, const std::vector<point_t>& ringB, const std::vector<TouchingPoint>& touchers) {
	//use a set to automatically filter duplicate vectors
	std::set<point_t> potentialVectors;
	std::vector<std::pair<segment_t,segment_t>> touchEdges;

	for (psize_t i = 0; i < touchers.size(); i++) {
		point_t vertexA = ringA[touchers[i].A_];
		vertexA.marked_ = true;

		// adjacent A vertices
		signed long prevAindex = touchers[i].A_ - 1;
		signed long nextAindex = touchers[i].A_ + 1;

		prevAindex = (prevAindex < 0) ? ringA.size() - 2 : prevAindex; // loop
		nextAindex = (static_cast<psize_t>(nextAindex) >= ringA.size()) ? 1 : nextAindex; // loop

		point_t prevA = ringA[prevAindex];
		point_t nextA = ringA[nextAindex];

		// adjacent B vertices
		point_t vertexB = ringB[touchers[i].B_];

		signed long prevBindex = touchers[i].B_ - 1;
		signed long nextBindex = touchers[i].B_ + 1;

		prevBindex = (prevBindex < 0) ? ringB.size() - 2 : prevBindex; // loop
		nextBindex = (static_cast<psize_t>(nextBindex) >= ringB.size()) ? 1 : nextBindex; // loop

		point_t prevB = ringB[prevBindex];
		point_t nextB = ringB[nextBindex];

		if (touchers[i].type_ == TouchingPoint::VERTEX) {
			segment_t a1 = { vertexA, nextA };
			segment_t a2 = { prevA, vertexA };
			segment_t b1 = { vertexB, nextB };
			segment_t b2 = { prevB, vertexB };

			//swap the segment elements so that always the first point is the touching point
			touchEdges.push_back({a1, b1});
			touchEdges.push_back({a1, {b2.second, b2.first}});
			touchEdges.push_back({{a2.second, a2.first}, b1});
			touchEdges.push_back({{a2.second, a2.first}, {b2.second, b2.first}});

			std::cerr << a1 << " " << a2 << " " << b1 << " " << b2 << std::endl;
			write_svg<segment_t>("touchers" + std::to_string(i) + ".svg", {a1,a2,b1,b2});

			//TODO test parallel edges for floating point stability
			Alignment al;
			//a1 and b1 meet at start vertex
			al = get_aligment(a1, b1.second);
			if(al == LEFT) {
				potentialVectors.insert(b1.first - b1.second);
			} else if(al == RIGHT) {
				potentialVectors.insert(a1.second - a1.first);
			} else {
				potentialVectors.insert(a1.second - a1.first);
			}

			//a1 and b2 meet at start and end
			al = get_aligment(a1, b2.first);
			if(al == LEFT) {
				//no feasible translation
			} else if(al == RIGHT) {
				potentialVectors.insert(a1.second - a1.first);
			} else {
				potentialVectors.insert(a1.second - a1.first);
			}

			//a2 and b1 meet at end and start
			al = get_aligment(a2, b1.second);
			if(al == LEFT) {
				//no feasible translation
			} else if(al == RIGHT) {
				potentialVectors.insert(b1.first - b1.second);
			} else {
				potentialVectors.insert(a2.second - a1.first);
			}
		} else if (touchers[i].type_ == TouchingPoint::B_ON_A) { 		//FIXME: why does svgnest generate two vectors for B_ON_A and A_ON_B?
			potentialVectors.insert( { vertexA.x_ - vertexB.x_, vertexA.y_ - vertexB.y_ });
		} else if (touchers[i].type_ == TouchingPoint::A_ON_B) {
			potentialVectors.insert( { vertexA.x_ - vertexB.x_, vertexA.y_ - vertexB.y_ });

		}
	}

	//discard immediately intersecting translations
	std::set<point_t> vectors;
	for(auto v : potentialVectors) {
		bool discarded = false;
		for(auto sp : touchEdges) {
			point_t& transStart = sp.first.first;
			point_t transEnd;
			trans::translate_transformer<coord_t, 2, 2> translate(v.x_, v.y_);
			boost::geometry::transform(transStart, transEnd, translate);
			segment_t transSeg(transStart, transEnd);
			Alignment a1 = get_aligment(transSeg, sp.first.second);
			Alignment a2 = get_aligment(transSeg, sp.second.second);
			if(a1 == a2 && a1 != ON) {
				// both segments are either left or right of the translation segment and the translation is discarded
				discarded = true;
				break;
			}
		}
		if(!discarded)
			vectors.insert(v);
	}
	return vectors;
}


int main(int argc, char** argv) {
	//TODO: tweak bg's floating point tolerance
	//TODO: remove superfluous points
	polygon_t pA;
	polygon_t pB;
	polygon_t pifsB;

	read_polygon(argv[1], pA);
	read_polygon(argv[2], pB);

	write_svg<polygon_t>("start.svg", {pA, pB});
	std::cout << bg::wkt(pA) << std::endl;
	std::cout << bg::wkt(pB) << std::endl;

	point_t ptyamin = find_minimum_y(pA);
	point_t ptybmax = find_maximum_y(pB);
	point_t transB = {ptyamin - ptybmax};
	trans::translate_transformer<coord_t, 2, 2> translate(transB.x_, transB.y_);
	boost::geometry::transform(pB, pifsB, translate);
	write_svg<polygon_t>("ifs.svg", {pA, pifsB});
	pB = std::move(pifsB);

	std::vector<TouchingPoint> touchers = findTouchingPoints(pA.outer(), pB.outer());
	std::set<point_t> transVectors = findTranslationVectors(pA.outer(), pB.outer(), touchers);

	std::cerr << "collected vectors: " << transVectors.size() << std::endl;
	for(auto pt : transVectors) {
		std::cerr << pt << std::endl;
	}
	return 0;
}

