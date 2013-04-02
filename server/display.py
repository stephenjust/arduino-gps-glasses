"""
A module that takes graphs, G=(V, E) and attributes on
vertices and edges, and generates a .dot file for display
with graphviz.  

The type of graph to be rendered is normalyy just a undirected
graph.  To render a directed graph, set the keyword parameter
    graphtype='digraph' 
the default is graphtype='graph'

V is a set of vertices
E is a set of edges, in a 
    graph - the tuple (u, v) for u, v in V is an edge between 
        u and v.  The pair (v, u) is equivalent, and only one
        of the two possibilities should appear in E.
    digraph - the tuple (u, v) for u, v in V is a directed edge 
        from u to v.  It is different from the tuple (v, u) 
        which is the edge in the opposite direction.


attributes is an optional argument.  It is a dictionary of vertex and
edge attributes with these possible (optional) keys:
    vertex_color
    vertex_label
    edge_color
    edge_label
Each of these keys maps to a dictionary.
    atrributes['vertex_color'][v] if present is a string
        that gives the color of vertex v
    atrributes['vertex_label'][v] if present is a string
        that will be used as the label for vertex v.  If
        not present, the label for v is v.
    atrributes['edge_color'][(u,v)] if present is a string
        that gives the color of edge (u, v)
    atrributes['edge_label'][(e,v)] if present is a string
        that will be used as the label for egde (u,v).  If
        not present, the dge is unlabelled. 
    
"""
import time
import sys

def gen_dot_desc(G, graphtype='graph', attributes={}):
    """
    Given graph G, return a string that encodes the dot
    representation of G.

    >>> g = ({1, 2, 3}, {(1, 2), (1, 3)} )
    >>> s = gen_dot_desc(g)
 
    """
    if "vertex_color" in attributes:
        vertex_color=attributes["vertex_color"]
    else:
        vertex_color={ }

    if "edge_color" in attributes:
        edge_color=attributes["edge_color"]
    else:
        edge_color={ }

    if "vertex_label" in attributes:
        vertex_label=attributes["vertex_label"]
    else:
        vertex_label={ }

    if "edge_label" in attributes:
        edge_label=attributes["edge_label"]
    else:
        edge_label={ }

    (V, E) = G

    edgesym = "--"
    if graphtype != 'graph':
        graphtype = 'digraph'
        edgesym = "->"

    # generate the header
    dot_desc = ( graphtype + 
        " g {\n" + 
        "  ordering=out;\n" +
        "  node [shape=circle];\n" +
        "  edge [penwidth=3];\n" 
        )

    # now generate vertex and edges information
    if len(V) == 0:
       dot_desc += "Empty [shape=ellipse];\n"
    else:
        for n in V:
            color = "white"
            if n in vertex_color: 
                color = vertex_color[n]

            label = str(n)
            if n in vertex_label: 
                label = vertex_label[n]

            dot_desc += '  {v} [label="{l}", style=filled, fillcolor="{c}"];\n'.format(
                v=str(n), l=label, c=color)

        for e in E:
            (x, y) = e
            color = "black"
            if e in edge_color: 
                color = edge_color[e]
            label = ""
            if e in edge_label:
                label = ', label="{}"'.format(edge_label[e])

            dot_desc += '  {vx} {esym} {vy} [color="{c}" {l}];\n'.format(
                    esym=edgesym, vx=str(x), vy=str(y), c=color, l=label)


    # close off the description
    dot_desc += "}\n"

    return dot_desc

def write_dot_desc(G, file_name, graphtype='graph', attributes={}):
    """
    Given graph G, write the dot description of G
    into the given file name, which should have extension .dot
    as in sample-graph.dot

    """
    # Implementation note:
    # instead of f = open(file_name, 'w') inside a try block, use
    # the safe open that closes file on an exception, from
    # http://docs.python.org/3.2/tutorial/inputoutput.html

    with open(file_name, 'w') as f:
        f.write( gen_dot_desc(G, graphtype, attributes) )

def pause(time=1,prompt="next?"):
    """
    if time > 0, then pause for time sec
    if time=0, then print the prompt and wait for a new line of
        input from the terminal before returning.
    """
    if time > 0:
        time.sleep(time)
    else:
        x = input(prompt)
        
    

if __name__ == "__main__":
    import doctest
    doctest.testmod()

