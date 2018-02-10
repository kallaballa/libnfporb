# libnfp
Implementation of a robust no-fit polygon generation in a library

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

Polygons can have concavities, holes or might fit perfectly:

![concavities](/images/concavities.png?raw=true)
![hole](/images/hole.png?raw=true)
![jigsaw](/images/jigsaw.png?raw=true)
