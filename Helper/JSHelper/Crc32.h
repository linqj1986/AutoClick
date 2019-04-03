//R0001  add
class CCrc32
{
public:
	CCrc32();
	virtual ~CCrc32();
	void MakeTable();
	
	BOOL GetFileCrc(LPCTSTR szFilename, DWORD &dwCrc32);
	DWORD GetValueCrc(PVOID pBuffer, size_t nSize);
protected:
	DWORD *Crc32Table;
};