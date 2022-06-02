#ifndef SRC_ALGO_TRIM_VECTOR_HPP_
#define SRC_ALGO_TRIM_VECTOR_HPP_

#include "../geometry.hpp"
#include "../translation_vector.hpp"

namespace libnfporb {
//TODO deduplicate code
/**
 * @brief Trim a given translation vector so that it doesn't intersect with either rA or rB.
 * @param rA Ring of A.
 * @param rB Ring of B.
 * @param tv The given translation vector
 * @return The trimmed translation vector
 */
TranslationVector trim_vector(const polygon_t::ring_type& rA, const polygon_t::ring_type& rB, const TranslationVector& tv) {
	coord_t shortest = bg::length(tv.edge_);
	TranslationVector trimmed = tv;
	for (const auto& ptA : rA) {
		//for polygon A we invert the translation
		point_t translated = { ptA.x_ - tv.vector_.x_, ptA.y_ - tv.vector_.y_};
		linestring_t projection;
		segment_t segproj(ptA, translated);
		projection.push_back(ptA);
		projection.push_back(translated);
		std::vector<point_t> intersections;
		bg::intersection(rB, projection, intersections);
		if (intersections.size() < 2) {
			bool found = false;
			for (const auto& ptB : rB) {
				if(equals(ptA,ptB)) {
					found = true;
					break;
				}
			}
			if(found)
				continue;
		}

		//find shortest intersection
		coord_t len;
		segment_t segi;
		for (const auto& pti : intersections) {
			segi = segment_t(ptA, pti);
			len = bg::length(segi);
			if (smaller(NFP_EPSILON, len) && smaller(len, shortest)) {
				trimmed.vector_ = ptA - pti;
				trimmed.edge_ = segi;
				shortest = len;
			}
		}
	}

	for (const auto& ptB : rB) {
		point_t translated = { ptB.x_ + tv.vector_.x_, ptB.y_ + tv.vector_.y_};
		linestring_t projection;
		segment_t segproj(ptB, translated);
		projection.push_back(ptB);
		projection.push_back(translated);
		std::vector<point_t> intersections;
		bg::intersection(rA, projection, intersections);
		if (intersections.size() < 2) {
			bool found = false;
			for (const auto& ptA : rA) {
				if(equals(ptB, ptA)) {
					found = true;
					break;
				}
			}
			if(found)
				continue;
		}
		//find shortest intersection
		coord_t len;
		segment_t segi;
		for (const auto& pti : intersections) {

			segi = segment_t(ptB, pti);
			len = bg::length(segi);
			if (smaller(NFP_EPSILON, len) && smaller(len, shortest)) {
				trimmed.vector_ = pti - ptB;
				trimmed.edge_ = segi;
				shortest = len;
			}
		}
	}
	return trimmed;
}
}

#endif /* SRC_ALGO_TRIM_VECTOR_HPP_ */
