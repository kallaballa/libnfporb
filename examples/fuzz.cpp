//#define LIBNFP_USE_RATIONAL
#include "../src/libnfp.hpp"
#include <random>

int main(int argc, char** argv) {
  using namespace libnfp;
	polygon_t pA;
	polygon_t pB;

	read_wkt_polygon(argv[1], pA);
	read_wkt_polygon(argv[2], pB);

  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(-1.0, 1.0);

	for(auto& ptA : pA.outer()) {
		ptA.x_ += dist(mt);
		ptA.y_ += dist(mt);
	}
	pA.outer().back() = pA.outer().front();

	for(auto& ptB : pB.outer()) {
		ptB.x_ += dist(mt);
		ptB.y_ += dist(mt);
	}
	pB.outer().back() = pB.outer().front();

	for(auto& rA : pA.inners()) {
		for(auto& ptA : rA) {
			ptA.x_ += dist(mt);
			ptA.y_ += dist(mt);
		}
		rA.back() = rA.front();
	}

	for(auto& rB : pB.inners()) {
		for(auto& ptB : rB) {
			ptB.x_ += dist(mt);
			ptB.y_ += dist(mt);
		}
		rB.back() = rB.front();
	}

	nfp_t nfp = generateNFP(pA, pB);
	write_svg("nfp.svg",{pA,pB},nfp);
	return 0;
}

