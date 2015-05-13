#
# GraphSplitter.py
# 
# python graph_splitter_directed.py --graph_file ./metis-5.1.0/graphs/4elt.graph --metis_bin ../metis_install/bin/gpmetis --core 4 --output .

from optparse import OptionParser
import os

def main(opts, args):
    """
    Main function of the script.

    """
    # Execute the command to run the metis and get the cluster split.
    #os.system(opts.metis_bin + " " + opts.graph_file + " " + opts.cores)

    # Split File.
    split_file = opts.graph_file + ".part." + opts.cores
    
    # Read in split files.
    f = open(split_file, "r")
    split_file_lines = f.readlines()
    f.close()

    split_file_lines = map(lambda x: int(x.strip()), split_file_lines)

    # Create the split of the graph.
    # Basically, for every node, we want the list of outclusters, where we would like to dispatch updates.
    # And another thing, we store is 
    split_out_clusters = []
    split_out_degree = []
    in_nodes_map = {}
    cluster_node_map = []
    for i in range(int(opts.cores)):
        split_out_clusters.append([])
        split_out_degree.append([])
        cluster_node_map.append({})

    # Read the actual graph file.
    cluster_2_cluster_count_map = {}
    with open(opts.graph_file + "_nonmetis", "r") as graph_file_lines:
        for i, line in enumerate(graph_file_lines):
            cluster_no = split_file_lines[i]
            
            # Neighbors are 1-indexed. Therefore, subtract one to be consistent.
            neighbors = map(lambda x: int(x) - 1, line.strip().split())
            out_clusters = {}
            
            unique_n_clusters = {}
            for n in neighbors:
                n_cluster = split_file_lines[n]
                unique_n_clusters[n_cluster] = True

            for n_cluster in unique_n_clusters.keys():
                if cluster_no in cluster_2_cluster_count_map:
                    if n_cluster in cluster_2_cluster_count_map[cluster_no]:
                        cluster_2_cluster_count_map[cluster_no][n_cluster] += 1
                    else:
                        # Keeping count also zero based, as we are not interested in 
                        # counts, but the index of the node.
                        cluster_2_cluster_count_map[cluster_no][n_cluster] = 0
                else:
                    cluster_2_cluster_count_map[cluster_no] = {n_cluster : 0}

            for n in neighbors:
                n_cluster = split_file_lines[n]
                if n in in_nodes_map:
                    in_nodes_map[n].append((cluster_no, cluster_2_cluster_count_map[cluster_no][n_cluster]))
                else:
                    in_nodes_map[n] = [(cluster_no, cluster_2_cluster_count_map[cluster_no][n_cluster])]
               
                out_clusters[n_cluster] = True

            # Again zero-based.
            cluster_node_map[cluster_no][len(split_out_degree[cluster_no])] = i
            split_out_clusters[cluster_no].append(out_clusters.keys())
            split_out_degree[cluster_no].append(len(neighbors))
 
    for cluster_no, out_degree in enumerate(split_out_degree):
        file_name = os.path.basename(opts.graph_file)
        f = open(opts.output + "/" + file_name + ".split." + str(cluster_no), "w")
        # Format of writing out details for each node.
        # OutDegree#out_core1,out_core2#in_core1-in_index1,in_core2-in_index2,...
        node_str = ""
        for i, out in enumerate(out_degree):
            node_str = str(out) + "#" + ",".join(map(lambda x: str(x), split_out_clusters[cluster_no][i])) + "#"
            n = cluster_node_map[cluster_no][i]
            if n in in_nodes_map:
                node_str += ",".join(map(lambda (x,y): str(x) + '-' + str(y), in_nodes_map[n]))
            f.write(node_str)
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
