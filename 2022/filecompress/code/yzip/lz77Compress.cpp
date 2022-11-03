#include"lz77Compress.hpp"


void lz77Compress::lz77Compressed() {
	//1.打开文件
	FILE* Fin = fopen("lz.txt", "rb");//要压缩的文件
	if (Fin == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fout_data = fopen("lz_1.txt", "wb");//压缩出来的文件
	if (Fout_data == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fout_flag = fopen("flag.txt", "wb");//存储标记位的文件
	if (Fout_flag == nullptr) {
		perror("fopen");
		return;
	}
	//2.读取文件中的内容
	_win_size = fread(_pWin, 1, WSIZE * 2, Fin);
	if (_win_size == 0) {
		return;
	}
	//3.处理窗口中的数据
	uch flag = 0;
	uch flag_count = 0;
	ush hashaddr = 0;
	uch ch = 0;
	int i;
	for (i = 0; i < 2; i++) {
		ch = _pWin[i];
		_HT.HashFunc(hashaddr, ch);
	}
	uch match_count = 0;
	ush match_dis = 0;
	while(_start + 2 < _win_size) {//防止越界
		if ((WSIZE * 2) - (MIN_DATA) <= _start && _win_size == (WSIZE * 2)) {
			AgainPutData(Fin);//再次读入数据
		}
		ch = _pWin[_start + 2];
		ush match_head = 0;
		_HT.InsertString(hashaddr, ch, _start, match_head);
		if (match_head != 0) {
			GetLengthMatch(match_count, match_dis, match_head);
		}
		if (match_count >= MIN_MATCH) {
			match_count -= MIN_MATCH;
			fwrite(&match_count, 1, 1, Fout_data);
			fwrite(&match_dis, 1, 2, Fout_data);
			flag = (flag <<= 1) | 1;
			flag_count++;
			//将匹配上的字符记录到哈希表中
			_start++;
			match_count += MIN_MATCH;
			while (match_count != 1) {
				ch = 0;//防止越界
				if (_start + 2 < _win_size) {
					ch = _pWin[_start + 2];
				}
				_HT.InsertString(hashaddr, ch, _start, match_head);
				_start++;
				match_count--;
			}
		}
		else {
			fwrite(&_pWin[_start], 1, 1, Fout_data);
			_start++;
			flag <<= 1;
			flag_count++;
		}
		if (flag_count == 8) {
			fwrite(&flag, 1, 1, Fout_flag);
			flag_count = 0;
		}
		match_count = 0;
		match_dis = 0;
	}
	for (_start; _start < _win_size; _start++) {
		ush matchhead = 0;
		_HT.InsertString(hashaddr, 0, _start, matchhead);
		fwrite(&_pWin[_start], 1, 1, Fout_data);
		flag <<= 1;
		flag_count++;
		if (flag_count == 8) {
			fwrite(&flag, 1, 1, Fout_flag);
			flag_count = 0;
		}
	}
	if (flag_count) {
		flag <<= (8 - flag_count);
		fwrite(&flag, 1, 1, Fout_flag);
	}
	fclose(Fin);
	fclose(Fout_data);
	fclose(Fout_flag);
}

void lz77Compress::lz77UnCompressed() {
	//1.打开文件
	FILE* Fin = fopen("lz_1.txt", "rb");
	if (Fin == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fin_flag = fopen("flag.txt", "rb");
	if (Fin_flag == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fout = fopen("lz_2.txt", "wb");
	if (Fout == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fout_ps = fopen("lz_2.txt", "rb");
	if (Fout_ps == nullptr) {
		perror("fopen");
		return;
	}
	//2.根据flag文件中的标识向lz_2中写入数据
	uch flag = 0;
	uch flag_count = 0;
	uch match_count = 0;
	ush match_dis = 0;
	uch temp = 1;
	temp <<= 7;
	_win_size = fread(_pWin, 1, WSIZE * 2, Fin);
	if (_win_size == 0) {
		return;
	}
	fread(&flag, 1, 1, Fin_flag);
	while (_win_size) {
		if ((flag & temp) == temp) {
			match_dis = _pWin[_start + 2];
			match_dis <<= 8;
			match_dis |= _pWin[_start + 1];
			match_count = _pWin[_start];
			fseek(Fout_ps, -match_dis, SEEK_END);//调整文件流指针的位置
			int i;
			for (i = 0; i < match_count + 3; i++) {
				uch ch = 0;
				fread(&ch, 1, 1, Fout_ps);
				fwrite(&ch, 1, 1, Fout);
				//每次写完之后刷新缓冲区
				fflush(Fout);
			}
			_start += 3;
			_win_size -= 3;
		}
		else {
			fwrite(&_pWin[_start], 1, 1, Fout);
			fflush(Fout);
			_start += 1;
			_win_size -= 1;
		}
		flag <<= 1;
		flag_count++;
		if (flag_count == 8) {
			fread(&flag, 1, 1, Fin_flag);
			flag_count = 0;
		}
		if (_win_size == 0) {
			_strset((char*)_pWin, 0);
			_win_size = fread(_pWin, 1, WSIZE * 2, Fin);
			_start = 0;
		}
	}
	fclose(Fin);
	fclose(Fin_flag);
	fclose(Fout);
	fclose(Fout_ps);
}

void lz77Compress::GetLengthMatch(uch& match_count, ush& match_dis, ush _match_head) {
	int count = 10;
	do {
		ush start = _start;
		ush match_head = _match_head;
		uch temp_count = 0;
		while (temp_count <= MAX_MATCH && start < _win_size && _pWin[match_head] == _pWin[start]) {
			start++;
			match_head++;
			temp_count++;
		}
		if (temp_count > match_count) {
			match_count = temp_count;
			match_dis = start - match_head;
		}
		_match_head = GetNextMatch(_match_head);
	} while (_match_head && count-- && (_start - _match_head) <= MAX_DIST);//防止死循环，防止寻找距离过远
}

ush lz77Compress::GetNextMatch(ush match_head) {
	return _HT.GetNext(match_head);
}

void lz77Compress::AgainPutData(FILE*& FP) {
	//1.将右窗口中的数据移动到左窗
	int i = 0, j = WSIZE;
	while (i < WSIZE) {
		_pWin[i] = _pWin[j];
		i++;
		j++;
	}
	//2.读入新的数据
	ush new_size = fread(_pWin + WSIZE, 1, WSIZE, FP);
	//3.更新哈希表
	_HT.UpdataHT();
	//4.调整start和winsize
	_start -= WSIZE;
	_win_size = WSIZE + new_size;
}