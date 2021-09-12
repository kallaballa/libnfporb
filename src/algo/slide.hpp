#ifndef SRC_ALGO_SLIDE_HPP_
#define SRC_ALGO_SLIDE_HPP_

#include "../geometry.hpp"
#include "../translation_vector.hpp"

namespace libnfporb {

enum SlideResult {
	LOOP,
	NO_LOOP,
	NO_TRANSLATION
};

SlideResult slide(polygon_t& pA, polygon_t::ring_type& rA, polygon_t::ring_type& rB, nfp_t& nfp, const point_t& transB, bool inside) {
	polygon_t::ring_type rifsB;
	boost::geometry::transform(rB, rifsB, trans::translate_transformer<coord_t, 2, 2>(transB.x_, transB.y_));
	rB = std::move(rifsB);

#ifdef NFP_DEBUG
	write_svg("ifs.svg", pA, rB);
#endif

	bool startAvailable = true;
	psize_t cnt = 0;
	point_t referenceStart = rB.front();
	History history;

	//generate the nfp for the ring
	while (startAvailable) {
		DEBUG_VAL("");
		DEBUG_VAL("");
		DEBUG_VAL("#### iteration: " + std::to_string(cnt) + " ####");

		//use first point of rB as reference
		nfp.back().push_back(rB.front());
		if (cnt == 15)
			std::cerr << "";

		std::vector<TouchingPoint> touchers = findTouchingPoints(rA, rB);

		DEBUG_MSG("touchers", touchers.size());

		if (touchers.empty()) {
			throw std::runtime_error("Internal error: No touching points found");
		}
		std::vector<TranslationVector> transVectors = findFeasibleTranslationVectors(rA, rB, touchers);


#ifdef NFP_DEBUG
		DEBUG_MSG("collected vectors", transVectors.size());
		for(auto pt : transVectors) {
			DEBUG_VAL(pt);
		}
#endif

		if (transVectors.empty()) {
			return NO_LOOP;
		}

		TranslationVector next = selectNextTranslationVector(pA, rA, rB, transVectors, history);

		if (next.vector_ == INVALID_POINT)
			return NO_TRANSLATION;

		TranslationVector trimmed = trimVector(rA, rB, next);
		DEBUG_MSG("trimmed", trimmed);

		DEBUG_MSG("next", next);
		history.push_back(next);

		polygon_t::ring_type nextRB;
		boost::geometry::transform(rB, nextRB, trans::translate_transformer<coord_t, 2, 2>(trimmed.vector_.x_, trimmed.vector_.y_));
		rB = std::move(nextRB);

#ifdef NFP_DEBUG
		write_svg("next" + std::to_string(cnt) + ".svg", pA,rB);
#endif
		if(bg::overlaps(pA, rB))
			throw std::runtime_error("Internal Error: Slide resulted in overlap");

		++cnt;
		if (referenceStart == rB.front() || (inside && bg::touches(rB.front(), nfp.front()))) {
			startAvailable = false;
		}
	}
	return LOOP;
}
}


#endif /* SRC_ALGO_SLIDE_HPP_ */
