"""
Priority queue implementation as a binary heap

pop_smallest()
    remove the element with the lowest priority
    return tuple (key, priority)

update(key, priority)
    lower the priority of an element
    if priority is not lower do nothing,
    if key is not in priority queue, add it
    return True if priority of a key has changed or is added, otherwise False

is_empty()
    return True if nothing is in the priority queue, otherwise False

>>> q = PQueue()
>>> q.is_empty()
True
>>> q.update("thing", 1)
True
>>> q.is_empty()
False
>>> q.update("another thing", 5)
True
>>> q.pop_smallest()
('thing', 1)
>>> q.is_empty()
False
>>> q.update("thing", 3)
True
>>> q.update("another thing", 1)
True
>>> len(q)
2
>>> 'thing' in q
True
>>> 'nothing' in q
False
>>> q.pop_smallest()
('another thing', 1)
>>> q.pop_smallest()
('thing', 3)
>>> q.is_empty()
True

"""

def _parent(i):
    """
    >>> _parent(3)
    1
    >>> _parent(27)
    13
    """
    return (i-1)//2

def _children(i):
    """
    >>> _children(5)
    [11, 12]
    """
    return [ 2*i + 1, 2*i + 2 ]

class PQueue:
    def __init__(self):
        self._heap = []
        self._keyindex = {}

    def __len__(self):
        return len(self._heap)

    def __contains__(self, key):
        return key in self._keyindex

    def update(self, key, priority):
        if key in self._keyindex:
            i = self._keyindex[key]
            # Update existing entry, possibly lower its priority
            if priority > self._priority(i):
                return False
            else:
                self._heap[i] = (key, priority)
                self._heapify_up(i)
                return True
        else:
            # Create a new heap entry
            self._heap.append( (key, priority) )
            self._keyindex[key] = len(self._heap) - 1;
            self._heapify_up(len(self._heap) - 1)
            return True

    def pop_smallest(self):
        self._swap(0, len(self._heap) - 1)
        rv = self._heap.pop()
        del self._keyindex[rv[0]]
        self._heapify_down(0)
        return rv

    def is_empty(self):
        return len(self._heap) == 0

    def _swap(self, i, j):
        (self._heap[i], self._heap[j]) = (self._heap[j], self._heap[i])
        self._keyindex[self._heap[i][0]] = i
        self._keyindex[self._heap[j][0]] = j

    def _priority(self, i):
        return self._heap[i][1]

    def _heapify_down(self, i):
        children = [ c for c in _children(i) if c < len(self._heap) ]
        if not children: return

        # Find child with smallest priority
        minchild = min(children, key = self._priority)

        if self._priority(i) > self._priority(minchild):
            # Swap element i with its smallest child
            self._swap(i, minchild)

            self._heapify_down(minchild)

    def _heapify_up(self, i):
        if i == 0: return

        # compare i with its parent
        if self._priority(i) < self._priority(_parent(i)):
            self._swap(i, _parent(i))
            self._heapify_up(_parent(i)) 

if __name__ == "__main__":
    import doctest
    doctest.testmod()
