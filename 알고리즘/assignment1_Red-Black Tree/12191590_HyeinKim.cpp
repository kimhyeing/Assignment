#include<iostream>
#include<string>

using namespace std;

#define RED 0
#define BLACK 1

class Node {
public:
	//필드
	int ID;
	string name;
	int storage;
	int price;

	//부모자식
	Node* parent;
	Node* left;
	Node* right;

	//추가정보
	int color;
	int depth;

	Node() {
		ID = storage = price = depth = 0;
		name = "";
		parent = left = right = NULL;
		color = RED;
	}

	//데이터 설정(필드 설정)
	void setData(int id, string name, int storage, int price) {
		this->ID = id;
		this->name = name;
		this->storage = storage;
		this->price = price;
	}
};

// insert, delete, restructure, recolor, find
// check 함수
class RedBlackTree {
public:
	Node* root;
	int x; int y; // 범위
	double percent; // P%의 할인율
	
	RedBlackTree() {
		root = NULL;
		x = y = percent = 0;
	}

	void insert(int a, string n, int s, int p);
	Node* search(int a);
	void fsearch(int a);
	void leftRotation(Node* node, bool colorSet);
	void rightRotation(Node* node, bool colorSet);
	void restructuring(Node* node);
	void recoloring(Node* node);
	void update(int a, string n, int s, int p);
	void discount(int x, int y, double P);
	void inorder(Node* node);
};

// (1) 애플리케이션 등록
void RedBlackTree::insert(int a, string n, int s, int p) {
	// 이미 있다면 depth 출력하고 등록 거절
	Node* node = search(a);
	if (node != NULL) {
		cout << node->depth << "\n";
		return;
	}

	Node* new_Node = new Node();
	new_Node->setData(a, n, s, p);
	Node* cur = root;
	Node* par = NULL;

	// 삽입할 위치를 찾는다. (삽입할 new_Node의 부모를 찾는다)
	while (cur != NULL) {
		par = cur;
		if (cur->ID > a) cur = cur->left;
		else cur = cur->right;
	}
	
	// par의 위치에 따라 왼쪽, 오른쪽 자식으로 구분해준다. par이 NULL이면 new_Node는 root
	if (par == NULL) {// new_Node는 root
		root = new_Node;
		root->color = BLACK;
		cout << 0 << "\n"; // depth 출력하고 return
		return;
	}
	else if (par->ID > a)
		par->left = new_Node;
	else
		par->right = new_Node;
	new_Node->parent = par;

	
	// double red일 경우 uncle node의 색에 따라 고친다.
	if (par->color == RED) {
		// uncle node를 설정
		Node* uncle = NULL;
		if (par->parent == NULL) { // node의 부모가 root
			cout << 1 << "\n"; // depth 출력하고 return
			return;
		}
		if (par->parent->left == par)
			uncle = par->parent->right;
		else
			uncle = par->parent->left;

		if (uncle == NULL || uncle->color == BLACK) // uncle이 NULL(leaf node)이면 color는 BLACK이다
			restructuring(new_Node); // uncle의 color가 black일 경우 restructuring 진행
		else if (uncle->color == RED)
			recoloring(new_Node); // uncle의 color가 red일 경우 recoloring 진행
	}
	cout << search(a)->depth << "\n";
}


// node를 찾고 그 node의 depth를 설정
Node* RedBlackTree::search(int a) {
	Node* cur = root;
	int depth = 0;
	while (cur != NULL) {
		if (cur->ID == a) { // node를 찾음
			cur->depth = depth;
			return cur;
		}
		else if (cur->ID > a) cur = cur->left; // a(ID)값이 더 작다면 왼쪽 자식 노드를 탐색
		else cur = cur->right; // a(ID)값이 더 크다면 오른쪽 자식 노드를 탐색
		depth++;
	}
	return NULL;
}

