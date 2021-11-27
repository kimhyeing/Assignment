#include<iostream>
#include<string>

using namespace std;

#define RED 0
#define BLACK 1

class Node {
public:
	//�ʵ�
	int ID;
	string name;
	int storage;
	int price;

	//�θ��ڽ�
	Node* parent;
	Node* left;
	Node* right;

	//�߰�����
	int color;
	int depth;

	Node() {
		ID = storage = price = depth = 0;
		name = "";
		parent = left = right = NULL;
		color = RED;
	}

	//������ ����(�ʵ� ����)
	void setData(int id, string name, int storage, int price) {
		this->ID = id;
		this->name = name;
		this->storage = storage;
		this->price = price;
	}
};

// insert, delete, restructure, recolor, find
// check �Լ�
class RedBlackTree {
public:
	Node* root;
	int x; int y; // ����
	double percent; // P%�� ������
	
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

// (1) ���ø����̼� ���
void RedBlackTree::insert(int a, string n, int s, int p) {
	// �̹� �ִٸ� depth ����ϰ� ��� ����
	Node* node = search(a);
	if (node != NULL) {
		cout << node->depth << "\n";
		return;
	}

	Node* new_Node = new Node();
	new_Node->setData(a, n, s, p);
	Node* cur = root;
	Node* par = NULL;

	// ������ ��ġ�� ã�´�. (������ new_Node�� �θ� ã�´�)
	while (cur != NULL) {
		par = cur;
		if (cur->ID > a) cur = cur->left;
		else cur = cur->right;
	}
	
	// par�� ��ġ�� ���� ����, ������ �ڽ����� �������ش�. par�� NULL�̸� new_Node�� root
	if (par == NULL) {// new_Node�� root
		root = new_Node;
		root->color = BLACK;
		cout << 0 << "\n"; // depth ����ϰ� return
		return;
	}
	else if (par->ID > a)
		par->left = new_Node;
	else
		par->right = new_Node;
	new_Node->parent = par;

	
	// double red�� ��� uncle node�� ���� ���� ��ģ��.
	if (par->color == RED) {
		// uncle node�� ����
		Node* uncle = NULL;
		if (par->parent == NULL) { // node�� �θ� root
			cout << 1 << "\n"; // depth ����ϰ� return
			return;
		}
		if (par->parent->left == par)
			uncle = par->parent->right;
		else
			uncle = par->parent->left;

		if (uncle == NULL || uncle->color == BLACK) // uncle�� NULL(leaf node)�̸� color�� BLACK�̴�
			restructuring(new_Node); // uncle�� color�� black�� ��� restructuring ����
		else if (uncle->color == RED)
			recoloring(new_Node); // uncle�� color�� red�� ��� recoloring ����
	}
	cout << search(a)->depth << "\n";
}


// node�� ã�� �� node�� depth�� ����
Node* RedBlackTree::search(int a) {
	Node* cur = root;
	int depth = 0;
	while (cur != NULL) {
		if (cur->ID == a) { // node�� ã��
			cur->depth = depth;
			return cur;
		}
		else if (cur->ID > a) cur = cur->left; // a(ID)���� �� �۴ٸ� ���� �ڽ� ��带 Ž��
		else cur = cur->right; // a(ID)���� �� ũ�ٸ� ������ �ڽ� ��带 Ž��
		depth++;
	}
	return NULL;
}

// ���� �����̼� �ϴ� �Լ�
// 1) node�� ������ �ڽ��� �θ� �ȴ�.
// 2) ������ �ڽ��� ���� �ڽ��� node�� ������ �ڽ��� ��
// 3) colorSet�� true�̸� color�� ����
// 4) root�� �ٲ� ��� root�� ���� �������ش�.
void RedBlackTree::leftRotation(Node* node, bool colorSet) {
	// ��尡 �����̰ų� ����� �������� �����̸� 
	if (node == NULL || node->right == NULL)
		return;
	// ��尡 root�̸� rotation���� root�� �ٲ�Ƿ� ���� root�� �ٲٰڴٴ� flag�� set���ش�.
	bool rootSet = false;
	if (node == root)
		rootSet = true;

	Node* temp = node->right; // node�� ���ο� �θ� �� ���� temp�� ����
	Node* new_right = temp->left; // node�� ������ �ڽ��� �� ���� new_right�� ����


	// 1) node�� ������ �ڽ��� �θ�(node->right)�� �ȴ�.
	// 1-1) temp�� �θ���
	temp->parent = node->parent; // temp�� �θ� ����
	if (node->parent != NULL) { // node�� parent�� NULL�� �ƴ϶��, �� node�� root�� �ƴ϶��
		if (node->parent->left == node) // node�� �θ��� �ڽ��� temp�� ����
			node->parent->left = temp;
		else
			node->parent->right = temp;
	}
	// 1-2) node�� �θ� ����
	node->parent = temp; //node�� �θ� ����
	temp->left = node; //temp�� �ڽ��� node�� ����

	
	// 2) ������ �ڽ��� ���� �ڽ��� node�� ������ �ڽ��� ��
	// 2-1) new_right�� �θ� ����
	if(new_right != NULL) // new_right�� NULL(�������)�� �ƴ϶�� parent ����
		new_right->parent = node;
	node->right = new_right; // node�� ������ �ڽ��� new_right��

	// 3) color ����
	if (colorSet) {
		temp->color = BLACK;
		temp->left->color = temp->right->color = RED;
	}

	// 4) root ����
	if (rootSet)
		root = temp;
}

// ������ �����̼� �ϴ� �Լ�
// 1) node�� ���� �ڽ��� �θ� �ȴ�.
// 2) ���� �ڽ��� ������ �ڽ��� node�� ���� �ڽ��� ��
// 3) colorSet�� true�̸� color�� ����
// 4) root�� �ٲ� ��� root�� ���� �������ش�.
void RedBlackTree::rightRotation(Node* node, bool colorSet) {
	// ��尡 �����̰ų� ����� ������ �����̸� 
	if (node == NULL || node->left == NULL)
		return;
	// ��尡 root�̸� rotation���� root�� �ٲ�Ƿ� ���� root�� �ٲٰڴٴ� flag�� set���ش�.
	bool rootSet = false;
	if (node == root)
		rootSet = true;

	Node* temp = node->left; // node�� ���ο� �θ� �� ���� temp�� ����
	Node* new_left = temp->right; // node�� ���� �ڽ��� �� ���� new_left�� ����


	// 1) node�� ���� �ڽ��� �θ�(node->right)�� �ȴ�.
	// 1-1) temp�� �θ���
	temp->parent = node->parent; // temp�� �θ� ����
	if (node->parent != NULL) { // node�� parent�� NULL�� �ƴ϶��, �� node�� root�� �ƴ϶��
		if (node->parent->left == node) // node�� �θ��� �ڽ��� temp�� ����
			node->parent->left = temp;
		else
			node->parent->right = temp;
	}
	// 1-2) node�� �θ� ����
	node->parent = temp; //node�� �θ� ����
	temp->right = node; //temp�� ������ �ڽ��� node�� ����


	// 2) ���� �ڽ��� ������ �ڽ��� node�� ���� �ڽ��� ��
	// 2-1) new_left�� �θ� ����
	if (new_left != NULL) // new_left�� NULL(�������)�� �ƴ϶�� parent ����
		new_left->parent = node;
	node->left = new_left; // node�� ���� �ڽ��� new_left��

	// 3) color ����
	if (colorSet) {
		temp->color = BLACK;
		temp->left->color = temp->right->color = RED;
	}

	// 4) root ����
	if(rootSet)
		root = temp;
}

// restructuring �ϴ� �Լ�
// grand-parent-node�� ���ڶ�� grand���� lotation �� ��(���� �ٲ��ش�) -> �� lotation �� ��
// grand-parent-node�� �����ִٸ� ���ڷ� ����� ���� parent���� lotation �� ��, �� �� grand���� lotation(���� �ٲ��ش�) -> �� lotation �� ��
void RedBlackTree::restructuring(Node* node) {
	Node* par = node->parent;
	Node* grand = par->parent;

	if (grand->left == par) {
		if (par->left == node) // �������� ���� -> grand���� right lotation �ѹ�
			rightRotation(grand, true);
		else { // �����ִ�.
			leftRotation(par, false); // ���ڷ� ������ش�.
			rightRotation(grand, true); // �������� ���� -> grand���� right lotation �ѹ�
		}
	}
	else {
		if (par->right == node) // ���������� ���� -> grand���� left lotation �ѹ�
			leftRotation(grand, true);
		else { // �����ִ�.
			rightRotation(par, false); // ���ڷ� ������ش�.
			leftRotation(grand, true); // ���������� ���� -> grand���� left lotation �ѹ�
		}
	}
}

// recoloring �ϴ� �Լ�
// 1) node�� parent, uncle node, node�� parent�� parent�� ���� �ٲ��ش�
// 2) ���� ���, double red�� �߻��� ���ɼ��� �ִ� -> double red�� �ذ�� �� ���� recoloring Ȥ�� restructuring ����
void RedBlackTree::recoloring(Node* node) {
	// 1) �� ����
	Node* grand = node->parent->parent;
	grand->left->color = grand->right->color = BLACK;
	grand->color = RED;
	// grand�� root��� ���� black���� �ٲٰ� ����
	if (grand == root) {
		grand->color = BLACK;
		return;
	}

	// 2) double red �ذ�
	// 2-1) double red�� �߻��ߴ��� check
	if (grand->parent->color == BLACK) // double red�� �߻����� ����
		return;
	// 2-2) grand ���� uncle node�� Ȯ��
	Node* uncle = NULL;
	if (grand->parent->parent->left == grand->parent)
		uncle = grand->parent->parent->right;
	else
		uncle = grand->parent->parent->left;
	// 2-3) grand ���� restructuring, recoloring ����
	if (uncle == NULL || uncle->color == BLACK) // uncle�� NULL(leaf node)�̸� color�� BLACK�̴�
		restructuring(grand); // uncle�� color�� black�� ��� restructuring ����
	else if (uncle->color == RED)
		recoloring(grand); // uncle�� color�� red�� ��� recoloring ����
}

// (2) ���ø����̼� �˻�
void RedBlackTree::fsearch(int a) {
	Node* node = search(a);
	if (node == NULL)
		cout << "NULL\n";
	else
		cout << node->depth << " " << node->name << " " << node->storage << " " << node->price << "\n";
}

// (3) ���ø����̼� ������Ʈ
void RedBlackTree::update(int a, string n, int s, int p) {
	// ���ٸ� NULL ���
	Node* node = search(a);
	if (node == NULL) {
		cout << "NULL\n";
		return;
	}
	// �ִٸ� ���ø����̼��� ������Ʈ �ϰ� depth�� ����Ѵ�
	node->setData(a, n, s, p);
	cout << node->depth << "\n";
}

// (4) ���ø����̼� ����
void RedBlackTree::discount(int x, int y, double P) {
	this->x = x; this->y = y;
	this->percent = (100 - P) / (double)100;
	// root���� inorder�� Ž���ϸ鼭 x,y ������ �´� ID�� ���� ���ø����̼ǿ� ������ ����
	inorder(root);
}

// inorder�� Ž���ϸ鼭 [x,y] ������ ã�´�.
void RedBlackTree::inorder(Node* node) {
	if (root == NULL)
		return;

	if (node == NULL) return;
	if (node->left != NULL) inorder(node->left);

	// ID�� �����ȿ� �ִٸ� percent(P%�� ������)�� �����Ѵ�.
	if (node->ID >= x && node->ID <= y)
		node->price *= percent;
	else if (node->ID > y) // inorder�̹Ƿ� ������������ Ž���Ѵ�. Ž���ϴ� ID�� y���� ũ�ٸ� ���Ŀ� Ž���� �͵��� �� y���� ũ�Ƿ� Ž���� �ߴ�
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
		case 'I': // ���ø����̼� ���
			cin >> a >> n >> s >> p;
			rbt->insert(a, n, s, p);
			break;
		case 'F': // ���ø����̼� �˻�
			cin >> a;
			rbt->fsearch(a);
			break;
		case 'R': // ���ø����̼� ������Ʈ
			cin >> a >> n >> s >> p;
			rbt->update(a, n, s, p);
			break;
		case 'D': // ���ø����̼� ����
			cin >> x >> y >> P;
			rbt->discount(x, y, P);
			break;
		default:
			break;
		}
	}
	return 0;
}