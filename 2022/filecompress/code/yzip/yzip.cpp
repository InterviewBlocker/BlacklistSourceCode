#include"lzip.hpp"


void lzip::lzipCompressed() {
	//1.打开文件
	FILE* Fin = fopen("333.txt", "rb");//要压缩的文件
	if (Fin == nullptr) {
		perror("fopen");
		return;
	}

	FILE* Fout = fopen("333zip.txt", "wb");//压缩后的文件
	if (Fout == nullptr) {
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
	ush match_count = 0;
	ush match_dis = 0;
	while (_start + 2 < _win_size) {//防止越界
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
			//将匹配上的字符记录到哈希表中
			_start++;
			ush temp_count = match_count;
			while (temp_count != 1) {
				ch = 0;//防止越界
				if (_start + 2 < _win_size) {
					ch = _pWin[_start + 2];
				}
				_HT.InsertString(hashaddr, ch, _start, match_head);
				_start++;
				temp_count--;
			}
			//将准备好的数据加入到缓冲区中
			match_count -= MIN_MATCH;
			flag = (flag <<= 1) | 1;
			flag_count++;
			WriteBuff(0, match_count, match_dis, flag, flag_count, Fout);
			match_count += MIN_MATCH;
		}
		else {
			//将准备好的数据加入到缓冲区中
			flag <<= 1;
			flag_count++;
			WriteBuff(1, match_count, match_dis, flag, flag_count, Fout);
			_start++;
		}
		match_count = 0;
		match_dis = 0;
	}
	for (_start; _start < _win_size; _start++) {
		ush matchhead = 0;
		_HT.InsertString(hashaddr, 0, _start, matchhead);
		//将剩余的源字符写入缓冲区
		flag <<= 1;
		flag_count++;
		WriteBuff(1, match_count, match_dis, flag, flag_count, Fout);
	}
	fclose(Fin);
	fclose(Fout);
}

void lzip::WriteBuff(int is, ush match_count, ush match_dis, uch& flag, uch& flag_count, FILE*& Fout) {
	//1. 写入数据
	if (is == 1) {//源字符
		lz77_biteLength_.push_back(_pWin[_start]);
	}
	else {//长度和距离
		lz77_biteLength_.push_back(match_count);
		lz77_dis_.push_back(match_dis);
	}
	//2. 写入标记位数据
	if (flag_count == 8) {
		//将标记位数据写入缓冲区
		lz77_flag_.push_back(flag);
		flag_count = 0;
	}
	//3. 判断是否结束
	if ((_start == _win_size - 1 && is == 1) || _start == _win_size) {
		isLast = true;
	}
	//4. 判断是否需要进行hfm压缩
	if (lz77_biteLength_.size() == BUFF_SIZE || isLast) {
		if (flag_count) {
			flag <<= (8 - flag_count);
			lz77_flag_.push_back(flag);
		}
		HFMCompressed(Fout);
	}
}

