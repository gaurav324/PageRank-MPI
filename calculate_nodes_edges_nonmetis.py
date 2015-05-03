import sys

nodes = 0
edges = 0
with open(sys.argv[1], "r") as lines:
    for line in lines:
        nodes += 1
        edges += len(line.split())

print "Nodes: ", nodes
print "Edges: ", edges
