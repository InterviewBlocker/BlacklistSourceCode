#define _CRT_SECURE_NO_WARNINGS
#include"huffmanCompress.hpp"

void HFMCompress::HFMCompressed(string FilePath) {
	//记录每种字符的出现次数
	vector<DataRecord> D(256);
	vector<DataRecord> temp_(256);//检测没有出现过的字符
	//1.统计元素个数
	FILE* FIn = fopen(FilePath.c_str(), "rb");
	if (FIn == nullptr) {
		perror("fopen:");
		return;
	}
	while (1) {
		unsigned char buf[1024] = { 0 };
		int ret_size = fread(buf, sizeof(char), 1024, FIn);
		if (ret_size == 0) {
			break;
		}
		int i;
		for (i = 0; i < ret_size; i++) {
			D[buf[i]]._ht = buf[i];
			D[buf[i]]._count++;
		}
	}
	//2.根据字符的出现频次创建哈夫曼树
	HuffmanTree<DataRecord> HT;
	HT.CreateHT(D, temp_);
	//3.根据哈夫曼树获得哈夫曼编码
	HFMCode(HT.GetRoot(), D);
	fseek(FIn, 0, SEEK_SET);//调整文件流指针的位置
	//4.用得到的编码对文件中的每个字节进行改写
	FILE* FOut = fopen("2.txt", "wb");
	if (FOut == nullptr) {
		perror("fopen:");
		return;
	}
	//a.组织数据
	string File_Format("txt");
	File_Format.push_back('\n');
	vector<string> File_DataCount;
	size_t i, f_size = 0;
	for (i = 0; i < 256; i++) {
		if (D[i]._count != 0) {
			string new_f;
			new_f.push_back(D[i]._ht);
			new_f.push_back(':');
			new_f += std::to_string(D[i]._count);
			new_f.push_back('\n');
			f_size++;
			File_DataCount.push_back(new_f);
		}
	} 
	string File_size(std::to_string(f_size).c_str());
	File_size.push_back('\n');
	fwrite(File_Format.c_str(), sizeof(char), File_Format.size(), FOut);
	fwrite(File_size.c_str(), sizeof(char), File_size.size(), FOut);
	for (i = 0; i < File_DataCount.size(); i++) {
		fwrite(File_DataCount[i].c_str(), sizeof(char), File_DataCount[i].size(), FOut);
	}
	//b.改写编码
	unsigned char temp = 0;
	int code_count = 0;
	while (1) {
		unsigned char buf[1024] = { 0 };
		size_t ret_size = fread(buf, sizeof(char), 1024, FIn);
		if (ret_size == 0) {
			break;
		}
		for (i = 0; i < ret_size; i++) {
			string& typedata = D[buf[i]]._code;
			size_t j;
			for (j = 0; j < typedata.size(); j++) {
				temp = ((temp <<= 1) | (typedata[j] - '0'));
				code_count++;
				if (code_count == 8) {
					fwrite(&temp, sizeof(char), 1, FOut);
					temp = 0;
					code_count = 0;
				}
			}
		}
	}
	if (code_count != 0) {
		temp <<= (8 - code_count);//注意：将最后的剩余的字符移动到前列
		fwrite(&temp, sizeof(char), 1, FOut);
	}
	fclose(FIn);
	fclose(FOut);
}

void HFMCompress::UnHFMCompressed(string FilePath) {
	//1.获取文件格式
	FILE* FIn = fopen(FilePath.c_str(), "rb");
	if (FIn == nullptr) {
		perror("fopen:");
		return;
	}
	string File_type(".");
	File_type += GetLine(FIn);
	//2.获取字符出现次数
	vector<DataRecord> D(256);
	vector<DataRecord> temp(256);
	string Count(GetLine(FIn));
	int count = atoi(Count.c_str());
	size_t all_count = 0;
	int i;
	for (i = 0; i < count; i++) {
		string str(GetLine(FIn));
		if (str.size() == 0) {
			string str_(GetLine(FIn));
			unsigned char index_ = '\n';
			D[index_]._ht = index_;
			D[index_]._count = atoi(str_.c_str() + 1);
			all_count += D[index_]._count;
			continue;
		}
		unsigned char index_ = str[0];
		D[index_]._ht = index_;
		D[index_]._count = atoi(str.c_str() + 2);
		all_count += D[index_]._count;
	}
	//3.构造哈夫曼树
	HuffmanTree<DataRecord> H;
	H.CreateHT(D, temp);
	//4.进行字符替换
	string File_name("3");
	FILE* FOut = fopen((File_name + File_type).c_str(), "wb");
	HuffmanTNode<DataRecord>* cur = H.GetRoot();
	unsigned char temp_1 = 1;
	temp_1 <<= 7;
	while (1) {
		unsigned char buf[1024] = { 0 };
		int ret_size = fread(buf, sizeof(char), 1024, FIn);
		if (ret_size == 0) {
			break;
		}
		for (i = 0; i < ret_size; i++) {
			unsigned char temp_ = buf[i];
			int j, flag = 0;
			for (j = 0; j < 8; j++) {
				if (all_count == 0) {
					flag = 1;
					break;
				}
				if ((temp_1 & temp_) == temp_1) {
					cur = cur->_right;
				}
				else {
					cur = cur->_left;
				}
				temp_ <<= 1;
				if (cur->_left == nullptr && cur->_right == nullptr) {
					fwrite(&cur->_value._ht, sizeof(char), 1, FOut);
					cur = H.GetRoot();
					all_count--;
				}
			}
			if (flag == 1) {
				break;
			}
		}
	}
	fclose(FIn);
	fclose(FOut);
}

string HFMCompress::GetLine(FILE*& FIn) {
	string buf;
	int ret_size;
	unsigned char buf_[1] = { 0 };
	while (1) {
		ret_size = fread(buf_, sizeof(char), 1, FIn);
		if (ret_size == 0) {
			break;
		}
		if (buf_[0] == '\n') {
			break;
		}
		buf.push_back(buf_[0]);
		buf_[0] = { 0 };
	}
	return buf;
}

void HFMCompress::HFMCode(TNode* root, vector<DataRecord>& D) {
	if (root == nullptr) {
		return;
	}
	HFMCode(root->_left, D);
	HFMCode(root->_right, D);
	if (root->_left == nullptr && root->_right == nullptr) {
		TNode* cur = root;
		TNode* parent = root->_parent;
		while (parent) {
			if (cur == parent->_left) {
				root->_value._code += "0";
			}
			if (cur == parent->_right) {
				root->_value._code += "1";
			}
			cur = parent;
			parent = parent->_parent;
		}
		reverse(root->_value._code.begin(), root->_value._code.end());
		D[root->_value._ht]._code = root->_value._code;
	}
}

