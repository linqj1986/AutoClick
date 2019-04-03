#include "StdAfx.h"
#include "WebCommWrapper.h"
#include "..\\ZipHelper\\XUnzip.h"
#include <direct.h>
#include "TCodeConversion.h"
#include "THttpDownload.h"
#include "TDirectory.h"
#include "TFile.h"
using namespace TFC;


CWebCommWrapper::CWebCommWrapper()
{
	/*DWORD dwRetNum;
	TFC::CTHttp oCtHttp;
	BYTE *bRet = oCtHttp.GetUrlContentW(_T("http://120.26.84.31:8089/weixinLogin/login?mobile=test111&pass=123456"), dwRetNum);

	//BYTE *bRet = oCtHttp.PostUrlContentW(_T("http://120.26.84.31:8089/weixinLogin/login"), L"mobile=test111&pass=123456", wcslen(L"mobile=test111&pass=123456"), dwRetNum);
	_tDebugPrint(L"xxxxxxx=%s,dwnum=%d", bRet, dwRetNum);*/

	m_bHaveGetGift = FALSE;
	m_strTeamCode = "";
	m_strProtocol = "";
	m_strDomain = "";
	GetUrlParam();

	memset(m_szUrlHead, NULL, MAX_PATH);
	strcat(m_szUrlHead, m_strProtocol.c_str());
	strcat(m_szUrlHead, "://robot.");
	strcat(m_szUrlHead, m_strDomain.c_str());
	strcat(m_szUrlHead, "/api");
}


CWebCommWrapper::~CWebCommWrapper()
{
}

std::string CWebCommWrapper::BuildPostContent(int nR,LPCSTR pszData,string strParams)
{
	string strGet = DesAndBase64Encode((char*)pszData);
	CStringA strTemp;
	strTemp.Format("%s%d%s", strGet.c_str(), nR,GLOBLE_KEY[nR].c_str());
	std::string strSign = TFC::CTCodeConversion::MD5((BYTE*)strTemp.GetBuffer(), strTemp.GetLength());
	strTemp.ReleaseBuffer();

	strTemp.Format("r=%d&sign=%s&data=%s", nR, strSign.c_str(), TFC::CTCodeConversion::UrlEncode(strGet.c_str()).c_str());
	string strDst = strTemp.GetBuffer();
	strTemp.ReleaseBuffer();

	return strDst;
}

string CWebCommWrapper::Login(string strName, string strPwd)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/Login", m_szUrlHead);

	Json::Value jData;
	jData["UserName"] = strName.c_str();
	jData["Password"] = strPwd.c_str();

	Json::FastWriter jWriter;
	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("");
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();


	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		string strTest = TFC::CTCodeConversion::AnsiToUTF8(strGet.c_str());

		if (reader.parse(strTest.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];

				m_nAuID = jsonData["Aduid"].asInt();
				m_strAUserName = jsonData["AUserName"].asCString();
				m_strTeamCode = jsonData["TeamCode"].asCString();
				m_strSessionID = jsonData["SessionID"].asCString();
				
				for (int i = 0; i < jsonData["YXAccountList"].size(); i++)
				{
					Json::Value jYX = jsonData["YXAccountList"][i];
					TAGYX tag;
					tag.strAccessID = jYX["AccessID"].asCString();
					tag.strAccessToken = jYX["AccessToken"].asCString();
					m_vecYX.push_back(tag);
				}

				// power
				int nChat = jsonData["Powers"][0].asInt();
				int nRecord = jsonData["Powers"][1].asInt();
				if (nChat==1)
				{
					theApp.m_bChat = true;
				}
				if (nRecord == 1)
				{
					theApp.m_bRecord = true;
				}
				
				string strRet = jsonWriter.write(jsonData);
				if (strRet[strRet.length() - 1] == '\n')
				{
					strRet[strRet.length() - 1] = 0x00;
				}

				return strRet;
			}
			else
			{
				if (!jsonRoot["Msg"].isNull())
				{
					wstring wstrMsg = TFC::CTCodeConversion::UTF8ToUnicode(jsonRoot["Msg"].asCString());
					OutputDebugStringW(wstrMsg.c_str());
					AfxMessageBox(wstrMsg.c_str());
				}
			}
		}
		else
		{
			OutputDebugStringA("解析失败");
		}
	}
	return "";
}

