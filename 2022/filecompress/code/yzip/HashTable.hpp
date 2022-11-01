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
		_prev[pos & HASH_MASK] = _head[hashAddr];//ע��(���ϣ���������ֹposԽ��)
		_head[hashAddr] = pos;
	}

	void HashFunc(ush& hashAddr, uch ch) {
		hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch)) & HASH_MASK;
	}

	ush H_SHIFT(){
		return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
	}

	ush GetNext(ush pos) {
		return _prev[pos & HASH_MASK];//���ϣ����������ʹ�ô���WSIZE��pos��Ȼ����ȡ����Ӧ��ֵ
	}

	void UpdataHT() {
		int i;
		for (i = 0; i < HASH_SIZE; i++) {//���¹�ϣ������ָ���Ϊʵָ��
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
	// ��ϣͰ�ĸ���Ϊ2^15
	const ush HASH_BITS = 15;
	// ��ϣ��Ĵ�С
	const ush HASH_SIZE = (1 << HASH_BITS);
	// ��ϣ���룺��Ҫ�����ǽ��Ҵ��������󴰰���ʱ���������¹�ϣ�������ݣ�����μ�����
	const ush HASH_MASK = HASH_SIZE - 1;
	ush* _prev;
	ush* _head;
};