/**
 * This is faster cpp version of the graph splitter.
 */

// Arguments graph_file(1), nodes(2), metis_bin(3), cores(4), output_dir(5)
#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<map>

using namespace std;
int main(int argc, char *argv[]) {
    stringstream ss;
    ss << argv[3] << " " << argv[1] << " " << argv[4];
    //system(ss.str().c_str()); 
   
    // Read in split file.
    ss.str("");
    ss.clear();
    ss << argv[1] <<  ".part." << argv[4];
    ifstream split_file(ss.str().c_str());
    vector<int> split_file_lines;
    split_file_lines.reserve(atol(argv[2]));
    string line;

    int nodes = atoi(argv[2]);
    int cores = atoi(argv[4]);
    vector<int> core_length(cores, 0);
    int i=0;
    while (getline(split_file, line)) {
        int split_no = atoi(line.c_str());
        split_file_lines[i++] = split_no;
        core_length[split_no] += 1;
    }
    
    // Allocate enough memory for all the intermediate 
    // data structures.
    vector<vector<pair<int, int> > > in_nodes_map(nodes);
    vector<vector<int> >split_out_degree(cores);
    vector<vector<vector<int> > >split_out_cluster(cores);
    vector<vector<int> > cluster_node_map(cores);
    for (int i=0; i < cores; ++i) {
        split_out_degree[i].reserve(core_length[i]);    
        split_out_cluster[i].reserve(core_length[i]);
        cluster_node_map[i].reserve(core_length[i]);
    }

    // Read in the actual graph file.
    ss.str("");
    ss.clear();
    ss << argv[1] << "_nonmetis";
    ifstream graph_file(ss.str().c_str());
    vector<vector<int> >cluster_2_cluster_count(cores);
    for (int i = 0; i < cores; ++i) {
        cluster_2_cluster_count[i].reserve(cores);
        for (int j=0; j < cores; ++j) {
            cluster_2_cluster_count[i][j] = -1;
        }
    }

    vector<int> cluster_indices(cores, 0);
    i = 0;
    while (getline(graph_file, line)) {
        int cluster_no = split_file_lines[i];

        vector<int> neighbors;
        std::stringstream ss1(line);
        std::string item1 = "";
        while (std::getline(ss1, item1, ' ')) {
            int n = atoi(item1.c_str()) - 1;
            neighbors.push_back(n);
        }

        std::map<int, bool> out_clusters;
        for (auto n : neighbors) {
            int n_cluster = split_file_lines[n];
            out_clusters[n_cluster] = true;
        }

        for (auto& n_cluster : out_clusters) {
            cluster_2_cluster_count[cluster_no][n_cluster.first] += 1;
        }

        for (auto n : neighbors) {
            int n_cluster = split_file_lines[n];
            in_nodes_map[n].push_back(pair<int, int>(cluster_no, cluster_2_cluster_count[cluster_no][n_cluster]));
        }

        cluster_node_map[cluster_no][cluster_indices[cluster_no]] = i;
        for (auto& n_cluster : out_clusters) {
            split_out_cluster[cluster_no][cluster_indices[cluster_no]].push_back(n_cluster.first);
        }

        split_out_degree[cluster_no][cluster_indices[cluster_no]] = neighbors.size();
        cluster_indices[cluster_no] += 1;
        ++i;
    }

    std::string fullpath(argv[1]);
    int last_index = fullpath.rfind('/');
    string filename = fullpath.substr(last_index + 1, fullpath.length() - 1);
    for (int split_no=0; split_no < cores; ++split_no) {
        ss.str("");
        ss.clear();
        ss << argv[5] << filename << ".split." << split_no;
        string outfile = ss.str();
        cout << "Writing to " << outfile << endl;
        ofstream split_outfile(outfile);
           
        string newline = "\n";
        for (int j=0; j < cluster_indices[split_no]; ++j) {
            ss.str("");
            ss.clear();
            ss << split_out_degree[split_no][j] << "#";
            for (int k=0; k < split_out_cluster[split_no][j].size(); ++k) {
                ss << split_out_cluster[split_no][j][k];
                if (k != split_out_cluster[split_no][j].size() - 1) {
                    ss << ",";
                }
            }
            ss << "#";
            int n = cluster_node_map[split_no][j];
            for (int k=0; k < in_nodes_map[n].size(); ++k) {
                pair<int, int>& in_node = in_nodes_map[n][k];
                ss << in_node.first << "-" << in_node.second;
                if (k != in_nodes_map[n].size() - 1) {
                    ss << ",";
                }
            }
            
            string buffer = ss.str();
            split_outfile.write(buffer.c_str(), buffer.length());
            split_outfile.write(newline.c_str(), newline.length());
        }

        split_outfile.close();
    }
}
