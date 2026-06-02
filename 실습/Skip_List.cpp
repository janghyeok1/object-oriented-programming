#include <iostream>
#include <climits>
#include <string>
#include <sstream>
#include <vector>

class SkipList {
    struct Node {
        int key;
        Node *up = nullptr, *down = nullptr, *left = nullptr, *right = nullptr;
        explicit Node(int k) : key(k) {}
    };

    Node* topLeft;
    Node* topRight;

    static constexpr int NEG_INF = INT_MIN;
    static constexpr int POS_INF = INT_MAX;

public:
    SkipList();
    ~SkipList();
    void insert(int key);
    bool search(int key) const;
    void print() const;
private:
    void buildInitial();
    Node* findPredecessor(int key) const;
};

SkipList::SkipList() {
    topLeft  = new Node(NEG_INF);
    topRight = new Node(POS_INF);
    topLeft->right = topRight;
    topRight->left = topLeft;
    buildInitial();
}

SkipList::~SkipList() {
    Node* row = topLeft;
    while (row) {
        Node* nextRow = row->down;
        Node* p = row;
        while (p) {
            Node* nxt = p->right;
            delete p;
            p = nxt;
        }
        row = nextRow;
    }
}

void SkipList::buildInitial() {
    auto linkH = [](Node* a, Node* b) { a->right = b; b->left = a; };
    auto linkV = [](Node* u, Node* d) { u->down = d; d->up   = u; };

    Node* L0head = topLeft;
    Node* L0tail = topRight;
    Node* n0[10];
    Node* cur = L0head;
    for (int i = 0; i < 10; ++i) {
        n0[i] = new Node(i * 10);
        linkH(cur, n0[i]);
        cur = n0[i];
    }
    linkH(cur, L0tail);

    Node* L1head = new Node(NEG_INF);
    Node* L1tail = new Node(POS_INF);
    linkV(L1head, L0head);
    linkV(L1tail, L0tail);

    const int p1[] = {1, 3, 5, 7};
    Node* n1[4];
    cur = L1head;
    for (int i = 0; i < 4; ++i) {
        n1[i] = new Node(p1[i] * 10);
        linkH(cur, n1[i]);
        linkV(n1[i], n0[p1[i]]);
        cur = n1[i];
    }
    linkH(cur, L1tail);
    topLeft  = L1head;
    topRight = L1tail;

    Node* L2head = new Node(NEG_INF);
    Node* L2tail = new Node(POS_INF);
    linkV(L2head, L1head);
    linkV(L2tail, L1tail);

    Node* n2 = new Node(30);
    linkH(L2head, n2);
    linkH(n2, L2tail);
    linkV(n2, n1[1]);

    topLeft  = L2head;
    topRight = L2tail;
}

SkipList::Node* SkipList::findPredecessor(int key) const {
    Node* cur = topLeft;
    while (true) {
        while (cur->right->key <= key) cur = cur->right;
        if (cur->down) cur = cur->down;
        else break;
    }
    return cur;
}

bool SkipList::search(int key) const {
    return findPredecessor(key)->key == key;
}

void SkipList::insert(int key) {
    Node* pred = findPredecessor(key);
    if (pred->key == key) {
        cout << "중복" << endl;
        return;
    }
    Node* nn = new Node(key);
    nn->left  = pred;
    nn->right = pred->right;
    pred->right->left = nn;
    pred->right = nn;

    Node* cur = nn;
    while (true) {
        Node* p = cur->left;
        while (!p->up && p->left)  p = p->left;
        Node* q = cur->right;
        while (!q->up && q->right) q = q->right;
        int count = 0;
        Node* mid = nullptr;
        for (Node* it = p->right; it != q; it = it->right) {
            ++count;
            if (count == 2) mid = it;
        }
        if (count <= 2) break;
        Node* upMid = new Node(mid->key);
        upMid->down = mid;
        mid->up = upMid;

        if (!p->up && !q->up) {
            Node* nl = new Node(NEG_INF);
            Node* nr = new Node(POS_INF);
            nl->right = upMid;  upMid->left = nl;
            upMid->right = nr;  nr->left = upMid;

            nl->down = topLeft;  topLeft->up  = nl;
            nr->down = topRight; topRight->up = nr;

            topLeft  = nl;
            topRight = nr;
            break;
        }
        Node* pu = p->up;
        Node* qu = q->up;
        upMid->left  = pu;
        upMid->right = qu;
        pu->right = upMid;
        qu->left  = upMid;

        cur = upMid;
    }
}
void SkipList::print() const {
    Node* bottom = topLeft;
    while (bottom->down) bottom = bottom->down;

    std::vector<int> keys;
    for (Node* p = bottom->right; p->right != nullptr; p = p->right)
        keys.push_back(p->key);

    int W = 0;
    for (int k : keys) {
        int w = (int)std::to_string(k).size();
        if (w > W) W = w;
    }
    W += 2;

    int n = 0;
    for (Node* p = topLeft; p; p = p->down) ++n;

    Node* row = topLeft;
    int lvl = n - 1;
    while (row) {
        std::vector<bool> present(keys.size(), false);
        for (Node* p = row->right; p->right != nullptr; p = p->right) {
            for (size_t i = 0; i < keys.size(); ++i)
                if (keys[i] == p->key) { present[i] = true; break; }
        }

        std::string label = "L" + std::to_string(lvl) + ":";
        std::string pad(label.size(), ' ');

        std::cout << label;
        for (size_t i = 0; i < keys.size(); ++i) {
            if (present[i]) {
                std::string s = std::to_string(keys[i]);
                std::cout << std::string(W - s.size(), ' ') << s;
            } else {
                std::cout << std::string(W, ' ');
            }
        }
        std::cout << '\n';

        if (row->down) {
            std::cout << pad;
            for (size_t i = 0; i < keys.size(); ++i) {
                if (present[i])
                    std::cout << std::string(W - 1, ' ') << '|';
                else
                    std::cout << std::string(W, ' ');
            }
            std::cout << '\n';
        }

        row = row->down;
        --lvl;
    }
}
int main() {
    SkipList sl;
    sl.print();
    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;

        std::istringstream iss(line);
        std::string cmd;
        if (!(iss >> cmd)) continue;

        if (cmd == "insert") {
            int v;
            if (!(iss >> v)) { std::cout << "사용법: insert N\n"; continue; }
            sl.insert(v);
            std::cout << "[insert " << v << "]\n";
            sl.print();
        }
        else if (cmd == "search") {
            int v;
            if (!(iss >> v)) { std::cout << "사용법: search N\n"; continue; }
            std::cout << "search(" << v << ") = "
                      << (sl.search(v) ? "Found" : "Not found") << '\n';
        }
        else {
            std::cout << "알 수 없는 명령: '" << cmd << "'\n";
        }
    }
    return 0;
}