void lzip::HFMCompressed(FILE*& Fout) {
	//1.记录每种字符出现的次数
	CodeAppearCount();
	//2.创建哈夫曼树
	HuffmanTree<HFMDataType> BiteLengthTree;
	BiteLengthTree.CreateHT(hfm_biteLength_, vector<HFMDataType>(286));
	HuffmanTree<HFMDataType> DisTree;
	DisTree.CreateHT(hfm_dis_, vector<HFMDataType>(30));
	//3.获取编码位长
	GetCodeLength(BiteLengthTree.GetRoot(), hfm_biteLength_, 0);
	GetCodeLength(DisTree.GetRoot(), hfm_dis_, 0);
	//4.生成哈夫曼编码
	GetHuffmanCode(hfm_biteLength_);
	GetHuffmanCode(hfm_dis_);
	//5.写入解压缩时要使用到的信息
	WriteInfo(Fout);
	//6.压缩数据
	size_t i, indexDis = 0, indexFlag = 0;
	uch CPFlag = lz77_flag_[indexFlag++];
	ush CPFlagCount = 0;
	uch WData = 0, WDataCount = 0;
	for (i = 0; i < lz77_biteLength_.size(); i++) {
		if (CPFlag & 0x80) {
			//写入长度距离对
			WriteLengthDis(Fout, lz77_biteLength_[i], lz77_dis_[indexDis++], WData, WDataCount);
		}
		else {
			//写入源字符
			WriteElem(Fout, lz77_biteLength_[i], WData, WDataCount);
		}
		CPFlag <<= 1;
		CPFlagCount++;
		if (indexFlag < lz77_flag_.size() && CPFlagCount == 8) {
			CPFlag = lz77_flag_[indexFlag++];
			CPFlagCount = 0;
		}
	}
	//7.写入256表示块结束
	uch code_length = hfm_biteLength_[256].code_length_;
	uit code = (hfm_biteLength_[256].code_ << (32 - code_length));
	for (i = 0; i < code_length; i++) {
		WData <<= 1;
		WDataCount++;
		if (code & 0x80000000) {
			WData |= 1;
		}
		if (WDataCount == 8) {
			fputc(WData, Fout);
			WData = 0;
			WDataCount = 0;
		}
		code <<= 1;
	}
	//8.判断 WData 是否写完
	if (WDataCount) {
		WData <<= (8 - WDataCount);
		fputc(WData, Fout);
	}
	//9.清空缓冲区使其可以进行下一块的压缩
	lz77_biteLength_.clear();
	lz77_dis_.clear();
	lz77_flag_.clear();
	InfoClear(hfm_biteLength_);
	InfoClear(hfm_dis_);
}

//(1)
void lzip::CodeAppearCount() {
	size_t i, j = 0;
	uit flag_index = 0;
	uch flag_count = 0;
	uch flag = lz77_flag_[flag_index];
	for (i = 0; i < lz77_biteLength_.size(); i++) {
		if (flag & 0x80) {
			//是长度距离对
			hfm_biteLength_[GetLengthCode(lz77_biteLength_[i])].elem_count_++;
			hfm_dis_[GetDisCode(lz77_dis_[j++])].elem_count_++;
		}
		else {
			//是源字符
			hfm_biteLength_[lz77_biteLength_[i]].elem_count_++;
		}
		flag <<= 1;
		flag_count++;
		if (flag_index + 1 < lz77_flag_.size() && flag_count == 8) {
			flag_count = 0;
			flag = lz77_flag_[++flag_index];
		}
	}
	//将256统计一次用来标识块的结束
	hfm_biteLength_[256].elem_count_++;
}

//获取长度对应的表中的编码
ush lzip::GetLengthCode(uch length) {
	ush length_ = length + MIN_MATCH;
	int i;
	for (i = 0; i < 29; i++) {
		if (length_ >= lengthInterval[i].interval[0] && length_ <= lengthInterval[i].interval[1]) {
			return lengthInterval[i].code;
		}
	}
	cout << "GetLengthCode : error!" << endl;
	return 0;
}

//获取距离对应的表中的编码
ush lzip::GetDisCode(ush dis) {
	int i;
	for (i = 0; i < 30; i++) {
		if (dis >= distInterval[i].interval[0] && dis <= distInterval[i].interval[1]) {
			return distInterval[i].code;
		}
	}
	cout << "GetDisCode : error!" << endl;
	return 0;
}

//(3)
void lzip::GetCodeLength(HuffmanTNode<HFMDataType>* root, vector<HFMDataType>& hb, int k) {
	if (root == nullptr) {
		return;
	}
	if (root->_left == nullptr && root->_right == nullptr) {
		hb[root->_value.elem_].code_length_ = k;
		cout << k << endl;
		return;
	}
	k++;
	GetCodeLength(root->_left, hb, k);
	GetCodeLength(root->_right, hb, k);
}

//(4)
void lzip::GetHuffmanCode(vector<HFMDataType>& hb) {
	std::sort(hb.begin(), hb.end(), Cmp1());
	size_t i = 0;
	while (i < hb.size() && hb[i].code_length_ == 0) { i++; }
	if (i == hb.size()) {
		return;
	}
	hb[i++].code_ = 0;
	uit code = 0;
	ush count = 1;
	while (i < hb.size() && hb[i].code_length_ != 0) {
		if (hb[i].code_length_ != hb[i - 1].code_length_) {
			hb[i].code_ = (code + count) << (hb[i].code_length_ - hb[i - 1].code_length_);
			code = hb[i].code_;
			count = 1;
		}
		else {
			hb[i].code_ = hb[i - 1].code_ + 1;
			count++;
		}
		i++;
	}
	std::sort(hb.begin(), hb.end(), Cmp2());
}

