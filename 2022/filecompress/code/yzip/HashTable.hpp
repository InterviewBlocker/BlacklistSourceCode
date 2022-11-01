#pragma once
#include"LZ77head_file.hpp"

class HashTable {
public:
	HashTable()
		:_prev(new ush[HASH_SIZE]())
		,_head(new ush[HASH_SIZE]())
	{}

	~HashTable() {
		delete[] _prev;
		delete[] _head;
	}

	void InsertString(ush& hashAddr, uch ch, ush pos, ush& matchhead) {
		HashFunc(hashAddr, ch);
		matchhead = _head[hashAddr];
		_prev[pos & HASH_MASK] = _head[hashAddr];//注意(与哈希掩码运算防止pos越界)
		_head[hashAddr] = pos;
	}

	void HashFunc(ush& hashAddr, uch ch) {
		hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch)) & HASH_MASK;
	}

	ush H_SHIFT(){
		return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
	}

	ush GetNext(ush pos) {
		return _prev[pos & HASH_MASK];//与哈希掩码相运算使得大于WSIZE的pos依然可以取到相应的值
	}

	void UpdataHT() {
		int i;
		for (i = 0; i < HASH_SIZE; i++) {//更新哈希表（将虚指向变为实指向）
			if (_prev[i] < WSIZE) {
				_prev[i] = 0;
			}
			else {
				_prev[i] -= WSIZE;
			}
			if (_head[i] < WSIZE) {
				_head[i] = 0;
			}
			else {
				_head[i] -= WSIZE;
			}
		}
	}

private:
	// 哈希桶的个数为2^15
	const ush HASH_BITS = 15;
	// 哈希表的大小
	const ush HASH_SIZE = (1 << HASH_BITS);
	// 哈希掩码：主要作用是将右窗数据往左窗搬移时，用来更新哈希表中数据，具体参见后文
	const ush HASH_MASK = HASH_SIZE - 1;
	ush* _prev;
	ush* _head;
};