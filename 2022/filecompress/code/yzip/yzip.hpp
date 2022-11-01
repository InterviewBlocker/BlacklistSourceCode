#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include"HashTable.hpp"
#include"huffmanTree.hpp"
#include"lzipHead.hpp"

// 区间码结构体
struct IntervalSolution
{
	ush code;               // 区间编号
	uch extraBit;           // 扩展码
	ush interval[2];        // 改区间中包含多少个数字
};

// 距离区间码
static IntervalSolution distInterval[] = {
	{ 0, 0, { 1, 1 } },
	{ 1, 0, { 2, 2 } },
	{ 2, 0, { 3, 3 } },
	{ 3, 0, { 4, 4 } },
	{ 4, 1, { 5, 6 } },
	{ 5, 1, { 7, 8 } },
	{ 6, 2, { 9, 12 } },
	{ 7, 2, { 13, 16 } },
	{ 8, 3, { 17, 24 } },
	{ 9, 3, { 25, 32 } },
	{ 10, 4, { 33, 48 } },
	{ 11, 4, { 49, 64 } },
	{ 12, 5, { 65, 96 } },
	{ 13, 5, { 97, 128 } },
	{ 14, 6, { 129, 192 } },
	{ 15, 6, { 193, 256 } },
	{ 16, 7, { 257, 384 } },
	{ 17, 7, { 385, 512 } },
	{ 18, 8, { 513, 768 } },
	{ 19, 8, { 769, 1024 } },
	{ 20, 9, { 1025, 1536 } },
	{ 21, 9, { 1537, 2048 } },
	{ 22, 10, { 2049, 3072 } },
	{ 23, 10, { 3073, 4096 } },
	{ 24, 11, { 4097, 6144 } },
	{ 25, 11, { 6145, 8192 } },
	{ 26, 12, { 8193, 12288 } },
	{ 27, 12, { 12289, 16384 } },
	{ 28, 13, { 16385, 24576 } },
	{ 29, 13, { 24577, 32768 } }
};

// 长度区间码
static IntervalSolution lengthInterval[] = {
	{ 257, 0, { 3, 3 } },
	{ 258, 0, { 4, 4 } },
	{ 259, 0, { 5, 5 } },
	{ 260, 0, { 6, 6 } },
	{ 261, 0, { 7, 7 } },
	{ 262, 0, { 8, 8 } },
	{ 263, 0, { 9, 9 } },
	{ 264, 0, { 10, 10 } },
	{ 265, 1, { 11, 12 } },
	{ 266, 1, { 13, 14 } },
	{ 267, 1, { 15, 16 } },
	{ 268, 1, { 17, 18 } },
	{ 269, 2, { 19, 22 } },
	{ 270, 2, { 23, 26 } },
	{ 271, 2, { 27, 30 } },
	{ 272, 2, { 31, 34 } },
	{ 273, 3, { 35, 42 } },
	{ 274, 3, { 43, 50 } },
	{ 275, 3, { 51, 58 } },
	{ 276, 3, { 59, 66 } },
	{ 277, 4, { 67, 82 } },
	{ 278, 4, { 83, 98 } },
	{ 279, 4, { 99, 114 } },
	{ 280, 4, { 115, 130 } },
	{ 281, 5, { 131, 162 } },
	{ 282, 5, { 163, 194 } },
	{ 283, 5, { 195, 226 } },
	{ 284, 5, { 227, 257 } },
	{ 285, 0, { 258, 258 } }
};

//解码表
struct DeCodeTable {
	uch deCodeLen_; //解码位长
	uit code_;      //首字符编码
	ush LenCount_;  //相同编码长度个数
	ush CharIndex_; //符号索引
};

//编码表
class HFMDataType {
public:
	HFMDataType()
		:elem_(0)
		,elem_count_(0)
		,code_(0)
		,code_length_(0)
	{}


	bool operator>(const HFMDataType& D) const {
		return elem_count_ > D.elem_count_;
	}

	bool operator<(const HFMDataType& D) const {
		return elem_count_ < D.elem_count_;
	}

	bool operator!=(const HFMDataType& D) const {
		return elem_count_ != D.elem_count_;
	}

	bool operator==(const HFMDataType& D) const {
		return elem_count_ == D.elem_count_;
	}