//(5)
void lzip::WriteInfo(FILE*& Fout) {
	//1. 是否为最后一块
	if (isLast) {
		fputc(0, Fout);
	}
	else {
		fputc(1, Fout);
	}
	//2. 写入编码位长
	size_t i;
	for (i = 0; i < hfm_biteLength_.size(); i++) {
		fputc(hfm_biteLength_[i].code_length_, Fout);
	}
	for (i = 0; i < hfm_dis_.size(); i++) {
		if (hfm_dis_[i].code_length_ > 8) {
			cout << i << endl;
		}
		fputc(hfm_dis_[i].code_length_, Fout);
	}
}

//(6)
void lzip::WriteLengthDis(FILE*& Fout, uch length, ush dis, uch& WData, uch& WDataCount) {
	HFMDataType& blNode = hfm_biteLength_[GetLengthCode(length)];
	HFMDataType& dNode = hfm_dis_[GetDisCode(dis)];
	uch code_l_l = blNode.code_length_;
	uch code_d_l = dNode.code_length_;
	uit code_l = (blNode.code_ << (32 - code_l_l));
	uit code_d = (dNode.code_ << (32 - code_d_l));
	uch i;
	//长度
	for (i = 0; i < code_l_l; i++) {
		WData <<= 1;
		WDataCount++;
		if (code_l & 0x80000000) {
			WData |= 1;
		}
		if (WDataCount == 8) {
			fputc(WData, Fout);
			WData = 0;
			WDataCount = 0;
		}
		code_l <<= 1;
	}
	//长度的拓展比特位
	ush code_extend_l = 0;
	uch code_extend_l_l = GetLengthExtraBit(length, code_extend_l);
	code_extend_l <<= (16 - code_extend_l_l);
	for (i = 0; i < code_extend_l_l; i++) {
		WData <<= 1;
		WDataCount++;
		if (code_extend_l & 0x8000) {
			WData |= 1;
		}
		if (WDataCount == 8) {
			fputc(WData, Fout);
			WData = 0;
			WDataCount = 0;
		}
		code_extend_l <<= 1;
	}
	//距离
	for (i = 0; i < code_d_l; i++) {
		WData <<= 1;
		WDataCount++;
		if (code_d & 0x80000000) {
			WData |= 1;
		}
		if (WDataCount == 8) {
			fputc(WData, Fout);
			WData = 0;
			WDataCount = 0;
		}
		code_d <<= 1;
	}
	//距离的拓展比特位
	ush code_extend_d = 0;
	uch code_extend_d_l = GetDisExtraBit(dis, code_extend_d);
	code_extend_d <<= (16 - code_extend_d_l);
	for (i = 0; i < code_extend_d_l; i++) {
		WData <<= 1;
		WDataCount++;
		if (code_extend_d & 0x8000) {
			WData |= 1;
		}
		if (WDataCount == 8) {
			fputc(WData, Fout);
			WData = 0;
			WDataCount = 0;
		}
		code_extend_d <<= 1;
	}
}
//获得长度的扩展比特位
uch lzip::GetLengthExtraBit(uch length, ush& Bit) {
	ush length_ = length + MIN_MATCH;
	int i;
	for (i = 0; i < 29; i++) {
		if (length_ >= lengthInterval[i].interval[0] && length_ <= lengthInterval[i].interval[1]) {
			Bit = (length_ - lengthInterval[i].interval[0]);
			return lengthInterval[i].extraBit;
		}
	}
	cout << "GetLengthExtraBit : error!" << endl;
	return 0;
}
//获得距离的拓展比特位
uch lzip::GetDisExtraBit(ush dis, ush& Bit) {
	int i;
	for (i = 0; i < 30; i++) {
		if (dis >= distInterval[i].interval[0] && dis <= distInterval[i].interval[1]) {
			Bit = (dis - distInterval[i].interval[0]);
			return distInterval[i].extraBit;
		}
	}
	cout << "GetDisExtraBit : error!" << endl;
	return 0;
}

