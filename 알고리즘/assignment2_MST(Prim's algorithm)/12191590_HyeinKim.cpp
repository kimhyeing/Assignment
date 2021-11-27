#include<iostream>
#include<vector>

using namespace std;

//Prim's algorithm에서 3가지 vertex set
#define TREE 0 // tree에 속하는 vertex set
#define FRINGE 1 // tree는 아니지만 tree의 vertex와 인접한 vertex set
#define UNSEEN 2 // 그 외의 vertex set

// 최대 조형물 개수, 최대 비용
#define MAX 10001

// 조형물에 대한 정보를 나타내는 class
class vertex {
public:
	int num; // 조형물 번호
	int state; // vertex가 어떠한 상태인지를 저장하는 변수(TREE, FRINGE, UNSEEN)

	// prim's algorithm으로 만들어지는 mst에서 필요한 변수 2개
	int cost; // prim's algorithm 시 만들어지는 mst에서 edge의 cost을 vertex에 저장한 것
	vertex* parent; // 어떤 tree vertex에 의해 FRINGE가 되었는 지 저장하는 변수

	vertex(int num) {
		this->num = num; // 조형물 번호 초기화
		this->state = UNSEEN; // 기본 상태는 UNSEEN이다.
		parent = NULL; // parent 초기화
		cost = 0; // 비용 초기화
	}
};

// 조형물을 연결하는 산책로
class edge {
public:
	vertex* src; // 산책로 한쪽 편에 연결된 조형물
	vertex* dst; // 산책로 반대편에 연결된 조형물
	int cost; // 산책로 디자인 비용

	// 세가지 변수를 생성자에서 초기화
	edge(vertex* src, vertex* dst, int cost) {
		this->src = src;
		this->dst = dst;
		this->cost = cost;
	}
};

vector<edge*>graph[MAX]; // 인접리스트 기반 그래프, 각 조형물(vertex) 번호에 대해 incident한 edge의 list(vector)가 저장된다.
vector<vertex*>vertex_list; // vertex를 모아놓은 리스트

// priority queue를 구현한 클래스
// mst에서의 fringe vertex set을 담은 pq이며, cost가 작은 우선순위로 remove를 진행할 것이다.
// 필요 함수 : insert, removeMin, update
class PQ {
public:
	vector<vertex*>pq; // 정렬되지 않은 시퀀스로 pq를 구현

	// pq에 vertex(조형물, 그 중 fringe vetex)를 insert하는 함수, 정렬되지 않은 시퀀스
	// 단순히 시퀀스의 끝에 요소를 넣는다.
	// O(1) time이 소요된다.
	void insert(vertex* v) {
		pq.push_back(v);
	}

	// 정렬되지 않은 시퀀스에서 cost가 가장 작은(우선순위가 가장 높은) vertex를 제거 및 return 하는 함수
	// cost가 같다면 조형물 번호가 더 낮은 것이 우선순위가 높다
	// 이 때 우선순위가 가장 높은 vertex는 tree vertex가 되므로, state를 TREE로 변경시킨다.
	// O(n) time이 소요된다.
	vertex* removeMin() {
		int idx = 0; // pq에서 우선순위가 가장 높은 vertex의 위치를 담는 변수(erase 시 필요)
		vertex* min_vertex = new vertex(MAX); // 가장 작은 cost와 조형물 번호를 가진 vertex를 담는 변수, 즉 가장 우선순위가 높은 vertex를 담는 변수이다. 조형물 번호를 가장 큰 값으로 초기화한다.
		min_vertex->cost = MAX; // 초기화로는 cost를 가장 큰 값으로 해놓는다.

		for (int i = 0; i < pq.size(); i++) { // pq(fringe vertex set)의 모든 요소를 비교하여 가장 작은 cost를 가진 vertex를 찾는다.
			if (pq[i]->cost < min_vertex->cost) { // min_vertex보다 현재 pq의 cost가 더 작으면
				min_vertex = pq[i]; // min_vertex를 갱신하고
				idx = i; // idx에 새로운 위치를 저장한다.
			}
			else if (pq[i]->cost == min_vertex->cost && pq[i]->num < min_vertex->num) { // 만약 비용이 같으나 조형물 번호가 더 작다면
				min_vertex = pq[i]; // min_vertex를 새로 갱신시켜준다.
				idx = i; // 마찬가지로 idx도 갱신시킨다.
			}
		}
		pq.erase(pq.begin() + idx); // idx에 있는 vertex(우선순위가 가장 높은 vertex)를 remove시켜준다.
		min_vertex->state = TREE; // 찾은 min_vertex의 state를 TREE로 바꿔준다.
		return min_vertex; // 찾은 min_vertex를 return 한다.
	}

	// update하는 함수, tree vertex와 연결된 edge 중 더 작은 비용을 가진 것이 있다면 그것으로 비용을 갱신시키고, 부모 또한 갱신시킨다.
	// 여기서 v는 fringe vertex이다.
	// e의 src는 tree vertex이며 dst는 v이다.
	// v의 cost는 다른 edge에 의해 저장된 것이므로 e의 cost와 다를 수 있다. 따라서 비교를 통해 갱신시켜줄 필요가 있다.
	// O(1) time이 소요된다.
	void update(vertex* v, edge* e) {
		if (e->cost < v->cost) { // 현재 비교중인 e의 cost가 더 작으면
			v->cost = e->cost; // e의 cost로 갱신시켜준다.
			v->parent = e->src; // edge가 갱신되었으므로, v의 반대쪽 vertex(조형물)은 e의 반대쪽 vertex가 된다. 즉 부모(tree vertex)가 e의 src로 갱신된다.
		}
	}
};

// 최소 스패닝 트리를 만드는 클래스이다.
class MST {
public:
	vector<vertex*>mst[MAX]; // 최소 신장 트리를 나타내는 변수이다. graph와 다르게 해당 조형물을 기준으로 조형물과 인접한 조형물들(vertex)을 list(vector)로 담는다.
	vector<vertex*>mst_verteces; // 최소 신장 트리를 구성하는 vertex를 담는다. 이 때 최소 신장 트리로 들어간 순서대로 vertex를 저장한다.
	vertex* root; // MST의 root를 담는 변수
	PQ fringe; // fringe vertex set을 pq로 담는 변수
	int total_cost; // MST의 총 weight, 즉 총 cost를 담는 변수
	int total_vertex; // 총 vertex의 수, 즉 n이다.

	MST(int n) {
		root = NULL; // root 초기화
		total_cost = 0; // total cost 초기화
		total_vertex = n; // graph의 모든 vertex를 포함하기에 총 vertex의 수는 n이다.
		initState(n); // prim's algorithm에 따라 vertex를 초기화한다.
	}

	// 모든 vertex를 UNSEEN으로 초기화 하고 cost또한 초기화 한다.
	void initState(int n) {
		for (int i = 0; i < n; i++) {
			vertex_list[i]->state = UNSEEN;
			vertex_list[i]->cost = 0;
		}
	}

	// 출발 조형물 번호에 따라 prim's algorithm으로 mst를 만드는 함수이다.
	void primMST(int start) {
		root = vertex_list[start - 1]; // vertex_list에서 start번호(-1을 한 이유는 0번부터 저장되었기 때문이다.)를 통해 찾은 vertex를 root에 넣어 root를 초기화한다.
		root->state = TREE; // root vertex가 tree(mst)에 들어갔음을 명시하기 위해 state를 TREE로 변경시킨다.
		mst_verteces.push_back(root); // tree vertex를 담는 mst_verteces에 root를 넣는다.

		insertMST(root); // root부터 시작하여 재귀적으로 mst에 tree vertex를 추가하는 함수를 호출한다.
	}

	// tree vertex인 s를 기준으로 fringe vertex를 찾고, 이러한 fringe vertex를 담고있는 pq에서 tree vertex(우선순위가 가장 높다)를 찾아 mst에 추가하는 함수
	// 재귀적으로 호출하여 모든 vertex가 tree vertex로서 mst에 포함될 수 있게 한다.
	void insertMST(vertex* s) {
		if (mst_verteces.size() >= total_vertex) // mst의 tree vertex의 개수가 n(total_vertex) 보다 (크거나)같다면 알고리즘이 완료(모든 vertex가 tree vertex가 됨)된 것이므로 return(종료)
			return;

		// tree vertex인 s와 인접한 모든 vertex를 검사한다. -> incident한 edge들을 모두 불러와 검사한다.
		for (int i = 0; i < graph[s->num].size(); i++) {
			edge* e = graph[s->num][i]; // s와 incident한 edge
			vertex* v = e->dst; // s와 인접(adjacen)한 v

			if (v->state == UNSEEN) { // v가 UNSEEN 이라면 FRINGE SET으로 넣어준다.
				v->state = FRINGE; // FRINGE로 상태를 변경
				// edge를 설정하는 것과 같은 의미를 가진 코드이다.
				v->parent = e->src; // e->src, 즉 s(tree vertex)에 의해 FRINGE vertex가 되었으므로 e를 부모로 설정해준다.
				v->cost = e->cost; // fringe vertex인 v의 cost는 e의 cost로 설정해준다.

				fringe.insert(v); // fringe vertex set을 담고있는 priority queue에 v를 insert한다.
			}
			else if (v->state == FRINGE) { // 만약 상태가 FRINGE였다면 이전에 다른 edge로 인해 FRINGE SET에 포함된 것이므로, 더 작은 cost로 갱신해준다.
				fringe.update(v, e); // v의 cost는 이전 edge에 의해 설정된 cost이므로, 현재 e(edge)의 cost와 비교해 더 작은 cost로 설정해준다.
			}
		}
		vertex* tree_vertex = fringe.removeMin(); // fringe vertex set을 담은 priority queue에서 가장 작은 cost를 가진 fringe vertex를 tree vertex로서 return하여 tree_vertex 변수에 담는다.
		mst_verteces.push_back(tree_vertex); // tree_vertex는 이제 tree vertex가 된 것이므로 mst의 tree vertex들을 담는 변수에 넣어준다.

		s->cost = tree_vertex->cost; // 이 때 s(tree vertex)의 비용 또한 맞은편의 인접한 tree_vertex의 비용으로 초기화한다.
		// mst에 tree vertex를 추가하는 코드
		// mst의 해당 조형물 번호에 인접한 vertex를 추가한다.
		// undirected tree 이므로 양 쪽 조형물(vertex) 번호에 대해 list(vector)에 서로(조형물)를 추가한다.
		mst[s->num].push_back(tree_vertex);
		mst[tree_vertex->num].push_back(s);

		total_cost += tree_vertex->cost; // 총 비용(weight)에 새로 추가된 tree vertex의 비용을 추가하여 증가시킨다.
		insertMST(tree_vertex); // 새로 tree vertex가 된 tree_vertex를 기준으로 다시 재귀적으로 insertMST를 호출하여 모든 vertex가 tree vertex가 될 수 있도록 한다.
	}

	// 산책로 디자인 총 비용 및 Prim's algorithm 진행과정을 출력하는 함수
	void print() {
		cout << total_cost << " "; // 총 비용을 출력
		for (int i = 0; i < mst_verteces.size(); i++) {
			// mst_verteces에는 tree vertex가 된 순서되로 추가되어있으므로 0번부터 순차적으로 출력한다.
			cout << mst_verteces[i]->num << " ";
		}
		cout << "\n";
	}
};

// 초기 graph를 구성하기 위해 vertex를 만드는 함수
// 조형물의 번호는 1~n 까지 이므로 vertex들을 초기화하여 vertex_list에 넣어둔다.
void initVertex(int n) {
	for (int i = 1; i <= n; i++) {
		vertex* v = new vertex(i);
		vertex_list.push_back(v);
	}
}

// 초기 graph를 구성하기 위해 edge를 만드는 함수
void initEdge(int m) {
	int s1, s2, c;
	// m개의 edge(산책로)를 만든다.
	for (int i = 0; i < m; i++) {
		cin >> s1 >> s2 >> c;
		// vertex_list의 정보를 가지고 해당되는 조형물과 반대편 조형물을 불러와 비용과 함께 edge를 초기화시킨다
		// vertex_list에는 0부터 저장되어 있으므로 조형물번호-1 을 한 index로 vertex를 찾는다.
		// undirected이므로 edge를 두개 만들어준다.
		edge* e1 = new edge(vertex_list[s1 - 1], vertex_list[s2 - 1], c);
		edge* e2 = new edge(vertex_list[s2 - 1], vertex_list[s1 - 1], c);

		// undirected이므로 조형물1과 조형물2에 해당되는 list(vector)에 incident한 edge를 추가시켜준다.
		graph[s1].push_back(e1); // 조형물 번호 1에 해당하는 list(vector)에는 조형물 번호 1부터 출발하는 산책로(edge)를 추가
		graph[s2].push_back(e2); // 조형물 번호 2에 해당하는 list(vector)에는 조형물 번호 2부터 출발하는 산책로(edge)를 추가
	}
}

int main() {
	// 조형물 정보의 수, 조형물 사으이 산책로 정부의 수, 질의의 수
	int n, m, q;
	cin >> n >> m >> q;

	// 인접리스트 기반 그래프를 만들기 위해 vertex와 edge를 만드는 함수를 불러오는 코드이다.
	initVertex(n);
	initEdge(m);

	// 질의 P와 출발 조형물 번호를 담는 변수이다.
	char p; int s;
	// 질의의 수 q번 반복한다.
	for (int i = 0; i < q; i++) {
		cin >> p >> s;
		
		MST mst(n); // vertex의 개수는 n개라는 정보로 mst(최소 신장 트리)를 초기화한다.
		mst.primMST(s); // 출발 조형물 번호 s를 인자로 전달하여 prim's algorithm로 mst가 만들어지는 함수를 호출한다.
		mst.print(); // 출력형식에 맞게 값들을 출력해주는 함수를 호출한다.
	}
	return 0;
}