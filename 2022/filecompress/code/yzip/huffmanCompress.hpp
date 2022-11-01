#pragma once
#include"huffmanTree.hpp"

//记录节点
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

//哈夫曼压缩
class HFMCompress {
	typedef HuffmanTNode<DataRecord> TNode;
public:
	//压缩
	void HFMCompressed(string FilePath);
	//解压缩
	void UnHFMCompressed(string FilePath);
private:
	//获取哈夫曼编码
	void HFMCode(TNode* root, vector<DataRecord>& D);
	//读取一行
	string GetLine(FILE*& FIn);
};