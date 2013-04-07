class Map:
    """

    >>> m = Map("test.map")

    >>> p = m.get_path( (0.0, 0.0), (0.0, 0.0) )
    >>> p == [(0.0, 0.0)]
    True
    """
    def __init__(self, file):
        pass

    def get_path(self, start_coord, stop_coord):
        return []

if __name__ == "__main__":
    import doctest
    doctest.testmod()
