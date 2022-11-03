#include"huffmanCompress.hpp"
#include"lz77Compress.hpp"
#include"lzip.hpp"

void test_compress() {
	HFMCompress a;
	a.HFMCompressed("lz.txt");
	a.UnHFMCompressed("2.txt");
}

int main() {
	//TestHuffman();
	//test_compress();
	//_CrtDumpMemoryLeaks();
	//char* it = new char[10]();
	//HashTable ht;
	//lz77Compress lz77;
	//lz77.lz77Compressed();
	//lz77.lz77UnCompressed();
	//vector<int> temp;
	//temp.reserve(10);
	//cout << temp.size() << endl;
	//temp.push_back(1);
	//cout << temp.size() << endl;
	lzip l;
	//l.lzipCompressed();
	l.lzipUnCompressed();
	return 0;
}