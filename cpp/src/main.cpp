#include <bits/stdc++.h>
#include "./robin_hood.hpp"

using namespace std;

using NodeId = int;
using NodeIndex = int;
using Distance = int;
using Edge = pair<NodeIndex, Distance>;

const int DISTANCE_MULTIPLE = 100;

bool is_debug = false;

struct G {
    robin_hood::unordered_map <NodeId, NodeIndex> id2idx;
    vector <NodeId> idx2id = {0};
    NodeIndex idx = 1;
    vector <vector<Edge>> edge = {vector<Edge>()};
} g;

NodeIndex get_idx(NodeId id) {
    NodeIndex i = g.id2idx[id];
    if (i == 0) {
        i = g.idx++;
        g.id2idx[id] = i;
        g.idx2id.push_back(id);
        g.edge.push_back(vector<Edge>());
    }
    return i;
}


void add_edge(NodeId start, NodeId end, Distance distance) {
    NodeIndex s = get_idx(start);
    NodeIndex e = get_idx(end);
    g.edge[s].push_back({e, distance});
}

// 123.4567 --> 12345
int stof100(const char *s) {
    int result = 0;
    int place = 0;
    for (; *s != '\0'; s++) {
        if (*s == '.') {
            place = 1;
            continue;
        }
        result *= 10;
        result += *s - '0';
        if (place > 0) {
            place++;
            if (place >= 3) {
                break;
            }
        }
    }
    while (place < 3) {
        result *= 10;
        place++;
    }
    return result;
}

//XCoord,YCoord,START_NODE,END_NODE,EDGE,LENGTH
//0     ,1     ,2         ,3       ,4   ,5
//360665.2989,3931565.547,1,2,1,113.876845
auto parseLine(string_view v) {
    auto const first = v.find(',');
    auto const second = v.find(',', first + 1);
    auto const third = v.find(',', second + 1);
    auto const fourth = v.find(',', third + 1);
    auto const fifth = v.find(',', fourth + 1);

    auto s = 0;
    auto e = 0;
    auto d = 0.0f;
    auto const p = v.data();
    from_chars(p + second + 1, p + third, s);
    from_chars(p + third + 1, p + fourth, e);
    d = stof100(p + fifth + 1);
    return tuple{s, e, d};
}

void load() {
    string line;
    cin >> line; // skip header

    while (cin >> line) {
        auto const[s, e, d] = parseLine(line);
        if (is_debug) cout << "line: " << line << " s: " << s << " e: " << e << " D: " << d << endl;
        // cerr << "line:" << line << "s:" << s << " e:" << e << " d:" << d << endl;
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        add_edge(s, e, (int) d);
    }
}

using Visit = pair<Distance, NodeIndex>;

pair <Distance, vector<NodeId>> dijkstra(NodeId start, NodeId end) {
    NodeIndex s = get_idx(start);
    NodeIndex e = get_idx(end);

    int size = g.edge.size();
    Distance d[size] = {};
    NodeId prev[size] = {};

    priority_queue <Visit, vector<Visit>, greater<Visit>> queue;
    queue.push({0, s});

    int visited = 0;
    while (!queue.empty()) {
        auto const[distance, here] = queue.top();
        queue.pop();
        if (is_debug) cout << "visiting: " << here << " distance: " << distance << endl;
        visited++;
        for (auto const[to, toFromHere] : g.edge[here]) {
            auto const w = distance + toFromHere;
            auto const dTo = d[to];
            if (dTo == 0 || w < dTo) {
                prev[to] = here;
                d[to] = w;
                queue.push({w, to});
            }
        }
    }

    cerr << "visited: " << visited << endl;

    vector <NodeId> result;
    NodeIndex n = e;
    result.push_back(g.idx2id[n]);

    while (d[n] != 0 && n != s && n != 0) {
        n = prev[n];
        result.push_back(g.idx2id[n]);
    }

    return {d[end] / DISTANCE_MULTIPLE, result};

}

//- original
// Benchmark #1: ./bench.sh 20 < ../data/Tokyo_Edgelist.csv
//  Time (mean ± σ):      2.974 s ±  0.135 s    [User: 2.932 s, System: 0.041 s]
//  Range (min … max):    2.835 s …  3.216 s    10 runs
//- after parseLine
// Benchmark #1: ./bench.sh 20 < ../data/Tokyo_Edgelist.csv
//  Time (mean ± σ):      2.726 s ±  0.022 s    [User: 2.695 s, System: 0.031 s]
//  Range (min … max):    2.691 s …  2.760 s    10 runs
//- after unordered_map
// Benchmark #1: ./bench.sh 20 < ../data/Tokyo_Edgelist.csv
//  Time (mean ± σ):      2.606 s ±  0.030 s    [User: 2.579 s, System: 0.027 s]
//  Range (min … max):    2.572 s …  2.666 s    10 runs
//- after robin_hood
// Benchmark #1: ./bench.sh 20 < ../data/Tokyo_Edgelist.csv
//  Time (mean ± σ):      2.392 s ±  0.029 s    [User: 2.360 s, System: 0.032 s]
//  Range (min … max):    2.363 s …  2.462 s    10 runs
int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int count = atoi(argv[1]);
    is_debug = argc > 2 && string(argv[2]) == "debug";

    load();
    cerr << "loaded nodes: " << g.idx << endl;

    pair <Distance, vector<NodeId>> result;
    for (int i = 0; i < count; i++) {
        NodeId s = g.idx2id[(i + 1) * 1000];
        result = dijkstra(s, g.idx2id[1]);
        cout << "distance: " << result.first << endl;
    }

    cout << "route: ";
    for (NodeId id: result.second) {
        cout << id << " ";
    }
    cout << endl;
}
