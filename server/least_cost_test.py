import digraph
import pdb

"""
This function will determine the least cost path given a graph, a specified starting point, and ending point
"""

costs = { (1,2): 2, (1, 3): 0.3, (3, 5): 1.5, (3, 4): 4, (4, 6): 0.8, (5, 6): 2 }

def cost(e):
  return costs[e]
  pass

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
  """
  >>> G = digraph.Digraph(set([(1,2), (1, 3), (3, 5), (3, 4), (4, 6), (5, 6)]))
  >>> least_cost_path(G, 1, 2, cost)
  [1, 2]
  >>> least_cost_path(G, 1, 6, cost)
  [1, 3, 5, 6]
  """
  todo = { start: 0 }
  visited = set()
  parent = {}
  #pdb.set_trace()
  while todo and (dest not in visited):
    (cur, c) = min(todo.items(), key=lambda x: x[1])
    visited.add(cur)
    todo.pop(cur)
    for n in G.adj_to(cur):
      if n in visited: continue
      if n not in todo or ( c + cost((cur, n)) < todo[n] ):
        todo[n] = c + cost((cur, n))
        parent[n] = cur
    #pdb.set_trace()
  #pdb.set_trace()
  next = dest
  res = [dest]
  while next != start:
    next = parent[next]
    res.append(next)
    #pdb.set_trace()
  res.reverse()
  #pdb.set_trace()
  if G.is_path(res):
    return res
  return None
  pass
  
if __name__ == "__main__":
  import doctest
  doctest.testmod()