#include<iostream>
#include<vector>

using namespace std;

//Prim's algorithm���� 3���� vertex set
#define TREE 0 // tree�� ���ϴ� vertex set
#define FRINGE 1 // tree�� �ƴ����� tree�� vertex�� ������ vertex set
#define UNSEEN 2 // �� ���� vertex set

// �ִ� ������ ����, �ִ� ���
#define MAX 10001

// �������� ���� ������ ��Ÿ���� class
class vertex {
public:
	int num; // ������ ��ȣ
	int state; // vertex�� ��� ���������� �����ϴ� ����(TREE, FRINGE, UNSEEN)

	// prim's algorithm���� ��������� mst���� �ʿ��� ���� 2��
	int cost; // prim's algorithm �� ��������� mst���� edge�� cost�� vertex�� ������ ��
	vertex* parent; // � tree vertex�� ���� FRINGE�� �Ǿ��� �� �����ϴ� ����

	vertex(int num) {
		this->num = num; // ������ ��ȣ �ʱ�ȭ
		this->state = UNSEEN; // �⺻ ���´� UNSEEN�̴�.
		parent = NULL; // parent �ʱ�ȭ
		cost = 0; // ��� �ʱ�ȭ
	}
};

// �������� �����ϴ� ��å��
class edge {
public:
	vertex* src; // ��å�� ���� �� ����� ������
	vertex* dst; // ��å�� �ݴ��� ����� ������
	int cost; // ��å�� ������ ���

	// ������ ������ �����ڿ��� �ʱ�ȭ
	edge(vertex* src, vertex* dst, int cost) {
		this->src = src;
		this->dst = dst;
		this->cost = cost;
	}
};

vector<edge*>graph[MAX]; // ��������Ʈ ��� �׷���, �� ������(vertex) ��ȣ�� ���� incident�� edge�� list(vector)�� ����ȴ�.
vector<vertex*>vertex_list; // vertex�� ��Ƴ��� ����Ʈ

// priority queue�� ������ Ŭ����
// mst������ fringe vertex set�� ���� pq�̸�, cost�� ���� �켱������ remove�� ������ ���̴�.
// �ʿ� �Լ� : insert, removeMin, update
class PQ {
public:
	vector<vertex*>pq; // ���ĵ��� ���� �������� pq�� ����

	// pq�� vertex(������, �� �� fringe vetex)�� insert�ϴ� �Լ�, ���ĵ��� ���� ������
	// �ܼ��� �������� ���� ��Ҹ� �ִ´�.
	// O(1) time�� �ҿ�ȴ�.
	void insert(vertex* v) {
		pq.push_back(v);
	}

	// ���ĵ��� ���� ���������� cost�� ���� ����(�켱������ ���� ����) vertex�� ���� �� return �ϴ� �Լ�
	// cost�� ���ٸ� ������ ��ȣ�� �� ���� ���� �켱������ ����
	// �� �� �켱������ ���� ���� vertex�� tree vertex�� �ǹǷ�, state�� TREE�� �����Ų��.
	// O(n) time�� �ҿ�ȴ�.
	vertex* removeMin() {
		int idx = 0; // pq���� �켱������ ���� ���� vertex�� ��ġ�� ��� ����(erase �� �ʿ�)
		vertex* min_vertex = new vertex(MAX); // ���� ���� cost�� ������ ��ȣ�� ���� vertex�� ��� ����, �� ���� �켱������ ���� vertex�� ��� �����̴�. ������ ��ȣ�� ���� ū ������ �ʱ�ȭ�Ѵ�.
		min_vertex->cost = MAX; // �ʱ�ȭ�δ� cost�� ���� ū ������ �س��´�.

		for (int i = 0; i < pq.size(); i++) { // pq(fringe vertex set)�� ��� ��Ҹ� ���Ͽ� ���� ���� cost�� ���� vertex�� ã�´�.
			if (pq[i]->cost < min_vertex->cost) { // min_vertex���� ���� pq�� cost�� �� ������
				min_vertex = pq[i]; // min_vertex�� �����ϰ�
				idx = i; // idx�� ���ο� ��ġ�� �����Ѵ�.
			}
			else if (pq[i]->cost == min_vertex->cost && pq[i]->num < min_vertex->num) { // ���� ����� ������ ������ ��ȣ�� �� �۴ٸ�
				min_vertex = pq[i]; // min_vertex�� ���� ���Ž����ش�.
				idx = i; // ���������� idx�� ���Ž�Ų��.
			}
		}
		pq.erase(pq.begin() + idx); // idx�� �ִ� vertex(�켱������ ���� ���� vertex)�� remove�����ش�.
		min_vertex->state = TREE; // ã�� min_vertex�� state�� TREE�� �ٲ��ش�.
		return min_vertex; // ã�� min_vertex�� return �Ѵ�.
	}

