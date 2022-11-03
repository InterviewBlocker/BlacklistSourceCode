#include"lz77Compress.hpp"


void lz77Compress::lz77Compressed() {
	//1.���ļ�
	FILE* Fin = fopen("lz.txt", "rb");//Ҫѹ�����ļ�
	if (Fin == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fout_data = fopen("lz_1.txt", "wb");//ѹ���������ļ�
	if (Fout_data == nullptr) {
		perror("fopen");
		return;
	}
	FILE* Fout_flag = fopen("flag.txt", "wb");//�洢���λ���ļ�
	if (Fout_flag == nullptr) {
		perror("fopen");
		return;
	}
	//2.��ȡ�ļ��е�����
	_win_size = fread(_pWin, 1, WSIZE * 2, Fin);
	if (_win_size == 0) {
		return;
	}
	//3.�������е�����
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
	while(_start + 2 < _win_size) {//��ֹԽ��
		if ((WSIZE * 2) - (MIN_DATA) <= _start && _win_size == (WSIZE * 2)) {
			AgainPutData(Fin);//�ٴζ�������
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
			//��ƥ���ϵ��ַ���¼����ϣ����
			_start++;
			match_count += MIN_MATCH;
			while (match_count != 1) {
				ch = 0;//��ֹԽ��
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
	//1.���ļ�
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
	//2.����flag�ļ��еı�ʶ��lz_2��д������
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
			fseek(Fout_ps, -match_dis, SEEK_END);//�����ļ���ָ���λ��
			int i;
			for (i = 0; i < match_count + 3; i++) {
				uch ch = 0;
				fread(&ch, 1, 1, Fout_ps);
				fwrite(&ch, 1, 1, Fout);
				//ÿ��д��֮��ˢ�»�����
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
	} while (_match_head && count-- && (_start - _match_head) <= MAX_DIST);//��ֹ��ѭ������ֹѰ�Ҿ����Զ
}

ush lz77Compress::GetNextMatch(ush match_head) {
	return _HT.GetNext(match_head);
}

void lz77Compress::AgainPutData(FILE*& FP) {
	//1.���Ҵ����е������ƶ�����
	int i = 0, j = WSIZE;
	while (i < WSIZE) {
		_pWin[i] = _pWin[j];
		i++;
		j++;
	}
	//2.�����µ�����
	ush new_size = fread(_pWin + WSIZE, 1, WSIZE, FP);
	//3.���¹�ϣ��
	_HT.UpdataHT();
	//4.����start��winsize
	_start -= WSIZE;
	_win_size = WSIZE + new_size;
}