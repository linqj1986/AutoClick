#pragma once

#include "../SingletonObj.h"
#include "../StringHelper/StringHelper.h"
using namespace std;

#define MAX_PAGE_SIZE 20

typedef struct TAG_YX
{
	string strAccessID;
	string strAccessToken;
	BOOL bUsed;
	TAG_YX()
	{
		bUsed = FALSE;
	}
}TAGYX;

typedef struct TAG_RECORD
{
	int nCurrentPage;
	int nPageSize;
	string strAdUserName;
	int nUserLiang;
	string strLiveUserName;
	string strRobotUserName;
	int nRobotUserLiang;
	int nBeginTime;
	int nEndTime;
	int nMsgType;

	TAG_RECORD()
	{
		nCurrentPage = 0;
		nPageSize = 0;
		strAdUserName = "";
		nUserLiang = 0;
		strLiveUserName = "";
		strRobotUserName = "";
		nRobotUserLiang = 0;
		nBeginTime = 0;
		nEndTime = 0;
		nMsgType = 0;
	}

}TAGRECORD;
class CWebCommWrapper
{
	DECLARE_SINGLETON(CWebCommWrapper);
private:
	CWebCommWrapper();
	virtual ~CWebCommWrapper();
	std::string BuildPostContent(int nR,LPCSTR pszData, string strParams);

public:
	//
	string Login(string strName,string strPwd);
	string EnterRoomV2(int nLiveUserId,int nActionType);
	string GetRecord(TAGRECORD tag);

	//2.1	获取管理员对应的云信账号
	string GetAdminYXAcount(int nAduid);
	//2.2	获取自定义聊天标题和内容
	std::string GetPreSetContentList(int nAduid);
	//2.5	初始化信息
	std::string GetInitInfo(int nAduid);
	//2.4	获取直播间的机器人
	std::string GetBotList(TAGROOMINFO tagRoomInfo, int nRobotCount = ROBOT_NUM);
	//2.7	送礼
	std::string SendGift(TAGROOMINFO tagRoomInfo, int nBotID, int nGiftID, int nGiftCount, TAGCHATINFO* pTagRet);
	//2.11	关注/取关接口
	std::string RelationUpdate(TAGROOMINFO tagRoomInfo, TAGROBOTINFO tagBotInfo, int nActionType, TAGCHATINFO* pTagRet);
	//2.6	记录聊天内容
	BOOL RobotLog(TAGROOMINFO tagRoomInfo, int nBotID, int nMsgType, string strMsgContent, TAGCHATINFO* pTagRet, TAGPICINFO* pPicInfo=NULL);

	string RobotEnterRoom(TAGROOMINFO tagRoomInfo, int nFlag = 1);

	string DesAndBase64Encode(char *pszData);
	string DesAndBase64Decode(string strData);
	//BOOL GetYXinfo(int nRoomID, string& strAccessID, string& strAccessToken);
	MAPGIFT          m_mapGifts;
	MAPCHATCONTENT   m_mapChatContents;
	MAPPIC           m_mapPic;
	MAPPIC           m_mapAction;
	string m_strTeamCode;
	int m_nAuID;
	//vector<string> m_vecRoom;
	string m_strRoomInfo;
	vector<TAGYX> m_vecYX;
	BOOL GetIdleYX(TAGYX& tag);
	void SetIdleYX(string strAccessID);
	string m_strSessionID;
	string m_strAUserName;
	BOOL m_bHaveGetGift;
private:
	string m_strProtocol;
	string m_strDomain;
	char m_szUrlHead[MAX_PATH];
	string GetUrlParam();

	TAGZIPFACEINFO   m_tagZipFaceInfo;
//	vector<TAGYXINFO*>            m_vecYX;
	
	BOOL PrasePic(wstring wstrFilePath);
	BOOL PraseGifts(string strInfo);
	BOOL PraseZipFaceInfo(string strInfo);
	BOOL PraseIMMSG(string strInfo, TAGCHATINFO* pTagRet);
	BOOL DownloadFile(string strFileName, string strUrl);
	


};

