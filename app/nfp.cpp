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

struct Toucher {
	enum Type {
		VERTEX,
		A_ON_B,
		B_ON_A
	};
	Type type_;
	psize_t A_;
	psize_t B_;
};

struct TransVector {
	coord_t x_;
	coord_t y_;
	point_t start_;
	point_t end_;
};

template <typename Geometry>
void write_svg(std::string const& filename,typename std::vector<Geometry> const& geometries) {
    typedef typename boost::geometry::point_type<Geometry>::type point_type;
    std::ofstream svg(filename.c_str());

    boost::geometry::svg_mapper<point_type> mapper(svg, 1000, 1000, "width=\"10%\" height=\"10%\"");
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

std::vector<Toucher> findTouchers(const std::vector<point_t>& ringA, const std::vector<point_t>& ringB) {
	std::vector<Toucher> touchers;
	for(psize_t i = 0; i < ringA.size() - 1; i++) {
		size_t nextI = i+1;
		for(psize_t j = 0; j < ringB.size() - 1; j++) {
			size_t nextJ = j+1;
			if(bg::intersects(ringA[i], ringB[j])) {
				touchers.push_back({Toucher::VERTEX, i, j});
			} else if (!bg::intersects(ringA[nextI], ringB[j]) && bg::intersects(segment_t(ringA[i],ringA[nextI]), ringB[j])) {
				touchers.push_back({Toucher::B_ON_A, nextI, j});
			} else if (!bg::intersects(ringB[nextJ], ringA[i]) && bg::intersects(segment_t(ringB[j],ringB[nextJ]), ringA[i])) {
				touchers.push_back({Toucher::A_ON_B, i, nextJ});
			}
		}
	}
	return touchers;
}

std::set<point_t> findPotentialVectors(const std::vector<point_t>& ringA, const std::vector<point_t>& ringB, const std::vector<Toucher>& touchers) {
	std::set<point_t> transVectors;
	for (psize_t i = 0; i < touchers.size(); i++) {
		point_t vertexA = ringA[touchers[i].A_];
		vertexA.marked_ = true;

		// adjacent A vertices
		signed long prevAindex = touchers[i].A_ - 1;
		signed long nextAindex = touchers[i].A_ + 1;

		prevAindex = (prevAindex < 0) ? ringA.size() - 2 : prevAindex; // loop
		nextAindex = (nextAindex >= ringA.size()) ? 1 : nextAindex; // loop

		point_t prevA = ringA[prevAindex];
		point_t nextA = ringA[nextAindex];

		// adjacent B vertices
		point_t vertexB = ringB[touchers[i].B_];

		signed long prevBindex = touchers[i].B_ - 1;
		signed long nextBindex = touchers[i].B_ + 1;

		prevBindex = (prevBindex < 0) ? ringB.size() - 2 : prevBindex; // loop
		nextBindex = (nextBindex >= ringB.size()) ? 1 : nextBindex; // loop

		point_t prevB = ringB[prevBindex];
		point_t nextB = ringB[nextBindex];

		if (touchers[i].type_ == Toucher::VERTEX) {
			segment_t a1 = { vertexA, nextA };
			segment_t a2 = { prevA, vertexA };
			segment_t b1 = { vertexB, nextB };
			segment_t b2 = { prevB, vertexB };
			std::cerr << a1 << " " << a2 << " " << b1 << " " << b2 << std::endl;
			write_svg<segment_t>("touchers" + std::to_string(i) + ".svg", {a1,a2,b1,b2});

			//FIXME test parallel edges for floating point stability
			Alignment al;
			//a1 and b1 meet at start vertex
			al = get_aligment(a1, b1.second);
			if(al == LEFT) {
				std::cerr << "left" << std::endl;
				transVectors.insert(b1.first - b1.second);
			} else if(al == RIGHT) {
				std::cerr << "right" << (a1.second - a1.first) << std::endl;
				transVectors.insert(a1.second - a1.first);
			} else {
				std::cerr << "parallel" << std::endl;
				transVectors.insert(a1.second - a1.first);
			}

			//a1 and b2 meet at start and end
			al = get_aligment(a1, b2.first);
			if(al == LEFT) {
				std::cerr << "left" << std::endl;
				//no feasible edge
			} else if(al == RIGHT) {
				std::cerr << "right" << (a1.second - a1.first) << std::endl;
				transVectors.insert(a1.second - a1.first);
			} else {
				std::cerr << "parallel" << std::endl;
				transVectors.insert(a1.second - a1.first);
			}

			//a2 and b1 meet at end and start
			al = get_aligment(a2, b1.second);
			if(al == LEFT) {
				std::cerr << "left" << std::endl;
				//no feasible edge
			} else if(al == RIGHT) {
				std::cerr << "right" << (b1.first - b1.second) << std::endl;
				transVectors.insert(b1.first - b1.second);
			} else {
				std::cerr << "parallel" << std::endl;
				transVectors.insert(a2.second - a1.first);
			}
		} else if (touchers[i].type_ == Toucher::B_ON_A) { 		//FIXME: why does svgnest generate two vectors for B_ON_A and A_ON_B?
			transVectors.insert( { vertexA.x_ - vertexB.x_, vertexA.y_ - vertexB.y_ });
		} else if (touchers[i].type_ == Toucher::A_ON_B) {
			transVectors.insert( { vertexA.x_ - vertexB.x_, vertexA.y_ - vertexB.y_ });
		}
	}
	return transVectors;
}


int main(int argc, char** argv) {
	//TODO: tweak bg's floating point tolerance
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
	pB = pifsB;


	auto& pAO = pA.outer();
	auto& pBO = pB.outer();

	std::vector<Toucher> touchers = findTouchers(pA.outer(), pB.outer());
	std::set<point_t> transVectors = findPotentialVectors(pA.outer(), pB.outer(), touchers);

	std::cerr << "collected vectors: " << transVectors.size() << std::endl;
	for(auto pt : transVectors) {
		std::cerr << pt << std::endl;
	}
	return 0;
}