	// update�ϴ� �Լ�, tree vertex�� ����� edge �� �� ���� ����� ���� ���� �ִٸ� �װ����� ����� ���Ž�Ű��, �θ� ���� ���Ž�Ų��.
	// ���⼭ v�� fringe vertex�̴�.
	// e�� src�� tree vertex�̸� dst�� v�̴�.
	// v�� cost�� �ٸ� edge�� ���� ����� ���̹Ƿ� e�� cost�� �ٸ� �� �ִ�. ���� �񱳸� ���� ���Ž����� �ʿ䰡 �ִ�.
	// O(1) time�� �ҿ�ȴ�.
	void update(vertex* v, edge* e) {
		if (e->cost < v->cost) { // ���� ������ e�� cost�� �� ������
			v->cost = e->cost; // e�� cost�� ���Ž����ش�.
			v->parent = e->src; // edge�� ���ŵǾ����Ƿ�, v�� �ݴ��� vertex(������)�� e�� �ݴ��� vertex�� �ȴ�. �� �θ�(tree vertex)�� e�� src�� ���ŵȴ�.
		}
	}
};

// �ּ� ���д� Ʈ���� ����� Ŭ�����̴�.
class MST {
public:
	vector<vertex*>mst[MAX]; // �ּ� ���� Ʈ���� ��Ÿ���� �����̴�. graph�� �ٸ��� �ش� �������� �������� �������� ������ ��������(vertex)�� list(vector)�� ��´�.
	vector<vertex*>mst_verteces; // �ּ� ���� Ʈ���� �����ϴ� vertex�� ��´�. �� �� �ּ� ���� Ʈ���� �� ������� vertex�� �����Ѵ�.
	vertex* root; // MST�� root�� ��� ����
	PQ fringe; // fringe vertex set�� pq�� ��� ����
	int total_cost; // MST�� �� weight, �� �� cost�� ��� ����
	int total_vertex; // �� vertex�� ��, �� n�̴�.

	MST(int n) {
		root = NULL; // root �ʱ�ȭ
		total_cost = 0; // total cost �ʱ�ȭ
		total_vertex = n; // graph�� ��� vertex�� �����ϱ⿡ �� vertex�� ���� n�̴�.
		initState(n); // prim's algorithm�� ���� vertex�� �ʱ�ȭ�Ѵ�.
	}

	// ��� vertex�� UNSEEN���� �ʱ�ȭ �ϰ� cost���� �ʱ�ȭ �Ѵ�.
	void initState(int n) {
		for (int i = 0; i < n; i++) {
			vertex_list[i]->state = UNSEEN;
			vertex_list[i]->cost = 0;
		}
	}

	// ��� ������ ��ȣ�� ���� prim's algorithm���� mst�� ����� �Լ��̴�.
	void primMST(int start) {
		root = vertex_list[start - 1]; // vertex_list���� start��ȣ(-1�� �� ������ 0������ ����Ǿ��� �����̴�.)�� ���� ã�� vertex�� root�� �־� root�� �ʱ�ȭ�Ѵ�.
		root->state = TREE; // root vertex�� tree(mst)�� ������ ����ϱ� ���� state�� TREE�� �����Ų��.
		mst_verteces.push_back(root); // tree vertex�� ��� mst_verteces�� root�� �ִ´�.

		insertMST(root); // root���� �����Ͽ� ��������� mst�� tree vertex�� �߰��ϴ� �Լ��� ȣ���Ѵ�.
	}

	// tree vertex�� s�� �������� fringe vertex�� ã��, �̷��� fringe vertex�� ����ִ� pq���� tree vertex(�켱������ ���� ����)�� ã�� mst�� �߰��ϴ� �Լ�
	// ��������� ȣ���Ͽ� ��� vertex�� tree vertex�μ� mst�� ���Ե� �� �ְ� �Ѵ�.
	void insertMST(vertex* s) {
		if (mst_verteces.size() >= total_vertex) // mst�� tree vertex�� ������ n(total_vertex) ���� (ũ�ų�)���ٸ� �˰����� �Ϸ�(��� vertex�� tree vertex�� ��)�� ���̹Ƿ� return(����)
			return;

		// tree vertex�� s�� ������ ��� vertex�� �˻��Ѵ�. -> incident�� edge���� ��� �ҷ��� �˻��Ѵ�.
		for (int i = 0; i < graph[s->num].size(); i++) {
			edge* e = graph[s->num][i]; // s�� incident�� edge
			vertex* v = e->dst; // s�� ����(adjacen)�� v

			if (v->state == UNSEEN) { // v�� UNSEEN �̶�� FRINGE SET���� �־��ش�.
				v->state = FRINGE; // FRINGE�� ���¸� ����
				// edge�� �����ϴ� �Ͱ� ���� �ǹ̸� ���� �ڵ��̴�.
				v->parent = e->src; // e->src, �� s(tree vertex)�� ���� FRINGE vertex�� �Ǿ����Ƿ� e�� �θ�� �������ش�.
				v->cost = e->cost; // fringe vertex�� v�� cost�� e�� cost�� �������ش�.

				fringe.insert(v); // fringe vertex set�� ����ִ� priority queue�� v�� insert�Ѵ�.
			}
			else if (v->state == FRINGE) { // ���� ���°� FRINGE���ٸ� ������ �ٸ� edge�� ���� FRINGE SET�� ���Ե� ���̹Ƿ�, �� ���� cost�� �������ش�.
				fringe.update(v, e); // v�� cost�� ���� edge�� ���� ������ cost�̹Ƿ�, ���� e(edge)�� cost�� ���� �� ���� cost�� �������ش�.
			}
		}
		vertex* tree_vertex = fringe.removeMin(); // fringe vertex set�� ���� priority queue���� ���� ���� cost�� ���� fringe vertex�� tree vertex�μ� return�Ͽ� tree_vertex ������ ��´�.
		mst_verteces.push_back(tree_vertex); // tree_vertex�� ���� tree vertex�� �� ���̹Ƿ� mst�� tree vertex���� ��� ������ �־��ش�.

		s->cost = tree_vertex->cost; // �� �� s(tree vertex)�� ��� ���� �������� ������ tree_vertex�� ������� �ʱ�ȭ�Ѵ�.
		// mst�� tree vertex�� �߰��ϴ� �ڵ�
		// mst�� �ش� ������ ��ȣ�� ������ vertex�� �߰��Ѵ�.
		// undirected tree �̹Ƿ� �� �� ������(vertex) ��ȣ�� ���� list(vector)�� ����(������)�� �߰��Ѵ�.
		mst[s->num].push_back(tree_vertex);
		mst[tree_vertex->num].push_back(s);

		total_cost += tree_vertex->cost; // �� ���(weight)�� ���� �߰��� tree vertex�� ����� �߰��Ͽ� ������Ų��.
		insertMST(tree_vertex); // ���� tree vertex�� �� tree_vertex�� �������� �ٽ� ��������� insertMST�� ȣ���Ͽ� ��� vertex�� tree vertex�� �� �� �ֵ��� �Ѵ�.
	}

