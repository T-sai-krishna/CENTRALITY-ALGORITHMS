#include <bits/stdc++.h>
using namespace std;
#include <fstream>
const int inf = 100000000;
// To store the changes in the graph.(DN)
class node // new edge
{
public:
    int from;
    int to;
    // int weight;
    // node(int from, int to,) : from(from), to(to), weight(weight) {}
};

class graph
{

    int v;
    vector<double> Cc;       //  Closeness centrality values of nodes.
    vector<vector<int>> d;   // Shortest distance matrix
    vector<vector<int>> psi; // One of the out-neighbours of s that lies in the shortest path from s to t
    vector<vector<int>> AN;  // Adjacency matrix of the graph.
    map<pair<int, int>, int> dd;
    // To store the difference in shortest distances between two vertices s and t.
    // Initialisation of convergence step (C).

public:
    graph(int v) : v(v)
    {
        Cc.resize(v, 0);
        d.resize(v, vector<int>(v, inf));
        psi.resize(v, vector<int>(v, -1));
        AN.resize(v, vector<int>(v, inf));
        for (int i = 0; i < v; i++)
        {
            d[i][i] = 0;
            psi[i][i] = i;
            AN[i][i] = 0;
        }
        // G.AN = PHI.
    }

    void convergence(vector<node> &DN)
    {
        dd.clear();
        vector<pair<int, int>> FutureActive;
        vector<pair<int, int>> CurrentActive;
        // APPLY CHANGES IN AN
        for (int i = 0; i < DN.size(); i++)
        {
            int s = DN[i].from;
            int t = DN[i].to;
            // int w = DN[i].weight;
            AN[s][t] = 1;
        }

        for (int i = 0; i < DN.size(); i++)
        {
            int s = DN[i].from;
            int t = DN[i].to;
            // int w = DN[i].weight;
            psi[s][t] = t;
            dd[{s, t}] = dd[{s, t}] + 1 - d[s][t];
            d[s][t] = 1;
            FutureActive.push_back({s, t});
        }

        while (!FutureActive.empty())
        {
            CurrentActive = FutureActive;
            FutureActive.clear();
            for (int i = 0; i < CurrentActive.size(); i++)
            {
                int s = CurrentActive[i].first;
                int t = CurrentActive[i].second;
                for (int s_ = 0; s_ < v; s_++)
                {
                    if (AN[s_][s] == inf)
                        continue;
                    if (Update_Clo(s_, t, s))
                    {
                        FutureActive.push_back({s_, t});
                    }
                }
            }
        }
    }

    // CHG indicates true.
    // UNCHG indicates false.
    bool Update_Clo(int s, int t, int v)
    {
        int d_new = AN[s][v] + d[v][t];
        if (d_new < d[s][t])
        {
            psi[s][t] = v;
            dd[{s, t}] = dd[{s, t}] + d_new - d[s][t];
            d[s][t] = d_new;
            return true;
        }
        return false;
    }

    void Aggregation_Clo()
    {
        for (auto it = dd.begin(); it != dd.end(); it++)
        {
            int s = it->first.first;
            int t = it->first.second;
            int d_change = it->second;
            if (Cc[s] == 0)
            {
                Cc[s] = 1.0 / (double)d[s][t];
            }
            else
            {
                Cc[s] += ((double)d_change) / (double(d[s][t] * (d_change - d[s][t])));
            }
        }
    }

    vector<double> Closeness(vector<node> &DN)
    {
        // for(auto i:DN)cout<<i.from<<"->"<<i.to<<" ";
        // cout<<endl;
        convergence(DN);
        // for(int ii=0;ii<v;ii++)
        // {
        //     for(int j=0;j<v;j++)cout<<d[ii][j]<<" ";
        //     cout<<endl;
        // }
        Aggregation_Clo();
        // for(auto i:Cc)cout<<i<<" ";
        // cout<<endl;
        return Cc;
    }
};

// Updating closeness centrality values of a graph in CSR format.
int main()
{
    int v; // number of vertices int the graph
    cin >> v;

    ifstream inputFile;
    inputFile.open("input.csv");

    ofstream outputFile;
    outputFile.open("output.csv");
    outputFile << "graph_no"
               << ",";
    for (int i = 0; i < v; i++)
    {
        outputFile << "Cc[" << i << "]" << ',';
    }
    outputFile << endl;

    string line;
    getline(inputFile, line);

    stringstream s(line);
    string wordn;
    getline(s, wordn, ',');
    int n; // number of graphs with out root(emptygraph)
    n = stoi(wordn);

    vector<graph *> all_g(n + 1); // store pointers to all graphs
    for (int i = 0; i <= n; i++)
    {
        all_g[i] = new graph(v);
    }

    do
    {
        int init_g, final_g;
        stringstream s(line);
        string word;
        getline(s, word, ',');
        init_g = stoi(word);
        getline(s, word, ',');
        final_g = stoi(word);
        outputFile << final_g << ',';

        (*all_g[final_g]) = (*all_g[init_g]); // uses default copy assignment

        vector<node> data_row;
        while (getline(s, word, ','))
        {
            int edge = stoi(word);
            node dup1, dup2;
            dup1.from = edge / v;
            dup1.to = edge % v;
            data_row.push_back(dup1);
            dup2.from = edge % v;
            dup2.to = edge / v;
            data_row.push_back(dup2);
        }

        vector<double> cc = (*all_g[final_g]).Closeness(data_row);

        for (auto c : cc)
        {
            outputFile << c << ',';
        }
        outputFile << endl;
    } while (getline(inputFile, line));
}