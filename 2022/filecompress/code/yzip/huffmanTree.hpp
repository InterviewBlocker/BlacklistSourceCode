#pragma once
#include"HFMhead_file.hpp"
#include<queue>
using std::queue;

template<class T>
struct HuffmanTNode {
	HuffmanTNode(T value = T())
		:_value(value)
		, _left(nullptr)
		,_right(nullptr)
		,_parent(nullptr)
	{}

	HuffmanTNode* _left;
	HuffmanTNode* _right;
	HuffmanTNode* _parent;
	T _value;
};

template<class T>
class HuffmanTree {
	typedef HuffmanTNode<T> Node;
	struct Cmp {
		bool operator()(const Node* left, const Node* right)const {
			return left->_value > right->_value;
		}
	};
public:
	HuffmanTree()
		:_root(nullptr)
	{}

	~HuffmanTree() {
		Destory(_root);
	}
	//´´½¨¹þ·òÂüÊ÷
	void CreateHT(const vector<T>& V,const vector<T>& temp) {
		std::priority_queue<Node*, vector<Node*>, Cmp> Q;
		size_t i;
		for (i = 0; i < V.size(); i++) {
			if (V[i] != temp[i]) {
				Q.push(new Node(V[i]));
			}
		}
		if (Q.size() == 0) {
			return;
		}
		while (Q.size() != 1) {
			Node* left = Q.top();
			Q.pop();
			Node* right = Q.top();
			Q.pop();
			Node* cur = new Node(left->_value + right->_value);
			cur->_left = left;
			cur->_right = right;
			left->_parent = cur;
			right->_parent = cur;
			Q.push(cur);
		}
		_root = Q.top();
	}

	Node* GetRoot() {
		return _root;
	}

private:
	//Ïú»Ù
	void Destory(Node*& root) {
		if (root == nullptr) {
			return;
		}
		Destory(root->_left);
		Destory(root->_right);
		delete root;
		root = nullptr;
	}

private:
	Node* _root;
};