// 왼쪽 로테이션 하는 함수
// 1) node의 오른쪽 자식이 부모가 된다.
// 2) 오른쪽 자식의 왼쪽 자식이 node의 오른쪽 자식이 됨
// 3) colorSet이 true이면 color를 설정
// 4) root가 바뀔 경우 root를 새로 설정해준다.
void RedBlackTree::leftRotation(Node* node, bool colorSet) {
	// 노드가 리프이거나 노드의 오른쪽이 리프이면 
	if (node == NULL || node->right == NULL)
		return;
	// 노드가 root이면 rotation으로 root가 바뀌므로 새로 root를 바꾸겠다는 flag를 set해준다.
	bool rootSet = false;
	if (node == root)
		rootSet = true;

	Node* temp = node->right; // node의 새로운 부모가 될 것을 temp에 저장
	Node* new_right = temp->left; // node의 오른쪽 자식이 될 것을 new_right에 저장


	// 1) node의 오른쪽 자식이 부모(node->right)가 된다.
	// 1-1) temp의 부모설정
	temp->parent = node->parent; // temp의 부모 설정
	if (node->parent != NULL) { // node의 parent가 NULL이 아니라면, 즉 node가 root가 아니라면
		if (node->parent->left == node) // node의 부모의 자식을 temp로 설정
			node->parent->left = temp;
		else
			node->parent->right = temp;
	}
	// 1-2) node의 부모 설정
	node->parent = temp; //node의 부모 설정
	temp->left = node; //temp의 자식을 node로 설정

	
	// 2) 오른쪽 자식의 왼쪽 자식이 node의 오른쪽 자식이 됨
	// 2-1) new_right의 부모 설정
	if(new_right != NULL) // new_right가 NULL(리프노드)가 아니라면 parent 설정
		new_right->parent = node;
	node->right = new_right; // node의 오른쪽 자식을 new_right로

	// 3) color 설정
	if (colorSet) {
		temp->color = BLACK;
		temp->left->color = temp->right->color = RED;
	}

	// 4) root 변경
	if (rootSet)
		root = temp;
}

// 오른쪽 로테이션 하는 함수
// 1) node의 왼쪽 자식이 부모가 된다.
// 2) 왼쪽 자식의 오른쪽 자식이 node의 왼쪽 자식이 됨
// 3) colorSet이 true이면 color를 설정
// 4) root가 바뀔 경우 root를 새로 설정해준다.
void RedBlackTree::rightRotation(Node* node, bool colorSet) {
	// 노드가 리프이거나 노드의 왼쪽이 리프이면 
	if (node == NULL || node->left == NULL)
		return;
	// 노드가 root이면 rotation으로 root가 바뀌므로 새로 root를 바꾸겠다는 flag를 set해준다.
	bool rootSet = false;
	if (node == root)
		rootSet = true;

	Node* temp = node->left; // node의 새로운 부모가 될 것을 temp에 저장
	Node* new_left = temp->right; // node의 왼쪽 자식이 될 것을 new_left에 저장


	// 1) node의 왼쪽 자식이 부모(node->right)가 된다.
	// 1-1) temp의 부모설정
	temp->parent = node->parent; // temp의 부모 설정
	if (node->parent != NULL) { // node의 parent가 NULL이 아니라면, 즉 node가 root가 아니라면
		if (node->parent->left == node) // node의 부모의 자식을 temp로 설정
			node->parent->left = temp;
		else
			node->parent->right = temp;
	}
	// 1-2) node의 부모 설정
	node->parent = temp; //node의 부모 설정
	temp->right = node; //temp의 오른쪽 자식을 node로 설정


	// 2) 왼쪽 자식의 오른쪽 자식이 node의 왼쪽 자식이 됨
	// 2-1) new_left의 부모 설정
	if (new_left != NULL) // new_left가 NULL(리프노드)가 아니라면 parent 설정
		new_left->parent = node;
	node->left = new_left; // node의 왼쪽 자식을 new_left로

	// 3) color 설정
	if (colorSet) {
		temp->color = BLACK;
		temp->left->color = temp->right->color = RED;
	}

	// 4) root 변경
	if(rootSet)
		root = temp;
}

// restructuring 하는 함수
// grand-parent-node가 일자라면 grand에서 lotation 한 번(색을 바꿔준다) -> 총 lotation 한 번
// grand-parent-node가 꺾여있다면 일자로 만들기 위해 parent에서 lotation 한 번, 그 후 grand에서 lotation(색을 바꿔준다) -> 총 lotation 두 번
void RedBlackTree::restructuring(Node* node) {
	Node* par = node->parent;
	Node* grand = par->parent;

	if (grand->left == par) {
		if (par->left == node) // 왼쪽으로 일자 -> grand에서 right lotation 한번
			rightRotation(grand, true);
		else { // 꺾여있다.
			leftRotation(par, false); // 일자로 만들어준다.
			rightRotation(grand, true); // 왼쪽으로 일자 -> grand에서 right lotation 한번
		}
	}
	else {
		if (par->right == node) // 오른쪽으로 일자 -> grand에서 left lotation 한번
			leftRotation(grand, true);
		else { // 꺾여있다.
			rightRotation(par, false); // 일자로 만들어준다.
			leftRotation(grand, true); // 오른쪽으로 일자 -> grand에서 left lotation 한번
		}
	}
}

