"""
    python3 readgraph.py [ digraph-file ]

Takes a csv (comma separated values) text file containing the vertices
and edges of a street digraph and converts it into a digraph instance.

If the optional argument digraph-file is supplied, reads that, otherwise
takes input from stdin
"""
# For testing, just use a simple representation of set of vertices, set of
# edges as ordered pairs, and dctionaries that map
#   vertex to (lat,long)
#   edge to street name

def readgraph(filename):
    digraph_file = open(filename, 'r')

    V = set()
    E = set()
    V_coord = {}
    E_name = {}

    # process each line in the file
    for line in digraph_file:

        # strip all trailing whitespace
        line = line.rstrip()

        fields = line.split(",")
        type = fields[0]

        if type == 'V':
            # got a vertex record
            (i,lat,lng) = fields[1:]

            # vertex id's should be ints
            i=int(i)

            # lat and long are floats
            lat=float(lat)
            lng=float(lng)

            V.add(i)
            V_coord[i] = (lat,lng)

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

    return (V, E, V_coord, E_name)
