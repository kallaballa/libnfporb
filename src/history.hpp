#ifndef SRC_HISTORY_HPP_
#define SRC_HISTORY_HPP_

#include "geometry.hpp"
#include "translation_vector.hpp"

namespace libnfporb {
typedef std::vector<TranslationVector> History;

/**
 * Find a translation vector in the history.
 * @param h The history.
 * @param tv The translation vector.
 * @param offset The index offset to start searching from
 * @return returns The index of the translation vector or -1 if it isn't found.
 */
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

/**
 * Count the occurences of a translation vector in the history.
 * @param h The history.
 * @param tv The translation vector.
 * @return The count of occurrences in the history.
 */size_t count(const History& h, const TranslationVector& tv) {
	size_t cnt = 0;
	off_t offset = 0;
	while((offset = find(h,tv, offset + 1)) != -1)
		++cnt;

	return cnt;
}
}

#endif /* SRC_HISTORY_HPP_ */
