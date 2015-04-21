#
# GraphSplitter.py
# 


from optparse import OptionParser

import os

def main(opts, args):
    """
    Main function of the script.

    """
    # Execute the command to run the metis and get the cluster split.
    os.command(opts.metis_bin + " " + opts.graph_file + " " + opts.cores)

    # Split File.
    split_file = opts.graph_file + "." + opts.cores  + ".parts"
    
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

    # Create split graphs, with new edges and all.
    vertex_count, edge_count = graph_file_lines[0].strip()
    split_graphs = []
    for i in range(int(opts.cores)):
        split_graphs.append([])
    for i, line in enumerate(graph_file_lines[1:]):
        cluster_no = int(split_file_lines[i].strip())

        # Edges are 1-indexed. Therefore, subtract one to be consistent.
        edges = map(lambda x: int(x) - 1, line.strip().split(" "))

        # Get the new indices, after the cluster splits.
        new_edge_labels = []
        for neighbor in edges:
            edge_cluster_no = int(split_file_lines[neighbor].strip())
            split_arrays[edge_cluster_no].rfind(neighbor)
        edges = map(lambda x: split_arrays[cluster_no], edges)
        split_graphs[cluster_no].append(edges)
        

def getOpts():
    """
    Parse the command line options.

    """
    parser = OptionParser()
    parser.add_option("--graph_file", help="Graph file in the format accepted by Metis.")
    parser.add_option("--metis_bin", help="Location of the metis bin")
    parser.add_option("--cores", help="Number of mpi tasks that we are going to spawn.")
    pasrer.add_option("--output", help="Location of the output folder.")

    return parser.parse_args()

if __name__ == "__main__":
    opts, args = getOpts()
    main(opts, args) 