string CWebCommWrapper::GetRecord(TAGRECORD tag)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/ChatLog", m_szUrlHead);

	Json::Value jData;
	jData["TeamCode"] = m_strTeamCode.c_str();
	jData["Aduid"] = m_nAuID;
	jData["CurrentPage"] = tag.nCurrentPage;
	jData["PageSize"] = tag.nPageSize;

	if (tag.strAdUserName.length()>0)
		jData["AdUserName"] = TFC::CTCodeConversion::AnsiToUTF8(tag.strAdUserName.c_str()).c_str();
	if (tag.nUserLiang>0)
		jData["UserLiang"] = tag.nUserLiang;
	if (tag.strLiveUserName.length()>0)
		jData["LiveUserName"] = TFC::CTCodeConversion::AnsiToUTF8(tag.strLiveUserName.c_str()).c_str();
	if (tag.strRobotUserName.length()>0)
		jData["RobotUserName"] = TFC::CTCodeConversion::AnsiToUTF8(tag.strRobotUserName.c_str()).c_str();
	if (tag.nRobotUserLiang>0)
		jData["RobotUserLiang"] = tag.nRobotUserLiang;
	if (tag.nBeginTime>0)
		jData["BeginTime"] = tag.nBeginTime;
	if (tag.nEndTime>0)
		jData["EndTime"] = tag.nEndTime;
	if (tag.nMsgType != 0)
		jData["MsgType"] = tag.nMsgType;
	Json::FastWriter jWriter;
	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("");
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();


	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		string strTest = TFC::CTCodeConversion::AnsiToUTF8(strGet.c_str());

		if (reader.parse(strTest.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];

				string strRet = jsonWriter.write(jsonData);
				if (strRet[strRet.length() - 1] == '\n')
				{
					strRet[strRet.length() - 1] = 0x00;
				}

				return strRet;
			}
		}
	}
}

string CWebCommWrapper::EnterRoomV2( int nLiveUserId, int nActionType)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/RobotEnterRoomv2", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = m_nAuID;
	jData["TeamCode"] = m_strTeamCode.c_str();
	jData["LiveUserID"] = nLiveUserId;
	jData["ActionType"] = nActionType;
	jData["SessionID"] = m_strSessionID.c_str();

	Json::FastWriter jWriter;
	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("");
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();


	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		string strTest = TFC::CTCodeConversion::AnsiToUTF8(strGet.c_str());

		if (reader.parse(strTest.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];

				string strRet = jsonWriter.write(jsonData);
				if (strRet[strRet.length() - 1] == '\n')
				{
					strRet[strRet.length() - 1] = 0x00;
				}
				if (nActionType == 1)
				{
					Json::Reader reader;
					Json::Value jsonRoot;
					if (reader.parse(strRet.c_str(), jsonRoot))
					{
						if (!jsonRoot["LiveRoomInfo"].isNull())
						{
							for (int i = 0; i < jsonRoot["LiveRoomInfo"].size(); i++)
							{
								Json::Value jRoom, jSimpleRoom;
								jRoom = jsonRoot["LiveRoomInfo"][i];
								jSimpleRoom["Aduid"] = CWebCommWrapper::GetInstance()->m_nAuID;
								jSimpleRoom["RoomID"] = jRoom["RoomID"].asInt();
								jSimpleRoom["UserID"] = jRoom["UserID"].asInt();
								jSimpleRoom["AName"] = m_strAUserName.c_str();
								jSimpleRoom["UserName"] = jRoom["UserName"].asCString();
								jSimpleRoom["LiveID"] = jRoom["LiveID"].asInt();
								jSimpleRoom["NeteaseRoomID"] = jRoom["NeteaseRoomID"].asInt();
								jSimpleRoom["UserLiang"] = jRoom["UserLiang"].asInt();
								jSimpleRoom["Weburl"] = jRoom["Weburl"].asCString();
								string strNode = jsonWriter.write(jSimpleRoom);
								if (strNode[strNode.length() - 1] == '\n')
								{
									strNode[strNode.length() - 1] = 0x00;
								}
								string strEncode = CWebCommWrapper::GetInstance()->DesAndBase64Encode((char*)(strNode.c_str()));
								m_strRoomInfo = strEncode;
								//vecRoom.push_back(strEncode);
							}
						}
					}
				}
				return "";
			}
			else
			{
				return TFC::CTCodeConversion::UTF8ToAnsi(jsonRoot["Msg"].asCString());
			}
		}
		else
		{
			return "数据解析错误";
		}
	}
	return "连接错误";
}

std::string CWebCommWrapper::GetBotList(TAGROOMINFO tagRoomInfo, int nRobotCount)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/LiveRoomRobot", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = tagRoomInfo.nAduid;
	jData["LiveID"] = tagRoomInfo.nLiveID;

	jData["RobotCount"] = nRobotCount;
	jData["RoomID"] = tagRoomInfo.nRoomID;
	jData["LiveUserID"] = tagRoomInfo.nUserID;
	//int nAduid, const int nRoomID, const int nLiveID, int nRobotCount
	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("");
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	_DebugPrintA("机器人参数=%s", szArray);

	std::string strPostContent = BuildPostContent(nR,szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);

		//_DebugPrintA("机器人数=%s", strGet.c_str());
		string strTest = TFC::CTCodeConversion::AnsiToUTF8(strGet.c_str());

		if (reader.parse(strTest.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];
				
				string strRet = jsonWriter.write(jsonData);
				if (strRet[strRet.length() - 1] == '\n')
				{
					strRet[strRet.length() - 1] = 0x00;
				}
				
				return strRet;
			}
		}
		else
		{
			OutputDebugStringA("机器人解析失败");
		}
	}
	return "";
}

