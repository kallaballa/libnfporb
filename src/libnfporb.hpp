#ifndef NFP_HPP_
#define NFP_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <limits>

#include <boost/geometry/algorithms/intersects.hpp>

#include "geometry.hpp"
#include "svg.hpp"
#include "wkt.hpp"
#include "translation_vector.hpp"
#include "history.hpp"
#include "algo/touching_point.hpp"
#include "algo/trim_vector.hpp"
#include "algo/select_next.hpp"
#include "algo/find_feasible.hpp"
#include "algo/search_start.hpp"
#include "algo/slide.hpp"

namespace libnfporb {

/**
 * Delete oscillations and loops from the given ring.
 * @param ring A reference to the ring to be shortened.
 * @return true if the ring has changed.
 */
bool delete_consecutive_repeating_point_patterns(polygon_t::ring_type& ring) {
  size_t startLen = ring.size();
  off_t len = ring.size();
  int i, j, counter;
  for (i = 1; i <= len / 2; ++i) {
    for (j = i, counter = 0; j < len; ++j) {
      if (equals(ring[j], ring[j - i]))
        counter++;
      else
        counter = 0;
      if (counter > 2 && counter == i) {
        counter = 0;
        std::copy(ring.begin() + j, ring.begin() + len, ring.begin() + (j - i));
        j -= i;
        len -= i;
      }
    }
    ring.resize(j);
  }

  size_t start = 0, cnt = 0;
  point_t c, l = ring[0];

  for (size_t i = 1; i < ring.size(); ++i) {
    c = ring[i];
    if(equals(c, l)) {
      if(cnt == 0)
        start = i - 1;

      ++cnt;
    } else {
      if(cnt > 1) {
        ring.erase(ring.begin() + start + 1, ring.begin() + start + cnt);
        if(start + cnt >= ring.size())
          break;
      }
    }
    l = c;
  }
  return ring.size() != startLen;
}

/**
 * Remove co-linear points from a ring.
 * @param r A reference to a ring.
 */
void remove_co_linear(polygon_t::ring_type& r) {
	assert(r.size() > 2);
	psize_t nextI;
	psize_t prevI = 0;
	segment_t segment(r[r.size() - 2], r[0]);
	polygon_t::ring_type newR;

	for (psize_t i = 1; i < r.size() + 1; ++i) {
		if (i >= r.size())
			nextI = i % r.size() + 1;
		else
			nextI = i;

		if (get_alignment(segment, r[nextI]) != ON) {
			newR.push_back(r[prevI]);
		}
		segment = {segment.second, r[nextI]};
		prevI = nextI;
	}

	r = newR;
}
/**
 * Remove co-linear points from a polygon.
 * @param p A reference to a polygon.
 */
void remove_co_linear(polygon_t& p) {
	remove_co_linear(p.outer());
	for (auto& r : p.inners())
		remove_co_linear(r);

	bg::correct(p);
}

/**
 * Generate the NFP for the given polygons pA and pB. Optionally check the input polygons for validity.
 * @param pA polygon A (the stationary polygon).
 * @param pB polygon B (the orbiting polygon).
 *  * @param checkValidity Check the input polygons for validity if true. Defaults to true.
 * @return The generated NFP.
 */
nfp_t generate_nfp(polygon_t& pA, polygon_t& pB, const bool checkValidity = true) {
	remove_co_linear(pA);
	remove_co_linear(pB);

	if (checkValidity) {
		std::string reason;
		if (!bg::is_valid(pA, reason))
			throw std::runtime_error("Polygon A is invalid: " + reason);

		if (!bg::is_valid(pB, reason))
			throw std::runtime_error("Polygon B is invalid: " + reason);
	}

	nfp_t nfp;

#ifdef NFP_DEBUG
	write_svg("start.svg", {pA, pB});
#endif

	DEBUG_VAL(bg::wkt(pA));
	DEBUG_VAL(bg::wkt(pB));


	polygon_t pAtrans;
	polygon_t pBtrans;

	std::vector<psize_t> yAminI = find_minimum_y(pA);
	std::vector<psize_t> yBminI = find_minimum_y(pB);
	std::vector<psize_t> xAminI = find_minimum_x(pA);
	std::vector<psize_t> xBminI = find_minimum_x(pB);
	std::vector<psize_t> yAmaxI = find_maximum_y(pA);
	std::vector<psize_t> yBmaxI = find_maximum_y(pB);
	std::vector<psize_t> xAmaxI = find_maximum_x(pA);
	std::vector<psize_t> xBmaxI = find_maximum_x(pB);

	point_t preTrans;
	LongDouble leftA = pA.outer()[xAminI.front()].x_;
	LongDouble rightA = pA.outer()[xAmaxI.front()].x_;
	if(rightA < 0) {
		preTrans.x_ = rightA * -1;
	} else if(leftA < 0) {
		preTrans.x_ = leftA * -1;
	}

	LongDouble topA = pA.outer()[yAmaxI.front()].y_;
	LongDouble bottomA = pA.outer()[yAminI.front()].y_;
	if(topA < 0) {
		preTrans.y_ = topA * -1;
	} else if(bottomA < 0) {
		preTrans.y_ = bottomA * -1;
	}

	LongDouble leftB = pB.outer()[xBminI.front()].x_;
	LongDouble rightB = pB.outer()[xBmaxI.front()].x_;
	if(rightB < 0) {
		preTrans.x_ += rightB * -1;
	} else if(leftB < 0) {
		preTrans.x_ += leftB * -1;
	}

	LongDouble topB = pB.outer()[yBmaxI.front()].y_;
	LongDouble bottomB = pB.outer()[yBminI.front()].y_;
	if(topB < 0) {
		preTrans.y_ += topB * -1;
	} else if(bottomB < 0) {
		preTrans.y_ += bottomB * -1;
	}

	trans::translate_transformer<coord_t, 2, 2> transformer(preTrans.x_, preTrans.y_);
	boost::geometry::transform(pA, pAtrans, transformer);
	boost::geometry::transform(pB, pBtrans, transformer);
	pA = std::move(pAtrans);
	pB = std::move(pBtrans);

	point_t pAstart;
	point_t pBstart;

	if (yAminI.size() > 1 || yBmaxI.size() > 1) {
		//find right-most of A and left-most of B to prevent double connection at start
		coord_t maxX = MIN_COORD;
		psize_t iRightMost = 0;
		for (psize_t& ia : yAminI) {
			const point_t& candidateA = pA.outer()[ia];
			if (larger(candidateA.x_, maxX)) {
				maxX = candidateA.x_;
				iRightMost = ia;
			}
		}

		coord_t minX = MAX_COORD;
		psize_t iLeftMost = 0;
		for (psize_t& ib : yBmaxI) {
			const point_t& candidateB = pB.outer()[ib];
			if (smaller(candidateB.x_, minX)) {
				minX = candidateB.x_;
				iLeftMost = ib;
			}
		}
		pAstart = pA.outer()[iRightMost];
		pBstart = pB.outer()[iLeftMost];
	} else {
		pAstart = pA.outer()[yAminI.front()];
		pBstart = pB.outer()[yBmaxI.front()];
	}

	nfp.push_back( { });
	point_t transB = { pAstart - pBstart };



	SlideResult res;
	if ((res = slide(pA, pA.outer(), pB.outer(), nfp, transB, false))!= LOOP) {
		throw std::runtime_error("Unable to complete outer nfp loop: " + std::to_string(res));
	}

	DEBUG_VAL("##### outer #####");
	point_t startTrans;
	while (true) {
		SearchStartResult res = search_start_translation(pA.outer(), pB.outer(), nfp, false, startTrans);
		if (res == FOUND) {
			nfp.push_back( { });
			DEBUG_VAL("##### interlock start #####");
			polygon_t::ring_type rifsB;
			boost::geometry::transform(pB.outer(), rifsB, trans::translate_transformer<coord_t, 2, 2>(startTrans.x_, startTrans.y_));
			if (in_nfp(rifsB.front(), nfp)) {
				continue;
			}
			SlideResult sres = slide(pA, pA.outer(), pB.outer(), nfp, startTrans, true);
			if (sres != LOOP) {
				if (sres == NO_TRANSLATION) {
					//no initial slide found -> jigsaw
					if (!in_nfp(pB.outer().front(), nfp)) {
						nfp.push_back( { });
						nfp.back().push_back(pB.outer().front());
					}
				}
			}
			DEBUG_VAL("##### interlock end #####");
		} else if (res == FIT) {
			DEBUG_VAL("##### perfect fit #####");
			point_t reference = pB.outer().front();
			point_t translated;
			trans::translate_transformer<coord_t, 2, 2> translate(startTrans.x_, startTrans.y_);
			boost::geometry::transform(reference, translated, translate);
			if (!in_nfp(translated, nfp)) {
				nfp.push_back( { });
				nfp.back().push_back(translated);
			}
			break;
		} else
			break;
	}

	for (auto& rA : pA.inners()) {
		while (true) {
			SearchStartResult res = search_start_translation(rA, pB.outer(), nfp, true, startTrans);
			if (res == FOUND) {
				nfp.push_back( { });
				DEBUG_VAL("##### hole start #####");
				slide(pA, rA, pB.outer(), nfp, startTrans, true);
				DEBUG_VAL("##### hole end #####");
			} else if (res == FIT) {
				point_t reference = pB.outer().front();
				point_t translated;
				trans::translate_transformer<coord_t, 2, 2> translate(startTrans.x_, startTrans.y_);
				boost::geometry::transform(reference, translated, translate);
				if (!in_nfp(translated, nfp)) {
					nfp.push_back( { });
					nfp.back().push_back(translated);
				}
				break;
			} else
				break;
		}
	}

#ifdef NFP_DEBUG
	write_svg("nfp.svg", pA,pB, nfp);
#endif

	trans::translate_transformer<coord_t, 2, 2> transformerBack(-preTrans.x_, -preTrans.y_);

	polygon_t::ring_type translatedBack;
	for(auto& r : nfp) {
		while(delete_consecutive_repeating_point_patterns(r));
		bg::correct(r);
		boost::geometry::transform(r, translatedBack, transformerBack);
		r = std::move(translatedBack);
	}

	return nfp;
}
}
#endif
