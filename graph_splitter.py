#
# GraphSplitter.py
# 
# python graph_splitter.py --graph_file ./metis-5.1.0/graphs/4elt.graph --metis_bin ../metis_install/bin/gpmetis --core 4 --output .

from optparse import OptionParser
import os

def main(opts, args):
    """
    Main function of the script.

    """
    # Execute the command to run the metis and get the cluster split.
    os.system(opts.metis_bin + " " + opts.graph_file + " " + opts.cores)

    # Split File.
    split_file = opts.graph_file + ".part." + opts.cores
    
    # Read in split files.
    f = open(split_file, "r")
    split_file_lines = f.readlines()
    f.close()
    
    # Read the actual graph file.
    f = open(opts.graph_file, "r")
    graph_file_lines = f.readlines()
    f.close()

    # Create lists of list. Each list has all the vertices in the cluster.
    # Vertices are zero-indexed.
    split_arrays = []
    for i in range(int(opts.cores)):
        split_arrays.append([])
    for i, line in enumerate(split_file_lines):
        split_arrays[int(line.strip())].append(i)

    # Create split graphs, with new neighbors and all.
    vertex_count, edge_count = graph_file_lines[0].strip().split()
    split_graphs = []
    for i in range(int(opts.cores)):
        split_graphs.append([])
       
    for i, line in enumerate(graph_file_lines[1:]):
        cluster_no = int(split_file_lines[i].strip())

        # Neighbors are 1-indexed. Therefore, subtract one to be consistent.
        neighbors = map(lambda x: int(x) - 1, line.strip().split(" "))
        new_neighbors = []
        for n in neighbors:
            n_cluster = int(split_file_lines[n].strip())
            new_neighbors.append((n_cluster, n))
        split_graphs[cluster_no].append(new_neighbors)

    # 1) Now we iterate again and find all the neighbors for each MPI task.
    # 2) Once we find each neighbor, we assign them new ranks[0:..]
    # 3) Then we update the adjaceny lists with new rank indices, as those 
    # be passed by the MPI sends.
   
    for i, split in enumerate(split_graphs):
        split_neighbor_map = {}
        
        for adj in split:
            for neighbor in adj:
                cluster, vertex = neighbor
                if cluster in split_neighbor_map:
                    split_neighbor_map[cluster].append(vertex)
                else:
                    split_neighbor_map[cluster] = [vertex]

        for key in split_neighbor_map:
            x = set(split_neighbor_map[key])
            split_neighbor_map[key] = list(x)
            split_neighbor_map[key].sort()

        for key in split_neighbor_map:
            key_neighbor_map = {}
            for j, vertex in enumerate(split_neighbor_map[key]):
                key_neighbor_map[vertex] = j
            split_neighbor_map[key] = key_neighbor_map

        new_split = []
        for adj in split:
            new_adj = []
            for neighbor in adj:
                cluster, vertex = neighbor
                vertex = split_neighbor_map[cluster][vertex]
                new_adj.append((cluster, vertex))
            new_split.append(new_adj)

        split_graphs[i] = new_split
   
    for i, split in enumerate(split_graphs):
        file_name = os.path.basename(opts.graph_file)
        f = open(opts.output + "/" + file_name + ".split." + str(i), "w")
        for adj in split:
            adj_str = ",".join(map(lambda (x,y): str(x) + '-' + str(y), adj))
            f.write(adj_str)
            f.write("\n")
        f.close()

def getOpts():
    """
    Parse the command line options.

    """
    parser = OptionParser()
    parser.add_option("--graph_file", help="Graph file in the format accepted by Metis.")
    parser.add_option("--metis_bin", help="Location of the metis bin")
    parser.add_option("--cores", help="Number of mpi tasks that we are going to spawn.")
    parser.add_option("--output", help="Location of the output folder.")

    return parser.parse_args()

if __name__ == "__main__":
    opts, args = getOpts()
    main(opts, args) 
