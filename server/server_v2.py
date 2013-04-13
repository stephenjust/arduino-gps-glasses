"""
Python route-finding server

Call this program with the map data file as a parameter
$ python server.py roads-digraph.txt

CMPUT297 Assignment #3
Stephen Just

This assignment is a solo effort, although discussion about the assignment
occurred with several students: Anders Muskens, Stefan Martynkiw

"""

import argparse
import digraph
import pqueue
import readgraph
import serial
import sys
import time

global debug
debug = False

def send(serial_port, message):
    """
    Sends a message back to the client device.
    """
    full_message = ''.join((message, "\n"))

    (debug and
        print("server:" + full_message + ":") )

    reencoded = bytes(full_message, encoding='ascii')
    serial_port.write(reencoded)


def receive(serial_port, timeout=None):
    """
    Listen for a message. Attempt to timeout after a certain number of
    milliseconds.
    """
    raw_message = serial_port.readline()

    debug and print("client:", raw_message, ":")

    message = raw_message.decode('ascii')

    return message.rstrip("\n\r")



def parse_args():
    """
    Parses arguments for this program.
    Returns an object with the following members:
        args.
             serialport -- str
             verbose    -- bool
             graphname  -- str
    """

    parser = argparse.ArgumentParser(
        description='Assignment 1: Map directions.',
        epilog = 'If SERIALPORT is not specified, stdin/stdout are used.')
    parser.add_argument('-s', '--serial',
                        help='path to serial port',
                        dest='serialport',
                        default=None)
    parser.add_argument('-v', dest='verbose',
                        help='verbose',
                        action='store_true')
    parser.add_argument('-g', '--graph',
                        help='path to graph (DEFAULT = " edmonton_roads.txt")',
                        dest='graphname',
                        default='edmonton_roads.txt')

    return parser.parse_args()

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

def main():
    """
    Run the server
    """
    
    time1 = time.time()
    print("Initializing..")

    # Grab program arguments
    args = parse_args()

    # Load data file
    try:
        global V_coord
        (V, E, V_coord, E_name) = readgraph.readgraph(args.graphname)
    except FileNotFoundError:
        print("Data file {} not found!".format(args.graphname), file=sys.stderr)
        exit()

    # Initialize serial port
    try:
        if args.serialport:
            print("Opening serial port: {}".format(args.serialport))
            serial_out = serial_in = serial.Serial(args.serialport, 9600)
        else:
            print("No serial port. Supply one with the -s port option.")
            exit()
    except serial.SerialException:
        print("Could not open serial port: {}".format(args.serialport),
              file=sys.stderr)
        exit()

    # Set debug mode
    if args.verbose:
        debug = True
    else:
        debug = False
    debug = True

    # Generate a graph from the map and grab the needed values
    G = digraph.Digraph(E)
    # Add all orphan vertices. Not really useful, and in fact detrimental
    # in all cases, but do it for the sake of completeness
    for v in V:
        G.add_vertex(v)

    # Print some debug output
    if debug:
        print("Graph loaded with {} vertices and {} edges.".format(G.num_vertices(), G.num_edges()))

    # initialize storage value
    prev_end = 0
    time2 = time.time()
    delta_t = repr(time2 - time1)
    print("Done initializing, took " + delta_t + " seconds")

    # Parse input
    while True:
        msg = receive(serial_in)
        debug and print("GOT:{}:".format(msg), file=sys.stderr)

        fields = msg.split(" ")

        # Ignore malformed messages
        if len(fields) != 4:
            debug and print("Ignoring message: {}".format(msg))
            continue
        time1 = time.time()
        print("Processing..")
        # Get start and end vertices
        start_v = (int(fields[0])/10**5, int(fields[1])/10**5)
        end_v = (int(fields[2])/10**5, int(fields[3])/10**5)
        start = nearest_vertex( start_v )
        end = nearest_vertex( end_v )

        debug and print("Routing path from vertex {} to {}".format(start, end))

        if end is prev_end:
            # to speed things up, if the user wants to go to the same destination
            # as last time, find which point in the previous path
            # the user is close to, and return the shortest distance to
            # the next point in the previous path
            min_dist = float('infinity')
            for i in range(len(path)):
                print("i is: " + str(i) + " and path[i] is: " + str(path[i]))
                dist = distance(V_coord[start], V_coord[path[i]])
                if dist < min_dist:
                    closest_v = path[i]
                    min_dist = dist
                    next_dest = path[i + 1]
            
            if closest_v == prev_end:
                # we're there!
                prev_end = 0
                continue
            
            secondary_path = least_cost_path(G, start, next_dest, cost_distance)
            
            send(serial_out, str(len(secondary_path)))
            print("The secondary path is:")
            for v in secondary_path:
                print(str(v))
                print("lat: " + str(int(V_coord[v][0] * 10**5)) + " lon: " + str(int(V_coord[v][1] * 10**5)))
                send(serial_out, "{} {}".format(int(V_coord[v][0] * 10**5),
                                     int(V_coord[v][1] * 10**5)))
                
            print("Send path of length {}".format(len(secondary_path)))
            
            time2 = time.time()
            delta_t = repr(time2 - time1)
            print("Done processing, took " + delta_t + "seconds")
            continue

        path = least_cost_path(G, start, end, cost_distance)
        if path is None:
            send(serial_out, "0")
            debug and print("No path found!", file=sys.stderr)
        else:
            send(serial_out, str(len(path)))
            print("The path is:")
            for v in path:
                print(str(v))
                print("lat: " + str(int(V_coord[v][0] * 10**5)) + " lon: " + str(int(V_coord[v][1] * 10**5)))
                send(serial_out, "{} {}".format(int(V_coord[v][0] * 10**5),
                                     int(V_coord[v][1] * 10**5)))
            print("Send path of length {}".format(len(path)))
            
        
        # store for optimization
        prev_start = start
        prev_end = end
        
        time2 = time.time()
        delta_t = repr(time2 - time1)
        print("Done processing, took " + delta_t + "seconds")
        

# Execute the main function
if __name__ == "__main__":
    main()
