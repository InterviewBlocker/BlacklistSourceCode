#define _CRT_SECURE_NO_WARNINGS
#include"LZ77head_file.hpp"
#include"HashTable.hpp"

class lz77Compress {
public:
	lz77Compress()
		: _pWin(new uch[WSIZE * 2]())
		, _start(0)
		, _win_size(0)
		, _HT()
	{}

	~lz77Compress() {
		delete[] _pWin;
	}

	void lz77Compressed();

	void lz77UnCompressed();
private:
	void GetLengthMatch(uch& match_count, ush& match_dis, ush match_head);

	ush GetNextMatch(ush _macth_head);

	void AgainPutData(FILE*& FP);

	ush _start;
	size_t _win_size;
	uch* _pWin;
	HashTable _HT;

	//将数据存入缓冲区，提供给Huffman进行压缩
	const ush BUFF_SIZE = 0x8000;
	
	vector<uch> lz77_byte_length_;
	vector<ush> lz77_dis_;

};