std::string CWebCommWrapper::GetPreSetContentList(int nAduid)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/DefinChatContent", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = nAduid;
	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}
	OutputDebugStringA(strSrc.c_str());
	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d", nAduid);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;
		
		string strGet = DesAndBase64Decode(strResponse);

		string strTest = TFC::CTCodeConversion::AnsiToUTF8(strGet.c_str());
		/*FILE* fn = TFC::CTFile::OpenA("c:\\1.txt", BIN_CREATE);
		if (fn)
		{
			TFC::CTFile::Write(fn, (BYTE*)strGet.c_str(), strlen(strGet.c_str()));
			TFC::CTFile::Close(fn);
		}*/	
		
		if (reader.parse(strTest.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];
				Json::Value jsonChatContentList = jsonData["ChatContentList"];
				for (int i = 0; i < jsonChatContentList.size(); i++)
				{
					Json::Value jsonChatContent = jsonChatContentList[i]["ChatContent"];

					vector<string> *pVec = new vector<string>;
					for (int j = 0; j < jsonChatContent.size(); j++)
					{
						string strCT = TFC::CTCodeConversion::UTF8ToAnsi(jsonChatContent[j].asCString());
						pVec->push_back(strCT);
					}
					
					string strTT = TFC::CTCodeConversion::UTF8ToAnsi(jsonChatContentList[i]["Title"].asCString());
					m_mapChatContents.insert(MAPCHATCONTENT::value_type(strTT, pVec));
				}
				return jsonWriter.write(jsonData);
			}
		}
		else
		{
			OutputDebugStringW(L"获取标签数据失败");
		}
	}
	return "";
}

string CWebCommWrapper::GetAdminYXAcount(int nAduid)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/AdminYXAcount", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = nAduid;
	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}
	_DebugPrintA(strSrc.c_str());

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d", nAduid);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		_DebugPrintA(strGet.c_str());
		if (reader.parse(strGet.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];
				Json::Value jsonChatContent = jsonData["YXAccountList"];
				
				/*for (int i = 0; i < jsonChatContent.size(); i++)
				{
					
					TAGYXINFO* pTag = new TAGYXINFO();
					pTag->strAccessID = jsonChatContent[i]["AccessID"].asString();
					pTag->strAccessToken = jsonChatContent[i]["AccessToken"].asString();
					pTag->nRoomID = 0;
					
					m_vecYX.push_back(pTag);	
				}*/
				return jsonWriter.write(jsonChatContent);
			}
		}
		else
		{
			OutputDebugStringW(L"获取云信数据失败");
		}
	}
	return "";
}

/*BOOL CWebCommWrapper::GetYXinfo(int nRoomID, string& strAccessID, string& strAccessToken)
{
	BOOL bRet = FALSE;
	char szDirectory[MAX_PATH] = { NULL };
	TFC::CTDirectory::GetModuleFileDirectoryA(AfxGetInstanceHandle(), szDirectory, MAX_PATH);
	string strCfgName = szDirectory;
	strCfgName += "\\";
	strCfgName += "config.ini";

	char szID[MAX_PATH] = { NULL };
	char szTK[MAX_PATH] = { NULL };
	GetPrivateProfileStringA("config", "strAccessID", "",szID, MAX_PATH, strCfgName.c_str());
	GetPrivateProfileStringA("config", "strAccessToken","", szTK, MAX_PATH, strCfgName.c_str());

	strAccessID = szID;
	strAccessToken = szTK;
	return bRet;
}*/

std::string CWebCommWrapper::GetInitInfo(int nAduid)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/InitInfo", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = nAduid;
	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}


	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d", nAduid);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		//_DebugPrintA(strGet.c_str());
		if (reader.parse(strGet, jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Data"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Data"];
				if (!jsonData["GiftList"].isNull())
				{
					string strGift = jWriter.write(jsonData["GiftList"]);
					if (strGift[strGift.length() - 1] == '\n')
					{
						strGift[strGift.length() - 1] = 0x00;
					}
					PraseGifts(strGift);
				}
					
				if (!jsonData["ZipFaceInfo"].isNull())
				{
					string strZipFaceInfo = jWriter.write(jsonData["ZipFaceInfo"]);
					if (strZipFaceInfo[strZipFaceInfo.length() - 1] == '\n')
					{
						strZipFaceInfo[strZipFaceInfo.length() - 1] = 0x00;
					}
					PraseZipFaceInfo(strZipFaceInfo);
				}
					
				return jsonWriter.write(jsonData);
			}
		}
		else
		{
			OutputDebugStringW(L"获取初始化数据失败");
		}
		
	}
	return "";
}

