# libnfp
Implementation of a robust no-fit polygon generation in a library

## Description

The no-fit polygon optimization makes it possible to check for overlap (or non-overlapping touch) of two polygons with only 1 point in polygon check.
This library implements the orbiting approach to generate the no-fit polygon: Given two polygons A and B, A is the stationary one and B the orbiting one, B is slid as tightly as possibly around the edges of polygon A. During the orbiting a chosen reference point is tracked. By tracking the movement of the reference point a third polygon can be generated: the no-fit polygon.

Once the no-fit polygon has been generated it can be used to test for overlap be only checking if the reference point is inside the NFP (overlap) outside the NFP (no overlap) or exactly on the edge of the NFP (touch).

Example:
