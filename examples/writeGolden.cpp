//uncomment next line to use infinite precision (slow)
//#define LIBNFP_USE_RATIONAL
#include "../src/libnfporb.hpp"

#include <iostream>

using namespace libnfporb;
using namespace std;


int main(int argc, char** argv) {

  polygon_t pA;
  polygon_t pB;
  //read polygons from wkt files
  read_wkt_polygon(argv[1], pA);
  read_wkt_polygon(argv[2], pB);
  
  //generate NFP of polygon A and polygon B and check the polygons for validity.
  //When the third parameters is false validity check is skipped for a little performance increase
  nfp_t nfp = generateNFP(pA, pB, true);
  
  ofstream wktfile(argv[3]);
  for(polygon_t::ring_type ring : nfp) {
    wktfile << std::setprecision(12) << bg::wkt(ring) << endl;
  }
  wktfile.close();
    
  return 0;
}