std::string CWebCommWrapper::SendGift(TAGROOMINFO tagRoomInfo, int nBotID, int nGiftID, int nGiftCount, TAGCHATINFO* pTagRet)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/SendGift", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = tagRoomInfo.nAduid;
	jData["GiftCount"] = nGiftCount;
	jData["GiftID"] = nGiftID;
	jData["LiveID"] = tagRoomInfo.nLiveID;
	jData["LiveUserID"] = tagRoomInfo.nUserID;
	jData["RobotID"] = nBotID;
	jData["RoomID"] = tagRoomInfo.nRoomID;

	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d%d%d%d%d%d%d", tagRoomInfo.nAduid, nGiftCount, nGiftID,tagRoomInfo.nLiveID, nR
		,nBotID, tagRoomInfo.nRoomID);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		//_DebugPrintA("SendGift=%s", strGet.c_str());
		if (reader.parse(strGet.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Msg"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Msg"];

				if (!jsonRoot["Data"].isNull())
					PraseIMMSG(jsonWriter.write(jsonRoot["Data"]).c_str(), pTagRet);

				return "1";
			}
			else
			{
				if (!jsonRoot["Msg"].isNull())
				{
					wstring wstrMsg = TFC::CTCodeConversion::AnsiToUnicode(jsonRoot["Msg"].asCString());
					OutputDebugStringW(wstrMsg.c_str());
					AfxMessageBox(wstrMsg.c_str());
				}
			}
		}
	}
	return "";
}

std::string CWebCommWrapper::RelationUpdate(TAGROOMINFO tagRoomInfo, TAGROBOTINFO tagBotInfo, int nActionType, TAGCHATINFO* pTagRet)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/RelationUpdate", m_szUrlHead);

	Json::Value jData;
	jData["AdName"] = TFC::CTCodeConversion::AnsiToUTF8(tagRoomInfo.strAName.c_str()).c_str();
	jData["Aduid"] = tagRoomInfo.nAduid;
	jData["ActionType"] = nActionType;
	jData["LiveID"] = tagRoomInfo.nLiveID;
	jData["LiveUserID"] = tagRoomInfo.nUserID;
	
	jData["RobotID"] = tagBotInfo.nUserID;
	jData["RobotName"] = TFC::CTCodeConversion::AnsiToUTF8(tagBotInfo.strUserName.c_str()).c_str();
	jData["RoomID"] = tagRoomInfo.nRoomID;

	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}

	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d%d%d%d%d%d%d", tagRoomInfo.nAduid, nActionType, tagRoomInfo.nLiveID, tagRoomInfo.nUserID,nR, 
		tagBotInfo.nUserID, tagRoomInfo.nRoomID);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;
		
		string strGet = DesAndBase64Decode(strResponse);
		//_DebugPrintA("strGet=%s", strGet.c_str());
		if (reader.parse(strGet.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Msg"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Msg"];

				if (!jsonRoot["Data"]["YunXinSign"].isNull())
					PraseIMMSG(jsonWriter.write(jsonRoot["Data"]["YunXinSign"]).c_str(), pTagRet);
				return jsonWriter.write(jsonData);
			}
			else
			{
				if (!jsonRoot["Msg"].isNull())
				{
					wstring wstrMsg = TFC::CTCodeConversion::AnsiToUnicode(jsonRoot["Msg"].asCString());
					OutputDebugStringW(wstrMsg.c_str());
					AfxMessageBox(wstrMsg.c_str());
				}
			}
		}
	}
	return "";
}

