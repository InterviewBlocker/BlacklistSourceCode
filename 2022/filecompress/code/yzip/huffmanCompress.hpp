#pragma once
#include"huffmanTree.hpp"

//��¼�ڵ�
struct DataRecord {

	DataRecord()
		:_ht(0)
		, _code()
		, _count(0)
	{}

	bool operator>(const DataRecord& D) const{
		return _count > D._count;
	}

	bool operator<(const DataRecord& D) const{
		return _count < D._count;
	}
	
	bool operator!=(const DataRecord& D) const{
		return _count != D._count;
	}

	bool operator==(const DataRecord& D) const{
		return _count == D._count;
	}

	DataRecord operator+(const DataRecord& D) const{
		DataRecord temp;
		temp._count = _count + D._count;
		return temp;
	}

	unsigned char _ht;
	string _code;
	size_t _count;
};

//������ѹ��
class HFMCompress {
	typedef HuffmanTNode<DataRecord> TNode;
public:
	//ѹ��
	void HFMCompressed(string FilePath);
	//��ѹ��
	void UnHFMCompressed(string FilePath);
private:
	//��ȡ����������
	void HFMCode(TNode* root, vector<DataRecord>& D);
	//��ȡһ��
	string GetLine(FILE*& FIn);
};