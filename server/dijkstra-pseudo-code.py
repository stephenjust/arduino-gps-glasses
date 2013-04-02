"""
Pseudo code (ok, Python code) for Dijkstra's algorithm
"""

def least_cost_path(G, start, dest, cost):
    """
    path = least_cost_path(G, start, dest, cost)

    least_cost_path returns a least cost path in the digraph G from vertex
    start to vertex dest, where costs are defined by the cost function.
    cost should be a function that takes a single edge argument and returns
    a real-valued cost.

    if there is no path, then returns None

    the path from start to start is [start]

    """

    # todo[v] is the current best estimate of cost to get from start to v 
    todo = { start: 0}

    # v in visited when the vertex v's least cost from start has been determined
    visited = set()

    # parent[v] is the vertex that just precedes v in the path from start to v
    parent = {}

    while todo and (dest not in visited):

        # priority queue operation
        # remove smallest estimated cost vertex from todo list
        # this is not the efficient heap form, but it works
        # because it mins on the cost (2nd) field of the tuple of
        # items from the todo dictionary

        (cur,c) = min(todo.items(), key=lambda x: x[1])
        todo.pop(cur)

        # it is now visited, and will never have a smaller cost
        visited.add(cur)

        for n in G.adj_to(cur):
            if n in visited: continue
            if n no in todo or ( c + cost((cur,n)) < todo[n] ):
                todo[n] = c + cost((cur,n))
                parent[n] = cur

    # now, if there is a path, extract it.  The graph may be disconnected
    # so in that case return None

    return None

if __name__ == "__main__":
    import doctest
    doctest.testmod()
