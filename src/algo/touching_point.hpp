
#ifndef SRC_ALGO_TOUCHING_POINT_HPP_
#define SRC_ALGO_TOUCHING_POINT_HPP_

#include "../geometry.hpp"

namespace libnfporb {
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

std::vector<TouchingPoint> findTouchingPoints(const polygon_t::ring_type& ringA, const polygon_t::ring_type& ringB) {
	std::vector<TouchingPoint> touchers;
	for (psize_t i = 0; i < ringA.size() - 1; i++) {
		psize_t nextI = i + 1;
		for (psize_t j = 0; j < ringB.size() - 1; j++) {
			psize_t nextJ = j + 1;
			if (equals(ringA[i], ringB[j])) {
				DEBUG_MSG("vertex", segment_t(ringA[i],ringB[j]));
				touchers.push_back( { TouchingPoint::VERTEX, i, j });
			} else if (!equals(ringA[nextI], ringB[j]) && bg::intersects(segment_t(ringA[i], ringA[nextI]), ringB[j])) {
				DEBUG_MSG("bona", segment_t(ringA[i],ringB[j]));
				touchers.push_back( { TouchingPoint::B_ON_A, nextI, j });
			} else if (!equals(ringB[nextJ], ringA[i]) && bg::intersects(segment_t(ringB[j], ringB[nextJ]), ringA[i])) {
				DEBUG_MSG("aonb", segment_t(ringA[i],ringB[j]));
				touchers.push_back( { TouchingPoint::A_ON_B, i, nextJ });
			}
		}
	}
	return touchers;
}
}


#endif /* SRC_ALGO_TOUCHING_POINT_HPP_ */
