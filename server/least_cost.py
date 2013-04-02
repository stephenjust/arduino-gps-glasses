import digraph
import math
import pdb

"""
This function will determine the least cost path given a graph, a specified starting point, and ending point
"""

def cost_distance(e):
  # look up the lat and long of the two points
  (lat1, long1) = V_coord[e[0]]
  (lat2, long2) = V_coord[e[1]]
  # perform calculation
  return math.sqrt( (lat2 - lat1)**2 + (long2 - long1)**2 )

def get_smallest(dict):
  label = 1
  min = dict[label]
  for thing in dict:
    if dict[thing] < min: 
      min = dict[thing]
      label = thing
  return (label, min)
  pass
  
def least_cost_path(G, start, dest, cost):
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
  
if __name__ == "__main__":
  import doctest
  doctest.testmod()