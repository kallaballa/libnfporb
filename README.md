# libnfp
Implementation of a robust no-fit polygon generation in a library

## Description

The no-fit polygon optimization makes it possible to check for overlap (or non-overlapping touch) of two polygons with only 1 point in polygon check.
This library implements the orbiting approach to generate the no-fit polygon: Given two polygons A and B, A is the stationary one and B the orbiting one, B is slid as tightly as possibly around the edges of polygon A. During the orbiting a chosen reference point is tracked. By tracking the movement of the reference point a third polygon can be generated: the no-fit polygon.