std::string CWebCommWrapper::RobotEnterRoom(TAGROOMINFO tagRoomInfo, int nFlag)
{
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/RobotEnterRoom", m_szUrlHead);

	Json::Value jData;
	jData["Aduid"] = tagRoomInfo.nAduid;
	jData["AdName"] = TFC::CTCodeConversion::AnsiToUTF8(tagRoomInfo.strAName.c_str()).c_str();
	jData["ActionType"] = nFlag;
	jData["LiveID"] = tagRoomInfo.nLiveID;
	jData["RoomID"] = tagRoomInfo.nRoomID;
	jData["LiveUserID"] = tagRoomInfo.nUserID;
	jData["RobotID"] = 0;
	
	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}
	OutputDebugStringA(szArray);
	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d%d%d%d%d%d%d", tagRoomInfo.nAduid, nFlag, tagRoomInfo.nLiveID, tagRoomInfo.nUserID, nR,
		0, tagRoomInfo.nRoomID);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;

		string strGet = DesAndBase64Decode(strResponse);
		_DebugPrintA("RobotEnterRoom=%s", strGet.c_str());
		if (reader.parse(strGet.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				&& !jsonRoot["Msg"].isNull()
				)
			{
				Json::FastWriter jsonWriter;
				Json::Value jsonData = jsonRoot["Msg"];

				return jsonWriter.write(jsonData);
			}
			else
			{
				if (!jsonRoot["Msg"].isNull())
				{
					wstring wstrMsg = TFC::CTCodeConversion::AnsiToUnicode(jsonRoot["Msg"].asCString());
					OutputDebugStringW(wstrMsg.c_str());
					AfxMessageBox(wstrMsg.c_str());
				}
			}
		}
	}
	return "";
}


BOOL CWebCommWrapper::RobotLog(TAGROOMINFO tagRoomInfo, int nBotID, int nMsgType, string strMsgContent, TAGCHATINFO* pTagRet, TAGPICINFO* pPicInfo)
{
	BOOL bRet = FALSE;
	char szUrl[MAX_PATH];
	memset(szUrl, 0, sizeof(szUrl));
	sprintf(szUrl, "%s/Robotlogv2", m_szUrlHead);

	Json::Value jData;
	//jData["AdName"] = tagRoomInfo.strAName.c_str();
	jData["AdName"] = TFC::CTCodeConversion::AnsiToUTF8(tagRoomInfo.strAName.c_str()).c_str();
	jData["Aduid"] = tagRoomInfo.nAduid;
	jData["LiveID"] = tagRoomInfo.nLiveID;
	jData["LiveUserID"] = tagRoomInfo.nUserID;
	//jData["MsgContent"] = strMsgContent.c_str();
	jData["MsgContent"] = TFC::CTCodeConversion::AnsiToUTF8(strMsgContent.c_str()).c_str();
	jData["MsgType"] = nMsgType;

	jData["RobotID"] =  nBotID;//1000024176;//
	jData["RoomID"] = tagRoomInfo.nRoomID;

	jData["TeamCode"] = m_strTeamCode.c_str();
	jData["SessionID"] = m_strSessionID.c_str();
	
	jData["YXAccessID"] = tagRoomInfo.strAccessID.c_str();

	if (nMsgType == EN_MSG_PIC)
	{
		jData["FaceName"] = TFC::CTCodeConversion::AnsiToUTF8(strMsgContent.c_str()).c_str();
		jData["FaceFormat"] = TFC::CTCodeConversion::AnsiToUTF8(pPicInfo->strFaceFormat.c_str()).c_str();
		jData["FaceFileName"] = TFC::CTCodeConversion::AnsiToUTF8(pPicInfo->strFaceFileName.c_str()).c_str();
		jData["FaceFilePath"] = TFC::CTCodeConversion::AnsiToUTF8(pPicInfo->strFaceFilePath.c_str()).c_str();
		jData["FaceContent"] = TFC::CTCodeConversion::AnsiToUTF8(pPicInfo->strFaceContent.c_str()).c_str();
		
		jData["FaceLabAndName"] = TFC::CTCodeConversion::AnsiToUTF8(pPicInfo->strFaceLabAndName.c_str()).c_str();
		jData["FaceType"] = TFC::CTCodeConversion::AnsiToUTF8(pPicInfo->strFaceType.c_str()).c_str();
	}

	Json::FastWriter jWriter;

	string strSrc = jWriter.write(jData);
	char szArray[MAX_POST_PARAM_LEN] = { NULL };
	memcpy(szArray, strSrc.c_str(), strSrc.length());
	if (szArray[strlen(szArray) - 1] == '\n')
	{
		szArray[strlen(szArray) - 1] = NULL;
	}
	_DebugPrintA(szArray);
	srand((unsigned long)time(NULL));
	int nR = rand() % _countof(GLOBLE_KEY);
	CStringA cstrParams;
	cstrParams.Format("%d%d%d%d%d%d%d%d%d", tagRoomInfo.strAName, tagRoomInfo.nAduid, 
		tagRoomInfo.nLiveID, tagRoomInfo.nUserID, strMsgContent.c_str(), nMsgType, nR, 
		nBotID, tagRoomInfo.nRoomID);
	string strParams = cstrParams.GetBuffer();
	cstrParams.ReleaseBuffer();

	std::string strPostContent = BuildPostContent(nR, szArray, strParams);
	std::string strResponse = "";

	if (TFC::CTHttp::PostUrlContentA(szUrl, strPostContent.c_str(), strPostContent.length(), strResponse))
	{
		Json::Reader reader;
		Json::Value jsonRoot;
		
		string strGet = DesAndBase64Decode(strResponse);
		//_DebugPrintA(strGet.c_str());
		if (reader.parse(strGet.c_str(), jsonRoot))
		{
			if (!jsonRoot["Code"].isNull()
				&& jsonRoot["Code"].asInt() == 0
				)
			{
				if (!jsonRoot["Data"].isNull())
				{
					bRet = TRUE;
					Json::FastWriter jsonWriter;
					if (!jsonRoot["Data"].isNull())
						PraseIMMSG(jsonWriter.write(jsonRoot["Data"]).c_str(), pTagRet);
					return bRet;
				}
				
			}
			else
			{
				if (jsonRoot["Code"].asInt() == 1002)
				{
					AfxMessageBox(L"SessionID已失效,请重新登录!");
					ExitProcess(0);
				}
				if (!jsonRoot["Msg"].isNull())
				{
					wstring wstrMsg = TFC::CTCodeConversion::AnsiToUnicode(jsonRoot["Msg"].asCString());
					OutputDebugStringW(wstrMsg.c_str());
					AfxMessageBox(wstrMsg.c_str());
				}
					
			}
		}
	}
	else
	{
		OutputDebugStringW(L"获取服务器数据失败");
		AfxMessageBox(L"获取服务器数据失败");
	}
	return bRet;
}

