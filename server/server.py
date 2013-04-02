"""
Python route-finding server

Call this program with the map data file as a parameter
$ python server.py roads-digraph.txt

CMPUT297 Assignment #3
Stephen Just

This assignment is a solo effort, although discussion about the assignment
occurred with several students: Anders Muskens, Stefan Martynkiw

generate_map() function from Michael Blouin
"""

import digraph
import pqueue
import readgraph
import sys
import time


def distance(a, b):
    """
    Calculate the distance between two coordinates
    using a standard distance equation
    ( (x1-x2)^2 + (y1-y2)^2 )^0.5

    >>> distance( (0, 0), (3, 4) ) == 5
    True
    >>> round(distance( (-1.6, 2.4), (3.45, -5) )) == 9
    True
    """
    if len(a) != 2 or len(b) != 2:
        raise ValueError

    dx = float(a[0]) - float(b[0])
    dy = float(a[1]) - float(b[1])

    return ( dx**2 + dy**2 )**0.5

def nearest_vertex(coord):
    """
    Find the nearest vertex to a coordinate

    This cannot be unit tested generically here because overriding
    V_coord here in a doctest does not seem to override V_coord
    used in the function, and spec says we have load the Edmonton roads
    graph
    """
    # Make sure a coordinate was passed in
    if len(coord) != 2:
        raise ValueError

    # Make sure any vertex has a shorter distance
    shortest_dist = float('inf')
    vertex = -1
    
    # Loop through all vertices and find closest
    for v in V_coord:
        dist = distance(coord, V_coord[v])
        if dist < shortest_dist:
            shortest_dist = dist
            vertex = v

    return vertex

def cost_distance(e):
    """
    Get the distance between two vertices

    Like above we can't test it here as is because I can't
    override V_coord.
    """
    # Make sure we have a proper edge with two vertices
    if len(e) != 2:
        raise ValueError

    a = V_coord[e[0]]
    b = V_coord[e[1]]

    # Return the distance between two points
    return distance(a, b)

def least_cost_path(G, start, dest, cost):
    """
    path = least_cost_path(G, start, dest, cost)

    least_cost_path returns a least cost path in the digraph G from vertex
    start to vertex dest, where costs are defined by the cost function.
    cost should be a function that takes a single edge argument and returns
    a real-valued cost.

    if there is no path, then returns None

    the path from start to start is [start]

    >>> g = digraph.Digraph( [(1, 2), (4, 2), (4, 6), (6, 7), (1, 7), (2, 4)] )
    >>> least_cost_path(g, 1, 7, lambda e: abs(2*e[0]-e[1]))
    [1, 7]
    >>> least_cost_path(g, 7, 2, lambda e: 1) is None
    True
    """

    # Create a priority queue
    todo = pqueue.PQueue()
    todo.update(start, 0);

    # v in visited when the vertex v's least cost from start has been determined
    visited = set()

    # parent[v] is the vertex that just precedes v in the path from start to v
    parent = {}

    while todo and (dest not in visited):

        # priority queue operation
        # remove smallest estimated cost vertex from todo list
        (cur, c) = todo.pop_smallest()

        # it is now visited, and will never have a smaller cost
        visited.add(cur)

        for n in G.adj_to(cur):
            if n in visited: continue
            if todo.update(n, c+cost((cur,n))):
                parent[n] = cur

    # now, if there is a path, extract it.  The graph may be disconnected
    # so in that case return None
    if dest not in visited:
        return None

    path = [dest]
    cur = dest
    while start not in path:
        cur = parent[cur]
        path.append(cur)

    path.reverse()
    return path

def generate_map(path):
    """
    Formats a path into a form that is easily visualized on the
    multiplottr.com website.

    Simply paste the output of the function into the "Bulk
    Addresses" box and hit submit.

    (Provided by Michael Blouin via C297 discussion board)
    """

    print("{}, {} |Start|GREEN|Endpoints".format(path[0][0], path[0][1]))
    for p in range(1, len(path) - 1):
        print("{}, {} |Waypoint {}|BLUE|Waypoints".format(path[p][0], path[p][1], p))
        print("{}, {} |Destionation|RED|Endpoints".format(path[-1][0], path[-1][1]))
        print("{}, {} |BLUE|LINE1|".format(path[0][0], path[0][1]))
    for p in path[1:-1]:
        print("{}, {} |BLUE|LINE1|".format(p[0], p[1]))
        print("{}, {} |BLUE|LINE1|".format(path[-1][0], path[-1][1]))
        print("{}, {} |15|CENTER".format((path[0][0]+path[-1][0])/2, (path[0][1]+path[-1][1])/2))

# Initialize the server by first loading the data file
try:
    if len(sys.argv) != 2:
        filename = 'edmonton_roads.txt'
    else:
        filename = argv[1]
    (V, E, V_coord, E_name) = readgraph.readgraph(filename)
except FileNotFoundError:
    print("Data file not found!")
    exit()

# Generate a graph from the map and grab the needed values
G = digraph.Digraph(E)
# Add all orphan vertices. Not really useful, and in fact detrimental
# in all cases, but do it for the sake of completeness
for v in V:
    G.add_vertex(v)


if __name__ == "__main__":
    """
    Handle input
    """
    source = sys.stdin
    while True:
        try:
            data = source.readline()
            if len(data) == 0:
                # Wait a bit for the next request
                time.sleep(0.5)
                continue
            params = data.split()

            # Check for the proper number of input parameters
            if len(params) != 4:
                raise ValueError
            # Check that all parameters are numeric
            for x in params:
                int(x) # Will raise ValueError if not numeric

            # Get start and end vertices
            start_v = (int(params[0])/10**5, int(params[1])/10**5)
            end_v = (int(params[2])/10**5, int(params[3])/10**5)
            start = nearest_vertex( start_v )
            end = nearest_vertex( end_v )

            path = least_cost_path(G, start, end, cost_distance)
            if path is None:
                print(0)
            else:
                print(len(path))
                for v in path:
                    print("{} {}".format(int(V_coord[v][0] * 10**5),
                                         int(V_coord[v][1] * 10**5)))
        except ValueError:
            print(0)