	// ��å�� ������ �� ��� �� Prim's algorithm ��������� ����ϴ� �Լ�
	void print() {
		cout << total_cost << " "; // �� ����� ���
		for (int i = 0; i < mst_verteces.size(); i++) {
			// mst_verteces���� tree vertex�� �� �����Ƿ� �߰��Ǿ������Ƿ� 0������ ���������� ����Ѵ�.
			cout << mst_verteces[i]->num << " ";
		}
		cout << "\n";
	}
};

// �ʱ� graph�� �����ϱ� ���� vertex�� ����� �Լ�
// �������� ��ȣ�� 1~n ���� �̹Ƿ� vertex���� �ʱ�ȭ�Ͽ� vertex_list�� �־�д�.
void initVertex(int n) {
	for (int i = 1; i <= n; i++) {
		vertex* v = new vertex(i);
		vertex_list.push_back(v);
	}
}

// �ʱ� graph�� �����ϱ� ���� edge�� ����� �Լ�
void initEdge(int m) {
	int s1, s2, c;
	// m���� edge(��å��)�� �����.
	for (int i = 0; i < m; i++) {
		cin >> s1 >> s2 >> c;
		// vertex_list�� ������ ������ �ش�Ǵ� �������� �ݴ��� �������� �ҷ��� ���� �Բ� edge�� �ʱ�ȭ��Ų��
		// vertex_list���� 0���� ����Ǿ� �����Ƿ� ��������ȣ-1 �� �� index�� vertex�� ã�´�.
		// undirected�̹Ƿ� edge�� �ΰ� ������ش�.
		edge* e1 = new edge(vertex_list[s1 - 1], vertex_list[s2 - 1], c);
		edge* e2 = new edge(vertex_list[s2 - 1], vertex_list[s1 - 1], c);

		// undirected�̹Ƿ� ������1�� ������2�� �ش�Ǵ� list(vector)�� incident�� edge�� �߰������ش�.
		graph[s1].push_back(e1); // ������ ��ȣ 1�� �ش��ϴ� list(vector)���� ������ ��ȣ 1���� ����ϴ� ��å��(edge)�� �߰�
		graph[s2].push_back(e2); // ������ ��ȣ 2�� �ش��ϴ� list(vector)���� ������ ��ȣ 2���� ����ϴ� ��å��(edge)�� �߰�
	}
}

int main() {
	// ������ ������ ��, ������ ������ ��å�� ������ ��, ������ ��
	int n, m, q;
	cin >> n >> m >> q;

	// ��������Ʈ ��� �׷����� ����� ���� vertex�� edge�� ����� �Լ��� �ҷ����� �ڵ��̴�.
	initVertex(n);
	initEdge(m);

	// ���� P�� ��� ������ ��ȣ�� ��� �����̴�.
	char p; int s;
	// ������ �� q�� �ݺ��Ѵ�.
	for (int i = 0; i < q; i++) {
		cin >> p >> s;
		
		MST mst(n); // vertex�� ������ n����� ������ mst(�ּ� ���� Ʈ��)�� �ʱ�ȭ�Ѵ�.
		mst.primMST(s); // ��� ������ ��ȣ s�� ���ڷ� �����Ͽ� prim's algorithm�� mst�� ��������� �Լ��� ȣ���Ѵ�.
		mst.print(); // ������Ŀ� �°� ������ ������ִ� �Լ��� ȣ���Ѵ�.
	}
	return 0;
}