BOOL CWebCommWrapper::DownloadFile(string strFileName, string strUrl)
{
	BOOL bRet = FALSE;
	string strRet;
	TFC::CTHttpDownload oDownloader;
	if (oDownloader.DownloadA(strUrl.c_str(), strFileName.c_str()))
	{
		bRet = TRUE;	
	}
	return bRet;
}

BOOL CWebCommWrapper::PraseGifts(string strInfo)
{
	BOOL bRet = FALSE;
	Json::Reader reader;
	Json::Value jsonRoot;
	
	if (reader.parse(strInfo.c_str(), jsonRoot))
	{
		if (m_bHaveGetGift)
		{
			return TRUE;
		}

		m_bHaveGetGift = TRUE;

		bRet = TRUE;
		for (int i = 0; i < jsonRoot.size(); i++)
		{
			TAGGIFTINFO *pTag = new TAGGIFTINFO();
			pTag->nGiftID = jsonRoot[i]["GiftID"].asInt();
			pTag->nGiftType = jsonRoot[i]["GiftType"].asInt();
			pTag->strGiftName = jsonRoot[i]["GiftName"].asString();
			pTag->strGiftDescription = jsonRoot[i]["GiftDescription"].asString();
			pTag->strGiftImage = jsonRoot[i]["GiftImage"].asString();

			//wstring wstrTempDir = CStringHelper::GetBinDir() + L"gift\\";
			//TFC::CTDirectory::MkDirW(wstrTempDir.c_str());
			//wstring wstrFile = wstrTempDir + CStringHelper::I2W(pTag->nGiftID) + L".gif";
			//string strFile = TFC::CTCodeConversion::UnicodeToAnsi((TCHAR*)(wstrFile.c_str()));// ((TCHAR*)(wstrZipFaceFile.c_str()));
			//BOOL bRet = DownloadFile(strFile, pTag->strGiftImage);

			pTag->nGiftPrice = jsonRoot[i]["GiftPrice"].asInt();
			pTag->nGiftEffectType = jsonRoot[i]["GiftEffectType"].asInt();
			m_mapGifts.insert(MAPGIFT::value_type(jsonRoot[i]["GiftPrice"].asInt(), pTag));
		}
	}
	return bRet;
}

