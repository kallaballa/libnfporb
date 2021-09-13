#ifndef SRC_ALGO_SELECT_NEXT_HPP_
#define SRC_ALGO_SELECT_NEXT_HPP_

#include "../geometry.hpp"
#include "../translation_vector.hpp"
#include "../history.hpp"

namespace libnfporb {
TranslationVector get_longest(const std::vector<TranslationVector>& tvs) {
	coord_t len;
	coord_t maxLen = MIN_COORD;
	TranslationVector longest;
	longest.vector_ = INVALID_POINT;

	for (auto& tv : tvs) {
		len = bg::length(segment_t { { 0, 0 }, tv.vector_ });
		if (larger(len, maxLen)) {
			maxLen = len;
			longest = tv;
		}
	}
	return longest;
}

void sort_by_length(std::vector<TranslationVector>& tvs) {
	std::sort( tvs.begin( ), tvs.end( ), [ ]( const TranslationVector& lhs, const TranslationVector& rhs ) {
		coord_t llen = bg::length(segment_t { { 0, 0 }, lhs.vector_ });
		coord_t rlen = bg::length(segment_t { { 0, 0 }, rhs.vector_ });
	   return llen < rlen;
	});
}


TranslationVector selectNextTranslationVector(const polygon_t& pA, const polygon_t::ring_type& rA, const polygon_t::ring_type& rB, const std::vector<TranslationVector>& tvs, const History& history) {
	if (history.size() > 1) {
		TranslationVector last = history.back();
		DEBUG_MSG("last", last);

		if(tvs.size() == 1) {
			return tvs.front();
		}

		std::vector<TranslationVector> viableTrans = tvs;

#ifdef NFP_DEBUG
		DEBUG_VAL("viable translations:");
		for (const auto& vtv : viableTrans) {
			DEBUG_VAL(vtv);
		}
		DEBUG_VAL("");

		if(history.size() > 5) {
			DEBUG_VAL("last 6 from history:");
			for (size_t i = 0; i < 6; ++i) {
				DEBUG_VAL(history[history.size() - 1 - i]);
			}
			DEBUG_VAL("");
		}
#endif

		size_t histAge = 0;
		size_t maxHistAge = 0;
		size_t histCnt = 0;
		size_t minHistCnt = history.size() + 1;
		TranslationVector oldest;
		TranslationVector least_used;

		DEBUG_VAL("non history viable translations:");
		for (const auto& vtv : viableTrans) {
			histAge = history.size() - find(history, vtv);
			histCnt = count(history, vtv);

			if(histCnt < minHistCnt) {
				minHistCnt = histCnt;
				least_used = vtv;
			}

			if(histAge >= maxHistAge) {
				maxHistAge = histAge;
				oldest = vtv;
			}
		}
		DEBUG_VAL("");

		sort_by_length(viableTrans);
		for(auto& candidate : viableTrans) {
			if(count(history, candidate) == 0) {
				DEBUG_MSG("longest unused", candidate);
				return candidate;
			}
		}

		if (tvs.size() == 1) {
			return tvs.front();
		} else {
			DEBUG_MSG("least used", least_used);
			return least_used;
		}
//		} else {
//			DEBUG_MSG("oldest", oldest);
//			return oldest;
//		}

		TranslationVector tv;
		tv.vector_ = INVALID_POINT;
		return tv;
	} else {
		auto longest = get_longest(tvs);
		DEBUG_MSG("longest", longest);
		return longest;
	}
}
}


#endif /* SRC_ALGO_SELECT_NEXT_HPP_ */
