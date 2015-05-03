# Converts an edge list to a normal directed graph.

# edgelist_2_adj_nonmetis.py <edgelist>
import sys

adj_list = []
vertex_neighbor_map = {}
edges = 0
with open(sys.argv[1], "r") as lines:
    for line in lines:
        if line.strip() == "":
            continue
        a, b = map(lambda x: int(x), line.split())
        if a == b:
            continue
        if a in vertex_neighbor_map:
            vertex_neighbor_map[a].append(b)
        else:
            vertex_neighbor_map[a] = [b]

nodes = int(sys.argv[2])
for i in range(nodes):
    if i in vertex_neighbor_map:
        x = vertex_neighbor_map[i]
        x.sort()
        adj_list.append(x)
    else:
        adj_list.append([])

f = open(sys.argv[1] + "_adjacency_nonmetis", "w")
for adj in adj_list:
    f.write(" ".join(map(lambda x: str(x+1), adj)))
    f.write("\n")
f.close()