BOOL CWebCommWrapper::PraseZipFaceInfo(string strInfo)
{
	BOOL bRet = FALSE;
	Json::Reader reader;
	Json::Value jsonRoot;
	if (reader.parse(strInfo.c_str(), jsonRoot))
	{
		m_tagZipFaceInfo.nQVersion = jsonRoot["QVersion"].asInt();
		m_tagZipFaceInfo.strQZipPath = jsonRoot["QZipPath"].asString();

		wstring wstrTempDir = CStringHelper::GetBinDir() + L"temp\\";
		TFC::CTDirectory::MkDirW(wstrTempDir.c_str());
		wstring wstrCfg = CStringHelper::GetBinDir() + CFG_INI;
		int nQVersion = GetPrivateProfileInt(L"config", L"QVersion", 0, wstrCfg.c_str());
		if ( m_tagZipFaceInfo.nQVersion > nQVersion )
		{
			wstring wstrZipFaceFile = wstrTempDir + L"face.zip";
			string strFile = TFC::CTCodeConversion::UnicodeToAnsi((TCHAR*)(wstrZipFaceFile.c_str()));// ((TCHAR*)(wstrZipFaceFile.c_str()));
			TFC::CTFile::DeleteW(wstrZipFaceFile.c_str());
			BOOL bRet = DownloadFile(strFile , m_tagZipFaceInfo.strQZipPath);
			if (bRet)
			{
				UnzipFile(wstrZipFaceFile.c_str(), wstrTempDir.c_str());

				// 保存版本号
				TCHAR tszMaxId[MAX_PATH] = { NULL };
				_itow(m_tagZipFaceInfo.nQVersion, tszMaxId, 10);
				WritePrivateProfileString(L"config", L"QVersion", tszMaxId, wstrCfg.c_str());
			}
			else
			{
				OutputDebugString(L"图片资源下载失败");
				OutputDebugStringA(m_tagZipFaceInfo.strQZipPath.c_str());
			}
		}
		else
		{
			OutputDebugString(L"图片资源版本已是最新");
		}
		// 获取贴图结构
		wstring wstrPicCfg = wstrTempDir + L"config.txt";
		PrasePic(wstrPicCfg);
	}
	return bRet;
}

BOOL CWebCommWrapper::PrasePic(wstring wstrFilePath)
{
	BOOL bRet = FALSE;
	DWORD dwFileSize = TFC::CTFile::GetFileSizeW(wstrFilePath.c_str());
	if (dwFileSize > 0)
	{
		string strFile = TFC::CTCodeConversion::UnicodeToAnsi((TCHAR*)(wstrFilePath.c_str()));// ((TCHAR*)(wstrZipFaceFile.c_str()));

		char *szList = (char*)calloc(dwFileSize + 1, 1);
		FILE* fn = TFC::CTFile::OpenA(strFile.c_str(), BIN_READ);
		TFC::CTFile::Read(fn, (BYTE*)szList, dwFileSize);
		int nLen = strlen(szList);

		Json::Reader reader;
		Json::Value jsonRoot;
		if (reader.parse(szList, jsonRoot))
		{
			if (!jsonRoot["LiveFaceList"].isNull())
			{
				for (int i = 0; i < jsonRoot["LiveFaceList"].size();i++)
				{
					Json::Value jsonNode;
					jsonNode = jsonRoot["LiveFaceList"][i];
					
					if (!jsonNode["FaceList"].isNull())
					{
						for (int j = 0; j < jsonNode["FaceList"].size(); j++)
						{
							Json::Value jsonTag;
							jsonTag = jsonNode["FaceList"][j];

							TAGPICINFO *pTag = new TAGPICINFO();
							pTag->strFaceName = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceName"].asString().c_str());
							pTag->strFaceFormat = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceFormat"].asString().c_str());
							pTag->strFaceFileName = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceFileName"].asString().c_str());
							pTag->strFaceFilePath = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceFilePath"].asString().c_str());
							pTag->strFaceContent = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceContent"].asString().c_str());
							pTag->strFaceLabAndName = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceLabAndName"].asString().c_str());
							pTag->strFaceType = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceType"].asString().c_str());
							m_mapPic.insert(MAPPIC::value_type(pTag->strFaceName, pTag));
						}
					}
				}
			}
			if (!jsonRoot["ActionFaceList"].isNull())
			{
				Json::Value jsonAction;
				jsonAction = jsonRoot["ActionFaceList"];
				for (int i = 0; i < jsonAction["FaceList"].size(); i++)
				{
					Json::Value jsonNode;
					jsonNode = jsonAction["FaceList"][i];

					if (!jsonAction["FaceList"].isNull())
					{
						for (int j = 0; j < jsonAction["FaceList"].size(); j++)
						{
							Json::Value jsonTag;
							jsonTag = jsonAction["FaceList"][j];

							TAGPICINFO *pTag = new TAGPICINFO();
							pTag->strFaceName = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceName"].asString().c_str());
							pTag->strFaceFormat = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceFormat"].asString().c_str());
							pTag->strFaceFileName = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceFileName"].asString().c_str());
							pTag->strFaceFilePath = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceFilePath"].asString().c_str());
							pTag->strFaceContent = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceContent"].asString().c_str());
							pTag->strFaceLabAndName = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceLabAndName"].asString().c_str());
							pTag->strFaceType = TFC::CTCodeConversion::UTF8ToAnsi(jsonTag["FaceType"].asString().c_str());
							m_mapAction.insert(MAPPIC::value_type(pTag->strFaceName, pTag));
						}
					}
				}
			}
			//m_mapChatContents.insert(MAPCHATCONTENT::value_type(jsonChatContentList[i]["Title"].asString(), pVec));
			bRet = TRUE;
		}

		free(szList);
		TFC::CTFile::Close(fn);
	}
	return bRet;
}

