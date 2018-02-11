# libnfp
Implementation of a robust no-fit polygon generation in a C++ library

## Description

The no-fit polygon optimization makes it possible to check for overlap (or non-overlapping touch) of two polygons with only 1 point in polygon check.
This library implements the orbiting approach to generate the no-fit polygon: Given two polygons A and B, A is the stationary one and B the orbiting one, B is slid as tightly as possibly around the edges of polygon A. During the orbiting a chosen reference point is tracked. By tracking the movement of the reference point a third polygon can be generated: the no-fit polygon.

Once the no-fit polygon has been generated it can be used to test for overlap by only checking if the reference point is inside the NFP (overlap) outside the NFP (no overlap) or exactly on the edge of the NFP (touch).

### Examples:

The polygons: 

![Start of NFP](/images/start.png?raw=true)

Orbiting:

![State 1](/images/next0.png?raw=true)
![State 2](/images/next1.png?raw=true)
![State 3](/images/next2.png?raw=true)
![State 4](/images/next3.png?raw=true)

![State 5](/images/next4.png?raw=true)
![State 6](/images/next5.png?raw=true)
![State 7](/images/next6.png?raw=true)
![State 8](/images/next7.png?raw=true)

![State 9](/images/next8.png?raw=true)

The resulting NFP is red:

![nfp](/images/nfp.png?raw=true)

Polygons can have concavities, holes, interlocks or might fit perfectly:

![concavities](/images/concavities.png?raw=true)
![hole](/images/hole.png?raw=true)
![interlock](/images/interlock.png?raw=true)
![jigsaw](/images/jigsaw.png?raw=true)

## The Approach
The approch of this library is highly inspired by the scientific paper [Complete and robust no-fit polygon generation
for the irregular stock cutting problem](https://pdfs.semanticscholar.org/e698/0dd78306ba7d5bb349d20c6d8f2e0aa61062.pdf) and by [Svgnest](http://svgnest.com)

Note that is wasn't completely possible to implement it as suggested in the paper because it had several shortcomings that prevent complete NFP generation on some of my test cases. Especially the termination criteria (reference point returns to first point of NFP) proved to be wrong (see: test-case rect). Also tracking of used edges can't be performed as suggested in the paper since there might be situations where no edge of A is traversed (see: test-case doublecon).

At the moment the library is using infinite precision to prevent problems with decimals and floating point. Because of this it is rather slow. One of the next steps is going to be implementing floating point support. 

## Build
The library has two dependencies: [Boost Geometry](http://www.boost.org/doc/libs/1_65_1/libs/geometry/doc/html/index.html) and [libgmp](https://gmplib.org). You need to install those first before building. Note that building is only required for the example. The library itself is header-only.

    git clone https://github.com/kallaballa/libnfp.git
    cd libnfp
    make
    sudo make install

## Code Example

```c++
#include "../src/libnfp.hpp"

int main(int argc, char** argv) {
  polygon_t pA;
  polygon_t pB;
  //read polygons from wkt files
  read_wkt_polygon(argv[1], pA);
  read_wkt_polygon(argv[2], pB);

  //generate NFP
  nfp_t nfp = generateNFP(pA, pB);
  
  //write an svg containing pA, pB and NFP
  write_svg("nfp.svg",{pA,pB},nfp);
  return 0;
}
```