// recoloring 하는 함수
// 1) node의 parent, uncle node, node의 parent의 parent의 색을 바꿔준다
// 2) 수행 결과, double red가 발생할 가능성이 있다 -> double red가 해결될 때 까지 recoloring 혹은 restructuring 진행
void RedBlackTree::recoloring(Node* node) {
	// 1) 색 반전
	Node* grand = node->parent->parent;
	grand->left->color = grand->right->color = BLACK;
	grand->color = RED;
	// grand가 root라면 색을 black으로 바꾸고 종료
	if (grand == root) {
		grand->color = BLACK;
		return;
	}

	// 2) double red 해결
	// 2-1) double red가 발생했는지 check
	if (grand->parent->color == BLACK) // double red가 발생하지 않음
		return;
	// 2-2) grand 기준 uncle node를 확인
	Node* uncle = NULL;
	if (grand->parent->parent->left == grand->parent)
		uncle = grand->parent->parent->right;
	else
		uncle = grand->parent->parent->left;
	// 2-3) grand 기준 restructuring, recoloring 진행
	if (uncle == NULL || uncle->color == BLACK) // uncle이 NULL(leaf node)이면 color는 BLACK이다
		restructuring(grand); // uncle의 color가 black일 경우 restructuring 진행
	else if (uncle->color == RED)
		recoloring(grand); // uncle의 color가 red일 경우 recoloring 진행
}

// (2) 애플리케이션 검색
void RedBlackTree::fsearch(int a) {
	Node* node = search(a);
	if (node == NULL)
		cout << "NULL\n";
	else
		cout << node->depth << " " << node->name << " " << node->storage << " " << node->price << "\n";
}

// (3) 애플리케이션 업데이트
void RedBlackTree::update(int a, string n, int s, int p) {
	// 없다면 NULL 출력
	Node* node = search(a);
	if (node == NULL) {
		cout << "NULL\n";
		return;
	}
	// 있다면 애플리케이션을 업데이트 하고 depth를 출력한다
	node->setData(a, n, s, p);
	cout << node->depth << "\n";
}

// (4) 애플리케이션 할인
void RedBlackTree::discount(int x, int y, double P) {
	this->x = x; this->y = y;
	this->percent = (100 - P) / (double)100;
	// root부터 inorder로 탐색하면서 x,y 범위에 맞는 ID를 가진 애플리케이션에 할인율 적용
	inorder(root);
}

// inorder로 탐색하면서 [x,y] 범위를 찾는다.
void RedBlackTree::inorder(Node* node) {
	if (root == NULL)
		return;

	if (node == NULL) return;
	if (node->left != NULL) inorder(node->left);

	// ID가 범위안에 있다면 percent(P%의 할인율)를 적용한다.
	if (node->ID >= x && node->ID <= y)
		node->price *= percent;
	else if (node->ID > y) // inorder이므로 오름차순으로 탐색한다. 탐색하는 ID가 y보다 크다면 이후에 탐색할 것들은 다 y보다 크므로 탐색을 중단
		return;

	if (node->right != NULL) inorder(node->right);
}


int main() {
	RedBlackTree* rbt = new RedBlackTree();
	int t;
	cin >> t;
	char q;
	int a, s, p, x, y;
	double P;
	string n;
	while(t--) {
		cin >> q;
		switch (q) {
		case 'I': // 애플리케이션 등록
			cin >> a >> n >> s >> p;
			rbt->insert(a, n, s, p);
			break;
		case 'F': // 애플리케이션 검색
			cin >> a;
			rbt->fsearch(a);
			break;
		case 'R': // 애플리케이션 업데이트
			cin >> a >> n >> s >> p;
			rbt->update(a, n, s, p);
			break;
		case 'D': // 애플리케이션 할인
			cin >> x >> y >> P;
			rbt->discount(x, y, P);
			break;
		default:
			break;
		}
	}
	return 0;
}