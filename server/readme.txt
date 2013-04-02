Python Path-Finding Server
by Stephen Just
==========================

To execute the server, run the following:
  python server.py

There must be a file named edmonton_roads.txt in the script directory.

This server takes its input from stdin in the format:
  a b c d
where a, b, c, and d are integers representing 100,000ths of a degree.
The starting coordinate is (a, b), and the destination is (c, d). Vertices
are in the format (latitude, longitude).

The server returns a path in the following format:
  n
  x1 y1
  x2 y2
  ...
  xn yn
where n is the number of vertices in the path, and x and y are the
coordinates of each vertex (lat, long).