BOOL CWebCommWrapper::PraseIMMSG(string strInfo, TAGCHATINFO* pTagRet)
{
	BOOL bRet = FALSE;
	Json::Reader reader;
	Json::Value jsonRoot;
	if (reader.parse(strInfo.c_str(), jsonRoot))
	{
		bRet = TRUE;
		pTagRet->strScene = jsonRoot["Scene"].asString();
		pTagRet->strTo = jsonRoot["To"].asString();

		
		{
			pTagRet->strIM = jsonRoot["ImMsg"]["IM"].asString();
			pTagRet->strIMMsg = jsonRoot["ImMsg"]["IMMsg"].asString();// TFC::CTCodeConversion::UTF8ToAnsi(jsonRoot["ImMsg"]["IMMsg"].asString().c_str());
		}
	}
	return bRet;
}

string CWebCommWrapper::DesAndBase64Encode(char* pszData)
{
	BYTE* pbyEncrypt = (BYTE*)calloc(strlen(pszData) + 9, 1);
	des_context ctx;
	des_set_key(&ctx, DES_KEY);
	int nEncryptLen = des_cbc_encrypt(&ctx, (const BYTE*)pszData, strlen(pszData), pbyEncrypt, DES_VI);

	int nBase64Len = TFC::CTCodeConversion::Base64Encode(pbyEncrypt, nEncryptLen, NULL, NULL);
	char* pcsBase64 = (char*)calloc(nBase64Len + 1, sizeof(char));
	TFC::CTCodeConversion::Base64Encode(pbyEncrypt, nEncryptLen, (BYTE*)pcsBase64, nBase64Len);

	string strGet = pcsBase64;
	free(pbyEncrypt);
	free(pcsBase64);

	return strGet;
}

string CWebCommWrapper::DesAndBase64Decode(string strData)
{
	int nBase64Len = TFC::CTCodeConversion::Base64Decode((const BYTE*)strData.c_str(), strData.length(), NULL, NULL);
	char* pcsBase64 = (char*)calloc(nBase64Len + 1, sizeof(char));
	TFC::CTCodeConversion::Base64Decode((const BYTE*)strData.c_str(), strData.length(), (BYTE*)pcsBase64, nBase64Len);

	BYTE* pbyDecrypt = (BYTE*)calloc(nBase64Len + 9, 1);
	des_context ctx;
	des_set_key(&ctx, DES_KEY);
	int nEncryptLen = des_cbc_decrypt(&ctx, (const BYTE*)pcsBase64, nBase64Len, pbyDecrypt, DES_VI);

	string strDecrypt = (char*)pbyDecrypt;
	string strGet = TFC::CTCodeConversion::UTF8ToAnsi(strDecrypt.c_str());

	free(pbyDecrypt);
	free(pcsBase64);

	return strGet;
}

string CWebCommWrapper::GetUrlParam()
{
	if (m_strDomain.length() <= 0)
	{
		m_strProtocol = "http";
		m_strDomain = "96k.com";

		std::string strUrl = "http://96k.84fk.com/xydomainsetting.txt";
		std::string strRet;
		if (TFC::CTHttp::GetUrlContentA(strUrl.c_str(), strRet))
		{
			Json::Reader reader;
			Json::Value jsonRoot;

			if (reader.parse(strRet.c_str(), jsonRoot))
			{
				if (!jsonRoot["DefaultSet"].isNull())
				{
					if (!jsonRoot["DefaultSet"]["Protocol"].isNull())
					{
						m_strProtocol = jsonRoot["DefaultSet"]["Protocol"].asString();
					}
					if (!jsonRoot["DefaultSet"]["Domain"].isNull())
					{
						m_strDomain = jsonRoot["DefaultSet"]["Domain"].asString();
					}
				}
			}
			else
			{
				//LogToFile("解析失败", LOG_NAME);
			}
		}
	}
	return m_strDomain;
}

BOOL CWebCommWrapper::GetIdleYX(TAGYX& tag)
{
	BOOL bRet = FALSE;
	int nCount = m_vecYX.size();
	for (int i = 0; i < nCount; i++)
	{
		if (m_vecYX[i].bUsed == FALSE)
		{
			m_vecYX[i].bUsed = TRUE;
			tag = m_vecYX[i];
			bRet = TRUE;
			break;
		}
	}
	return bRet;
}

void CWebCommWrapper::SetIdleYX(string strAccessID)
{
	int nCount = m_vecYX.size();
	for (int i = 0; i < nCount; i++)
	{
		if (m_vecYX[i].strAccessID == strAccessID)
		{
			m_vecYX[i].bUsed = FALSE;
			break;
		}
	}
}