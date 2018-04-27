#!/usr/bin/python

import random
import sys
from collections import deque

num_nodes = int(sys.argv[1])
branching_factor = int(sys.argv[2])
print >>sys.stderr, "Num nodes =",  num_nodes
print >>sys.stderr, "Branching factor =", branching_factor 

ctr = -1
def gensym():
    global ctr
    ctr += 1
    return ctr


def random_num_children():
    return branching_factor + (random.random() * branching_factor / 2 - branching_factor)

q = deque([gensym()])
while len(q):
    tos = q.pop()
    num_children = random_num_children()
    num_children = max(int(num_children), 1)

    for child in range(num_children):
        if ctr >= num_nodes:
            break
        c = gensym()
        print tos, c
        q.appendleft(c)