//(6)
void lzip::WriteElem(FILE*& Fout, uch code, uch& WData, uch& WDataCount) {
	HFMDataType& CNode = hfm_biteLength_[code];
	uch code_l = CNode.code_length_;
	uit code_ = (CNode.code_ << (32 - code_l));
	uch i;
	for (i = 0; i < code_l; i++) {
		WData <<= 1;
		WDataCount++;
		if (code_ & 0x80000000) {
			WData |= 1;
		}
		if (WDataCount == 8) {
			fputc(WData, Fout);
			WData = 0;
			WDataCount = 0;
		}
		code_ <<= 1;
	}
}

//(9)
void lzip::InfoClear(vector<HFMDataType>& ht) {
	ush i;
	for (i = 0; i < ht.size(); i++) {
		ht[i].code_ = 0;
		ht[i].code_length_ = 0;
		ht[i].elem_count_ = 0;
	}
}

//获取最大匹配长度
void lzip::GetLengthMatch(ush& match_count, ush& match_dis, ush _match_head) {
	int count = 50;
	do {
		ush start = _start;
		ush match_head = _match_head;
		ush temp_count = 0;
		while (temp_count < MAX_MATCH && start < _win_size && _pWin[match_head] == _pWin[start]) {
			start++;
			match_head++;
			temp_count++;
		}
		if (temp_count > match_count && start - match_head <= MAX_DIST) {
			match_count = temp_count;
			match_dis = start - match_head;
		}
		_match_head = GetNextMatch(_match_head);
	} while (_match_head && count-- && _start - _match_head <= MAX_DIST);//防止死循环，防止寻找距离过远
}

//获取哈希表中对应的下一个匹配结点
ush lzip::GetNextMatch(ush match_head) {
	return _HT.GetNext(match_head);
}

