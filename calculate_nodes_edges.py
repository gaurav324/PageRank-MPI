import sys

nodes = 0
edge_map = {}
with open(sys.argv[1], "r") as lines:
    for line in lines:
        nodes += 1
        neighbors = map(lambda x: int(x), line.split())
        for n in neighbors:
            a = min(nodes, n)
            b = max(nodes, n)
            if a not in edge_map:
                edge_map[a] = {b : True}
            else:
                edge_map[a][b] = True
        
print "Nodes: ", nodes
edges = 0
for a in edge_map.keys():
    edges += len(edge_map[a].keys())
print "Edges: ", edges
