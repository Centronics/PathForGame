static bool WriteToMemory(void* DstAddr, void* SrcMas, unsigned int Count);
static bool WriteToMemory(void* DstAddr, unsigned int SrcVal);
static bool WriteMemoryBlocks(void* DstAddr, unsigned int SizeOfBlock, unsigned int NumberOfBlocks, void* PBlock);

static bool WriteToMemory(void* DstAddr, void* SrcMas, unsigned int Count)
{
	bool result = true;
	if (Count!=0)
	{
		__try
		{
			DWORD old, old1, old_src, old1_src;
			if (VirtualProtect((LPVOID)DstAddr, (SIZE_T)Count, PAGE_EXECUTE_READWRITE, &old)==FALSE) { result = false; __leave; }
			if (VirtualProtect((LPVOID)SrcMas, (SIZE_T)Count, PAGE_EXECUTE_READWRITE, &old_src)==FALSE) { result = false; __leave; }
			_asm
			{
				mov edi, DstAddr
				mov esi, SrcMas
				mov ecx, Count
				REP movsb
			}
			if (VirtualProtect((LPVOID)SrcMas, (SIZE_T)Count, old_src, &old1_src)==FALSE) result = false;
			if (VirtualProtect((LPVOID)DstAddr, (SIZE_T)Count, old, &old1)==FALSE) result = false;
		}
		__except(EXCEPTION_EXECUTE_HANDLER) { result=false; }
	} else result = true;
	return result;
}

static bool WriteToMemory(void* DstAddr, unsigned int SrcVal)
{
	bool result = true;
	__try
	{
		DWORD old, old1;
		if (VirtualProtect((LPVOID)DstAddr, 4, PAGE_EXECUTE_READWRITE, &old)==FALSE) { result=false; __leave; }
		_asm
		{
			mov eax, SrcVal
			mov ebx, DstAddr
			mov [ebx], eax
		}
		if (VirtualProtect((LPVOID)DstAddr, 4, old, &old1)==FALSE) result=false;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) { result=false; }
	return result;
}

static bool WriteMemoryBlocks(void* DstAddr, unsigned int SizeOfBlock, unsigned int NumberOfBlocks, void* PBlock)
{
	bool result=true;
	__try
	{
		DWORD old, old1, old_src, old1_src;
		unsigned int kol=0;
		kol=SizeOfBlock*NumberOfBlocks;
		if (VirtualProtect((LPVOID)DstAddr, (SIZE_T)kol, PAGE_EXECUTE_READWRITE, &old)==FALSE) { result = false; __leave; }
		if (VirtualProtect((LPVOID)PBlock, (SIZE_T)SizeOfBlock, PAGE_EXECUTE_READWRITE, &old_src)==FALSE) { result = false; __leave; }
		if ((SizeOfBlock==1) || (SizeOfBlock>4) || (SizeOfBlock==3))
		{
			_asm mov edi, DstAddr
			for (unsigned int k=0; k<NumberOfBlocks; k++)
			{
				_asm
				{
					mov esi, PBlock
					mov ecx, SizeOfBlock
					REP movsb
				}
			}
		}
		if (SizeOfBlock==2)
		{
			_asm mov edi, DstAddr
			for (unsigned int k=0; k<NumberOfBlocks; k++)
			{
				_asm
				{
					mov esi, PBlock
					movsw
				}
			}
		}
		if (SizeOfBlock==4)
		{
			_asm mov edi, DstAddr
			for (unsigned int k=0; k<NumberOfBlocks; k++)
			{
				_asm
				{
					mov esi, PBlock
					movsd
				}
			}
		}
		if (VirtualProtect((LPVOID)DstAddr, (SIZE_T)kol, old, &old1)==FALSE) result = false;
		if (VirtualProtect((LPVOID)PBlock, (SIZE_T)SizeOfBlock, old_src, &old1_src)==FALSE) result = false;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) { result=false; }
	return result;
}
