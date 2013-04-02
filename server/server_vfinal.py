"""
    CMPUT 297/115 - Assignment 3 Server - Due 2013-03-08

    Version 1.4 2013-03-08

    By: Andrew Lawrence
    
    This assignment is a solo effort, and
    any extra resources are cited in the code below.
    
    I implemented a lot of code which was presented in class.

    python3 readgraph.py [ digraph-file ]

Takes a csv (comma separated values) text file containing the vertices
and edges of a street digraph and converts it into a digraph instance.

If the optional argument digraph-file is supplied, reads that, otherwise
takes input from stdin
"""

import sys
import digraph
import math
import string
import pdb
import serial
import argparse

# just use a simple representation of set of vertices, set of
# edges as ordered pairs, and dctionaries that map
#   vertex to (lat,long)
#   edge to street name

digraph_file = open('edmonton-roads-2.0.1.txt', 'r')

V = set()
E = set()
V_coord = { }
E_name = { }

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
        
# make digraph out of data
Dg = digraph.Digraph(E)
"""
These funcitons will allow the server to process serial data from the client
"""

def send(serial_port, message):
    """
    Sends a message back to the client device.
    """
    full_message = ''.join((message, "\n"))

    #(debug and
        #print("server:" + full_message + ":") )

    reencoded = bytes(full_message, encoding='ascii')
    serial_port.write(reencoded)


def receive(serial_port, timeout=None):
    """
    Listen for a message. Attempt to timeout after a certain number of
    milliseconds.
    """
    raw_message = serial_port.readline()
    
    if not raw_message:
        return None

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
                        help='path to graph (DEFAULT = " edmonton-roads-2.0.1.txt")',
                        dest='graphname',
                        default=' edmonton-roads-2.0.1.txt')

    return parser.parse_args()

"""
These functions will determine the least cost path given a graph, a specified starting point, and ending point
"""

def cost_distance(e):
  # look up the lat and long of the two points
  (lat1, long1) = V_coord[e[0]]
  (lat2, long2) = V_coord[e[1]]
  # perform calculation
  res = math.sqrt( (lat2 - lat1)**2 + (long2 - long1)**2 )
  return res
  
def least_cost_path(G, start, dest, cost):
  """
  >>> costs = { (1,2): 2, (1, 3): 0.3, (3, 5): 1.5, (3, 4): 4, (4, 6): 0.8, (5, 6): 2 }
  >>> def cost(e): return costs[e] 
  >>> G = digraph.Digraph(set([(1,2), (1, 3), (3, 5), (3, 4), (4, 6), (5, 6)]))
  >>> least_cost_path(G, 1, 2, cost)
  [1, 2]
  >>> least_cost_path(G, 1, 6, cost)
  [1, 3, 5, 6]
  """
  todo = { start: 0 }
  visited = set()
  parent = {}
  while todo and (dest not in visited):
    (cur, c) = min(todo.items(), key=lambda x: x[1])
    visited.add(cur)
    todo.pop(cur)
    for n in G.adj_to(cur):
      if n in visited: continue
      if n not in todo or ( c + cost((cur, n)) < todo[n] ):
        todo[n] = c + cost((cur, n))
        parent[n] = cur
  next = dest
  res = [dest]
  while next != start:
    next = parent[next]
    res.append(next)
  res.reverse()
  if G.is_path(res):
    return res
  return None
  pass

#                       --- Server Implementation ---
print("done loading")
global debug
debug = False

args = parse_args()

# Initialize some stuff...
if args.serialport:
    print("Opening serial port: %s" % args.serialport)
    serial_out = serial_in =  serial.Serial(args.serialport, 9600)
else:
    print("No serial port.  Supply one with the -s port option")
    exit()

if args.verbose:
    debug = True
else:
    debug = False

idx = 0
while True:
    # read in serial data
    inp = receive(serial_in)
    while not inp:
      inp = receive(serial_in)
    
    if not (inp == "Starting..."):

      # process data
      inp = inp.rstrip() # strip trailing whitespace
      values = inp.split(" ") # get separate values
      # add decimals
      # lat:
      values[0] = values[0][:2] + "." + values[0][2:]
      values[2] = values[2][:2] + "." + values[2][2:]
      # long:
      values[1] = values[1][:4] + "." + values[1][4:]
      values[3] = values[3][:4] + "." + values[3][4:]
      # convert to floats
      for i in range(len(values)):
        values[i] = float(values[i])
      # get the codes for the intersections
      la1 = values[0]
      lo1 = values[1]
      la2 = values[2]
      lo2 = values[3]
      # find the vertex which is closet to  the input provided
      # initialize minimum dist with random coord
      minl1 = math.sqrt( (la1 - V_coord[354287616][0])**2 + (lo1 - V_coord[354287616][1])**2 )
      minl2 = math.sqrt( (la2 - V_coord[354287616][0])**2 + (lo2 - V_coord[354287616][1])**2 )
      minj1 = 354287616
      minj2 = 354287616
      for j in V_coord:
        # calculate dist from loc1 ***
        next1 = math.sqrt( (la1 - V_coord[j][0])**2 + (lo1 - V_coord[j][1])**2 )
        if next1 < minl1:
          minl1 = next1
          minj1 = j
        # calculate dist from loc2 ***
        next2 = math.sqrt( (la2 - V_coord[j][0])**2 + (lo2 - V_coord[j][1])**2 )
        if next2 < minl2:
          minl2 = next2
          minj2 = j
      # assign values
      loc1 = minj1
      loc2 = minj2
      # find least cost path
      lcp = least_cost_path(Dg, loc1, loc2, cost_distance)
      # sendprint results; convert to appropriate format
      l = len(lcp)
      s = str(l)
      send(serial_out, s)
      for e in lcp:
        mssg = str(V_coord[e][0]*100000) + " " + str(V_coord[e][1]*100000)
        send(serial_out, mssg)

if __name__ == "__main__":
  import doctest
  doctest.testmod()
      
