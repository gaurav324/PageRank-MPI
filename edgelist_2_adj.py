# Converts an edge list to metis-ified undirected graph.

# edgelist_2_adj.py <edgelist>
import sys

adj_list = []
vertex_neighbor_map = {}
edges = 0
with open(sys.argv[1], "r") as lines:
    for line in lines:
        a, b = map(lambda x: int(x), line.split())
        if a in vertex_neighbor_map:
            vertex_neighbor_map[a].append(b)
        else:
            vertex_neighbor_map[a] = [b]
        
        if b in vertex_neighbor_map:
            vertex_neighbor_map[b].append(a)
        else:
            vertex_neighbor_map[b] = [a]

nodes = len(vertex_neighbor_map.keys())
for i in range(nodes):
    x = set(vertex_neighbor_map[i])
    x = list(x)
    x.sort()
    adj_list.append(x)

f = open(sys.argv[1] + "_adjacency", "w")
for adj in adj_list:
    f.write(" ".join(map(lambda x: str(x+1), adj)))
    f.write("\n")
f.close()
