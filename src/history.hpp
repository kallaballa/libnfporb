#ifndef SRC_HISTORY_HPP_
#define SRC_HISTORY_HPP_

#include "geometry.hpp"
#include "translation_vector.hpp"

namespace libnfporb {
typedef std::vector<TranslationVector> History;

off_t find(const History& h, const TranslationVector& tv, const off_t& offset = 0) {
	if(offset < 0)
		return -1;

	for(size_t i = offset; i < h.size(); ++i) {
		if (h[i] == tv) {
			return i;
		}
	}

	return -1;
}

size_t count(const History& h, const TranslationVector& tv) {
	size_t cnt = 0;
	off_t offset = 0;
	while((offset = find(h,tv, offset + 1)) != -1)
		++cnt;

	return cnt;
}
}

#endif /* SRC_HISTORY_HPP_ */