	HFMDataType operator+(const HFMDataType& D) const {
		HFMDataType temp;
		temp.elem_count_ = elem_count_ + D.elem_count_;
		return temp;
	}

	ush elem_;		  //源字符
	ush elem_count_;  //源字符出现次数
	uit code_;        //编码
	uch code_length_; //编码位长
};

class lzip {
	struct Cmp1 {
		bool operator()(const HFMDataType& left, const HFMDataType& right) {
			if (left.code_length_ < right.code_length_) {
				return true;
			}
			else if (left.code_length_ == right.code_length_) {
				return left.elem_ < right.elem_;
			}
			return false;
		}
	};

	struct Cmp2 {
		bool operator()(const HFMDataType& left, const HFMDataType& right) {
			return left.elem_ < right.elem_;
		}
	};
public:
	lzip()
		: _pWin(new uch[WSIZE * 2]())
		, _start(0)
		, _win_size(0)
		, _HT()
		, isLast(false)
	{
		lz77_biteLength_.reserve(BUFF_SIZE);
		lz77_dis_.reserve(BUFF_SIZE);
		lz77_flag_.reserve(BUFF_SIZE / 8);
		ush i;
		for (i = 0; i <= 285; i++) {
			HFMDataType h;
			h.elem_ = i;
			hfm_biteLength_.push_back(h);
		}
		for (i = 0; i <= 29; i++) {
			HFMDataType h;
			h.elem_ = i;
			hfm_dis_.push_back(h);
		}
	}

	~lzip() {
		delete[] _pWin;
	}

	void lzipCompressed();

	void lzipUnCompressed();
private:
	////////////////////////////////////////////////////////////////////////////////////////
	/////lz77压缩

	void GetLengthMatch(ush& match_count, ush& match_dis, ush match_head);

	ush GetNextMatch(ush _macth_head);

	void AgainPutData(FILE*& FP);

	void WriteBuff(int is, ush match_count, ush match_dis, uch& flag, uch& flag_count, FILE*& Fout);

	////////////////////////////////////////////////////////////////////////////////////////
	/////哈夫曼压缩

	void HFMCompressed(FILE*& Fout);

	void CodeAppearCount();

	ush GetLengthCode(uch length);

	ush GetDisCode(ush dis);

	void GetCodeLength(HuffmanTNode<HFMDataType>* root, vector<HFMDataType>& hb, int k);

	void GetHuffmanCode(vector<HFMDataType>& hb);

	void WriteInfo(FILE*& Fout);

	void WriteLengthDis(FILE*& Fout, uch length, ush dis, uch& WData, uch& WDataCount);

	void WriteElem(FILE*& Fout, uch code, uch& WData, uch& WDataCount);

	uch GetLengthExtraBit(uch length, ush& Bit);

	uch GetDisExtraBit(ush dis, ush& Bit);

	void InfoClear(vector<HFMDataType>& ht);

	////////////////////////////////////////////////////////////////////////////////////////
	/////解压缩
	
	void GetBitLengthTable(vector<HFMDataType>& BitLenT, FILE*& fPtr);

	void GetDecodeTable(vector<DeCodeTable>& T, vector<HFMDataType>& H);

	void UnCompressedBlock(FILE*& Fout, FILE*& Fin, vector<DeCodeTable> elemLenTable, vector<DeCodeTable> disTable);

	ush Decode(FILE*& Fin, vector<DeCodeTable>& D, vector<HFMDataType>& H, uch& bit_, uch& bit_count);

	void GetNextBit(FILE*& Fin, uit& getBit, uch& tempBit, uch& tempBitCount);

	ush GetLength(FILE*& Fin, ush length, uch& bit_, uch& bit_count);

	ush GetDis(FILE*& Fin, ush dis, uch& bit_, uch& bit_count);

	//是否为最后一块
	bool isLast;

	//lz77成员变量
	ush _start;
	size_t _win_size;
	uch* _pWin;
	HashTable _HT;

	//将数据存入缓冲区，提供给Huffman进行压缩
	const ush BUFF_SIZE = 0x8000;

	vector<uch> lz77_biteLength_;
	vector<ush> lz77_dis_;
	vector<uch> lz77_flag_;

	//huffman中存储统计的源字符（长度）和距离的出现次数
	vector<HFMDataType> hfm_biteLength_;
	vector<HFMDataType> hfm_dis_;
};