//再次读入数据
void lzip::AgainPutData(FILE*& FP) {
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//解压缩
void lzip::lzipUnCompressed() {
	//1.打开文件
	FILE* Fin = fopen("333zip.txt", "rb");
	if (Fin == nullptr) {
		perror("fin fopen:");
		return;
	}

	FILE* Fout = fopen("333uzip.txt", "wb");
	if (Fout == nullptr) {
		perror("fout fopen:");
		return;
	}

	//2.处理文件中的内容
	uch ch;
	while (1) {
		//（1）块标记信息
		ch = fgetc(Fin);
		if (ch == 0) {
			isLast = true;
		}
		//（2）读取编码位长信息
		GetBitLengthTable(hfm_biteLength_, Fin);
		GetBitLengthTable(hfm_dis_, Fin);
		//（3）根据编码位长构建解码表
		vector<DeCodeTable> elemLenTable;
		vector<DeCodeTable> disTable;
		GetDecodeTable(elemLenTable, hfm_biteLength_);
		GetDecodeTable(disTable, hfm_dis_);
		// (4) 解压缩
		UnCompressedBlock(Fout, Fin, elemLenTable, disTable);
		//（5）清空缓冲区
		std::sort(hfm_biteLength_.begin(), hfm_biteLength_.end(), Cmp2());
		std::sort(hfm_dis_.begin(), hfm_dis_.end(), Cmp2());
		InfoClear(hfm_biteLength_);
		InfoClear(hfm_dis_);
		//（6）是否为最后一块
		if (isLast) {
			break;
		}
	}
	fclose(Fin);
	fclose(Fout);
}
//构建位长表
void lzip::GetBitLengthTable(vector<HFMDataType>& BitLenT, FILE*& fPtr) {
	ush i;
	uch ch = 0;
	for (i = 0; i < BitLenT.size(); i++) {
		ch = fgetc(fPtr);
		if (ch != 0) {
			BitLenT[i].code_length_ = ch;
		}
	}
	std::sort(BitLenT.begin(), BitLenT.end(), Cmp1());
}
//构建解码表
void lzip::GetDecodeTable(vector<DeCodeTable>& T, vector<HFMDataType>& H) {
	//统计相同编码位长出现次数
	std::map<uch, ush> M;
	for (auto& e : H) {
		if (e.code_length_ != 0) {
			M[e.code_length_]++;
		}
	}
	//构建解码表
	for (auto& e : M) {
		DeCodeTable D;
		D.deCodeLen_ = e.first;
		D.LenCount_ = e.second;
		if (T.size() == 0) {
			D.code_ = 0;
			D.CharIndex_ = 0;
		}
		else {
			D.code_ = (T[T.size() - 1].code_ + T[T.size() - 1].LenCount_) << (D.deCodeLen_ - T[T.size() - 1].deCodeLen_);
			D.CharIndex_ = T[T.size() - 1].CharIndex_ + T[T.size() - 1].LenCount_;
		}
		T.push_back(D);
	}
}
//对块中的数据进行解压缩
void lzip::UnCompressedBlock(FILE*& Fout, FILE*& Fin, vector<DeCodeTable> elemLenTable, vector<DeCodeTable> disTable) {
	ush elem_1;
	ush elem_2;
	ush length;
	ush dis;
	uch bit_ = fgetc(Fin);
	uch bit_count = 0;
	FILE* fp = fopen("333uzip.txt", "rb");
	if (fp == nullptr) {
		perror("fp fopen:");
		return;
	}
	while (1) {
		elem_1 = Decode(Fin, elemLenTable, hfm_biteLength_, bit_, bit_count);
		if (elem_1 < 256) {//源字符
			fputc(elem_1, Fout);
			fflush(Fout);
		}
		else if (elem_1 > 256) {//长度
			length = GetLength(Fin, elem_1, bit_, bit_count);
			elem_2 = Decode(Fin, disTable, hfm_dis_, bit_, bit_count);
			dis = GetDis(Fin, elem_2, bit_, bit_count);
			//获取长度距离对后开始解压缩
			fseek(fp, -dis, SEEK_END);
			ush i;
			uch ch = 0;
			for (i = 0; i < length; i++) {
				ch = fgetc(fp);
				fputc(ch, Fout);
				fflush(Fout);
			}
		}
		else {//块结束
			break;
		}
	}
	fclose(fp);
}

//解码
ush lzip::Decode(FILE*& Fin, vector<DeCodeTable>& D, vector<HFMDataType>& H, uch& bit_, uch& bit_count) {
	uch i = 0;
	uit num = 0;
	uit code = 0;
	uch code_len = 0;
	while (1) {
		//1.获取相应长度的比特位
		uch j;
		for (j = 0; j < D[i].deCodeLen_ - code_len; j++) {
			GetNextBit(Fin, code, bit_, bit_count);
		}
		code_len = D[i].deCodeLen_;
		//2.将读取的数据与首编码相减
		num = code - D[i].code_;
		//3.根据num判断是继续还是退出
		if (num < D[i].LenCount_) {
			break;
		}
		else {
			i++;
		}
	}
	//解码成功（获取源字符）
	ush emptyCount = 0;
	while (H[emptyCount].code_length_ == 0) { emptyCount++; }
	num += D[i].CharIndex_;
	return H[num + emptyCount].elem_;
}

//获取真实长度
ush lzip::GetLength(FILE*& Fin, ush length, uch& bit_, uch& bit_count) {
	uch i;
	uit code = 0;
	for (i = 0; i < lengthInterval[length - 257].extraBit; i++) {
		GetNextBit(Fin, code, bit_, bit_count);
	}
	return lengthInterval[length - 257].interval[0] + code;
}

//获取真实距离
ush lzip::GetDis(FILE*& Fin, ush dis, uch& bit_, uch& bit_count) {
	uch i;
	uit code = 0;
	for (i = 0; i < distInterval[dis].extraBit; i++) {
		GetNextBit(Fin, code, bit_, bit_count);
	}
	return distInterval[dis].interval[0] + code;
}

//获取一个比特位
void lzip::GetNextBit(FILE*& Fin, uit& getBit, uch& tempBit, uch& tempBitCount) {
	getBit <<= 1;
	if (tempBit & 0x80) {
		getBit |= 1;
	}
	tempBit <<= 1;
	tempBitCount++;
	if (tempBitCount == 8) {
		tempBit = fgetc(Fin);
		tempBitCount = 0;
	}
}