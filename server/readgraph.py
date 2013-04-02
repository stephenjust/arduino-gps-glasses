"""
    python3 readgraph.py [ digraph-file ]

Takes a csv (comma separated values) text file containing the vertices
and edges of a street digraph and converts it into a digraph instance.

If the optional argument digraph-file is supplied, reads that, otherwise
takes input from stdin
"""
import sys
import pdb

# throw away executable name before processing command line arguments
argv = sys.argv[1:]

# if filename is supplied, use that, otherwise use stdin
if argv:
    digraph_file_name = argv.pop(0)
    digraph_file = open(digraph_file_name, 'r')
else:
    digraph_file = sys.stdin

# For testing, just use a simple representation of set of vertices, set of
# edges as ordered pairs, and dctionaries that map
#   vertex to (lat,long)
#   edge to street name

V = set()
E = set()
V_coord = { }
E_name = { }
pdb.set_trace()
# process each line in the file
for line in digraph_file:

    # strip all trailing whitespace
    line = line.rstrip()

    fields = line.split(",")
    type = fields[0]

    if type == 'V':
        # got a vertex record
        (id,lat,long) = fields[1:]

        # vertex id's should be ints
        id=int(id)

        # lat and long are floats
        lat=float(lat)
        long=float(long)

        V.add(id)
        V_coord[id] = (lat,long)
        
    elif type == 'E':
        # got an edge record
        (start,stop,name) = fields[1:]

        # vertices are ints
        start=int(start)
        stop=int(stop)
        e = (start,stop)

        # get rid of leading and trailing quote " chars around name
        name = name.strip('"')

        # consistency check, we don't want auto adding of vertices when
        # adding an edge.
        if start not in V or stop not in V:
            raise Exception("Edge {} has an endpoint that is not a vertex".format(e) )

        E.add(e)
        E_name[e] = name
    else:
        # weird input
        raise Exception("Error: weird line |{}|".format(line))

if True:
    print(V_coord)
    print()
    print(E_name)
