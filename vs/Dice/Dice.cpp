#include <Windows.h>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <thread>
#include <queue>
#include <chrono>
#include <mutex>
#include "APPINFO.h"
#include "RD.h"
#include "CQEVE_ALL.h"
#include "CQTools.h"

using namespace std;
using namespace CQ;
struct MsgType {
	long long target = 0;
	string msg = "";
	int type;
	//0-Private 1-Group 2-Discuss
};
queue<MsgType> SendMsgQueue;
mutex mutexMsg;
inline void SendMsg(MsgType msg) {
	if (msg.type == PrivateMsg) {
		sendPrivateMsg(msg.target, msg.msg);
	}
	else if (msg.type == GroupMsg) {
		sendGroupMsg(msg.target, msg.msg);
	}
	else if (msg.type == DiscussMsg) {
		sendDiscussMsg(msg.target, msg.msg);
	}
}

map<long long, RP> JRRP;
map<long long, int> DefaultDice;
map<pair<long long, long long>, string> GroupName;
map<pair<long long, long long>, string> DiscussName;
set<long long> DisabledGroup;
set<long long> DisabledDiscuss;
set<long long> DisabledJRRPGroup;
set<long long> DisabledJRRPDiscuss;
set<long long> DisabledMEGroup;
set<long long> DisabledMEDiscuss;
set<long long> DisabledOBGroup;
set<long long> DisabledOBDiscuss;
multimap<long long, long long> ObserveGroup;
multimap<long long, long long> ObserveDiscuss;
string strFileLoc;
bool Enabled = false;
bool MsgSendThreadStarted = false;
EVE_Enable(__eventEnable){
	while (MsgSendThreadStarted)Sleep(10);/*Wait until the thread terminates*/;
	thread MsgSend
	{ 
		[&]{
			Enabled = true;
			MsgSendThreadStarted = true;
			while (Enabled) {
				MsgType msgMsg;
				mutexMsg.lock();
				if (!SendMsgQueue.empty()) {
					msgMsg = SendMsgQueue.front();
					SendMsgQueue.pop();
				}
				mutexMsg.unlock();
				if (!msgMsg.msg.empty()) {
					SendMsg(msgMsg);
				}
				this_thread::sleep_for(chrono::milliseconds(20));
			}
			MsgSendThreadStarted = false;
		}
	};
	MsgSend.detach();
	strFileLoc = getAppDirectory();
	ifstream ifstreamGroupName(strFileLoc + "GroupName.RDconf");
	if (ifstreamGroupName) {
		long long QQ;
		long long Group;
		string name;
		pair<long long, long long> pairQQGroup;
		while (ifstreamGroupName >> QQ) {
			ifstreamGroupName >> Group >> name;
			pairQQGroup.first = QQ;
			pairQQGroup.second = Group;
			GroupName[pairQQGroup] = name;
		}
	}
	ifstreamGroupName.close();
	ifstream ifstreamDiscussName(strFileLoc + "DiscussName.RDconf");
	if (ifstreamDiscussName) {
		long long QQ;
		long long Discuss;
		string name;
		pair<long long, long long> pairQQDiscuss;
		while (ifstreamDiscussName >> QQ) {
			ifstreamDiscussName >> Discuss >> name;
			pairQQDiscuss.first = QQ;
			pairQQDiscuss.second = Discuss;
			DiscussName[pairQQDiscuss] = name;
		}
	}
	ifstreamDiscussName.close();
	ifstream ifstreamDisabledGroup(strFileLoc + "DisabledGroup.RDconf");
	if (ifstreamDisabledGroup) {
		long long Group;
		while (ifstreamDisabledGroup >> Group) {
			DisabledGroup.insert(Group);
		}
	}
	ifstreamDisabledGroup.close();
	ifstream ifstreamDisabledDiscuss(strFileLoc + "DisabledDiscuss.RDconf");
	if (ifstreamDisabledDiscuss) {
		long long Discuss;
		while (ifstreamDisabledDiscuss >> Discuss) {
			DisabledDiscuss.insert(Discuss);
		}
	}
	ifstreamDisabledDiscuss.close();
	ifstream ifstreamDisabledJRRPGroup(strFileLoc + "DisabledJRRPGroup.RDconf");
	if (ifstreamDisabledJRRPGroup) {
		long long Group;
		while (ifstreamDisabledJRRPGroup >> Group) {
			DisabledJRRPGroup.insert(Group);
		}
	}
	ifstreamDisabledJRRPGroup.close();
	ifstream ifstreamDisabledJRRPDiscuss(strFileLoc + "DisabledJRRPDiscuss.RDconf");
	if (ifstreamDisabledJRRPDiscuss) {
		long long Discuss;
		while (ifstreamDisabledJRRPDiscuss >> Discuss) {
			DisabledJRRPDiscuss.insert(Discuss);
		}
	}
	ifstreamDisabledJRRPDiscuss.close();
	ifstream ifstreamDisabledMEGroup(strFileLoc + "DisabledMEGroup.RDconf");
	if (ifstreamDisabledMEGroup) {
		long long Group;
		while (ifstreamDisabledMEGroup >> Group) {
			DisabledMEGroup.insert(Group);
		}
	}
	ifstreamDisabledMEGroup.close();
	ifstream ifstreamDisabledMEDiscuss(strFileLoc + "DisabledMEDiscuss.RDconf");
	if (ifstreamDisabledMEDiscuss) {
		long long Discuss;
		while (ifstreamDisabledMEDiscuss >> Discuss) {
			DisabledMEDiscuss.insert(Discuss);
		}
	}
	ifstreamDisabledMEDiscuss.close();
	ifstream ifstreamDisabledOBGroup(strFileLoc + "DisabledOBGroup.RDconf");
	if (ifstreamDisabledOBGroup) {
		long long Group;
		while (ifstreamDisabledOBGroup >> Group) {
			DisabledOBGroup.insert(Group);
		}
	}
	ifstreamDisabledOBGroup.close();
	ifstream ifstreamDisabledOBDiscuss(strFileLoc + "DisabledOBDiscuss.RDconf");
	if (ifstreamDisabledOBDiscuss) {
		long long Discuss;
		while (ifstreamDisabledOBDiscuss >> Discuss) {
			DisabledOBDiscuss.insert(Discuss);
		}
	}
	ifstreamDisabledOBDiscuss.close();
	ifstream ifstreamObserveGroup(strFileLoc + "ObserveGroup.RDconf");
	if (ifstreamObserveGroup) {
		long long Group, QQ;
		while (ifstreamObserveGroup >> Group >> QQ) {
			ObserveGroup.insert(make_pair(Group, QQ));
		}
	}
	ifstreamObserveGroup.close();

	ifstream ifstreamObserveDiscuss(strFileLoc + "ObserveDiscuss.RDconf");
	if (ifstreamObserveDiscuss) {
		long long Discuss, QQ;
		while (ifstreamObserveDiscuss >> Discuss >> QQ) {
			ObserveDiscuss.insert(make_pair(Discuss, QQ));
		}
	}
	ifstreamObserveDiscuss.close();
	ifstream ifstreamJRRP(strFileLoc + "JRRP.RDconf");
	if (ifstreamJRRP) {
		long long QQ;
		int Val;
		string strDate;
		while (ifstreamJRRP >> QQ >> strDate >> Val) {
			JRRP[QQ].Date = strDate;
			JRRP[QQ].RPVal = Val;
		}
	}
	ifstreamJRRP.close();
	ifstream ifstreamDefault(strFileLoc + "Default.RDconf");
	if (ifstreamDefault) {
		long long QQ;
		int DefVal;
		while (ifstreamDefault >> QQ >> DefVal) {
			DefaultDice[QQ] = DefVal;
		}
	}
	ifstreamDefault.close();
	return 0;
}


EVE_PrivateMsg_EX(__eventPrivateMsg) {
	init(eve.message);
	if (eve.message[0] != '.')return;
	int intMsgCnt = 1;
	while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
	eve.message_block();
	string strNickName = getStrangerInfo(eve.fromQQ).nick;
	string strLowerMessage = eve.message;
	transform(strLowerMessage.begin(), strLowerMessage.end(), strLowerMessage.begin(), tolower);
	if (strLowerMessage.substr(intMsgCnt, 4) == "help") {
		
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strHlpMsg,PrivateMsg });
		mutexMsg.unlock();
		
	}
	else if (strLowerMessage[intMsgCnt] == 'w') {
		intMsgCnt++;
		bool boolDetail = false;
		if (strLowerMessage[intMsgCnt] == 'w') {
			intMsgCnt++;
			boolDetail = true;
		}
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;

		string strMainDice;
		string strReason;
		int intTmpMsgCnt;
		for (intTmpMsgCnt = intMsgCnt; intTmpMsgCnt != eve.message.length() && eve.message[intTmpMsgCnt] != ' '; intTmpMsgCnt++) {
			if (!isdigit(strLowerMessage[intTmpMsgCnt]) && strLowerMessage[intTmpMsgCnt] != 'd' &&strLowerMessage[intTmpMsgCnt] != 'k'&&strLowerMessage[intTmpMsgCnt] != 'p'&&strLowerMessage[intTmpMsgCnt] != 'b'&&strLowerMessage[intTmpMsgCnt] != 'f'&& strLowerMessage[intTmpMsgCnt] != '+'&&strLowerMessage[intTmpMsgCnt] != '-' && strLowerMessage[intTmpMsgCnt] != '#'&& strLowerMessage[intTmpMsgCnt] != 'a') {
				break;
			}
		}
		strMainDice = strLowerMessage.substr(intMsgCnt, intTmpMsgCnt - intMsgCnt);
		while (eve.message[intTmpMsgCnt] == ' ')intTmpMsgCnt++;
		strReason = eve.message.substr(intTmpMsgCnt);


		int intTurnCnt = 1;
		if (strMainDice.find("#") != string::npos) {
			string strTurnCnt = strMainDice.substr(0, strMainDice.find("#"));
			if (strTurnCnt.empty())strTurnCnt = "1";
			strMainDice = strMainDice.substr(strMainDice.find("#") + 1);
			RD rdTurnCnt(strTurnCnt, eve.fromQQ);
			int intRdTurnCntRes = rdTurnCnt.Roll();
			if (intRdTurnCntRes == Value_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strValueErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == Input_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strInputErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroDice_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroDiceErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroType_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroTypeErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == DiceTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strDiceTooBigErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == TypeTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strTypeTooBigErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == AddDiceVal_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAddDiceValErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes != 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strUnknownErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			if (rdTurnCnt.intTotal > 10) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strRollTimeExceeded ,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (rdTurnCnt.intTotal <= 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strRollTimeErr ,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			intTurnCnt = rdTurnCnt.intTotal;
			if (strTurnCnt.find("d") != string::npos) {
				string strTurnNotice = strNickName + "的掷骰轮数: " + rdTurnCnt.FormShortString() + "轮";

				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strTurnNotice,PrivateMsg });
				mutexMsg.unlock();

			}
		}
		string strFirstDice = strMainDice.substr(0, strMainDice.find('+') < strMainDice.find('-') ? strMainDice.find('+') : strMainDice.find('-'));
		bool boolAdda10 = true;
		for (auto i : strFirstDice) {
			if (!isdigit(i)) {
				boolAdda10 = false;
				break;
			}
		}
		if (boolAdda10)strMainDice.insert(strFirstDice.length(), "a10");
		RD rdMainDice(strMainDice, eve.fromQQ);
		
		int intFirstTimeRes = rdMainDice.Roll();
		if (intFirstTimeRes == Value_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strValueErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == Input_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strInputErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroDice_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroDiceErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroType_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroTypeErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == DiceTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strDiceTooBigErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == TypeTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strTypeTooBigErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == AddDiceVal_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strAddDiceValErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strUnknownErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (!boolDetail && intTurnCnt!=1) {
			string strAns = strNickName + "骰出了: " + to_string(intTurnCnt) + "次" + rdMainDice.strDice + ": { ";
			if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
			vector<int> vintExVal;
			while (intTurnCnt--) {
				rdMainDice.Roll();
				strAns += to_string(rdMainDice.intTotal);
				if (intTurnCnt != 0)strAns += ",";
				if ((rdMainDice.strDice == "D100" || rdMainDice.strDice == "1D100") && (rdMainDice.intTotal <= 5 || rdMainDice.intTotal >= 96))vintExVal.push_back(rdMainDice.intTotal);
			}
			strAns += " }";
			if (!vintExVal.empty()) {
				strAns += ",极值: ";
				for (auto it = vintExVal.cbegin(); it != vintExVal.cend(); it++) {
					strAns += to_string(*it);
					if (it != vintExVal.cend() - 1)strAns += ",";
				}
			}
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
			mutexMsg.unlock();
		}
		else {
			while (intTurnCnt--) {
				rdMainDice.Roll();
				string strAns = strNickName + "骰出了: " + (boolDetail ? rdMainDice.FormCompleteString() :rdMainDice.FormShortString());
				if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
				mutexMsg.unlock();

			}
		}
		

	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "ti") {
		string strAns = strNickName + "的疯狂发作-临时症状:\n";
		TempInsane(strAns);
		
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
		mutexMsg.unlock();
		
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "li") {
		string strAns = strNickName + "的疯狂发作-总结症状:\n";
		LongInsane(strAns);
		
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
		mutexMsg.unlock();
		
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "sc") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string SanCost = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		intMsgCnt += SanCost.length();
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string San = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (SanCost == "" || SanCost.find("/") == string::npos) {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSCInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		if (San == "") {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSanInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		RD rdTest1(SanCost.substr(0, SanCost.find("/")));
		RD rdTest2(SanCost.substr(SanCost.find("/") + 1));
		if (rdTest1.Roll() != 0 || rdTest2.Roll() != 0) {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSCInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		for (auto i : San)
			if (!isdigit(i)) {
				
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strSanInvalid,PrivateMsg });
				mutexMsg.unlock();
				
				return;
			}
		if (San.length() >= 3) {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSanInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		int intSan = Convert<int> (San);
		if (intSan == 0) {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSanInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		string strAns = strNickName + "的Sancheck:\n1D100=";
		RD rdD100("D100");
		rdD100.Roll();
		strAns += to_string(rdD100.intTotal);

		if (rdD100.intTotal <= intSan) {
			strAns += " 成功\n你的San值减少" + SanCost.substr(0, SanCost.find("/"));
			RD rdSan(SanCost.substr(0, SanCost.find("/")));
			rdSan.Roll();
			if (SanCost.substr(0, SanCost.find("/")).find("d") != string::npos)strAns += "=" + to_string(rdSan.intTotal);
			strAns += +"点,当前剩余" + to_string(intSan - rdSan.intTotal) + "点";
		}
		else {
			strAns += " 失败\n你的San值减少" + SanCost.substr(SanCost.find("/") + 1);
			RD rdSan(SanCost.substr(SanCost.find("/") + 1));
			rdSan.Roll();
			if (SanCost.substr(SanCost.find("/") + 1).find("d") != string::npos)strAns += "=" + to_string(rdSan.intTotal);
			strAns += +"点,当前剩余" + to_string(intSan - rdSan.intTotal) + "点";
		}
		
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
		mutexMsg.unlock();
		
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "en") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strSkillName;
		while (intMsgCnt != eve.message.length()&&!isdigit(eve.message[intMsgCnt]) && !isspace(eve.message[intMsgCnt])) {
			strSkillName += eve.message[intMsgCnt];
			intMsgCnt++;
		}
		while (isspace(eve.message[intMsgCnt]))intMsgCnt++;
		string strCurrentValue;
		while (isdigit(eve.message[intMsgCnt])) {
			strCurrentValue += eve.message[intMsgCnt];
			intMsgCnt++;
		}
		if (strCurrentValue.empty()) {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strEnValInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		if (strCurrentValue.length() >= 3) {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strEnValInvalid,PrivateMsg });
			mutexMsg.unlock();
			
			return;
		}
		int intCurrentVal = Convert<int>(strCurrentValue);
		if (intCurrentVal == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strEnValInvalid,PrivateMsg });
			mutexMsg.unlock();

			return;
		}
		string strAns = strNickName + "的"+ strSkillName +"增强或成长检定:\n1D100=";
		RD rdD100("D100");
		rdD100.Roll();
		strAns += to_string(rdD100.intTotal);

		if (rdD100.intTotal <= intCurrentVal) {
			strAns += " 失败!\n你的" + (strSkillName.empty() ? "属性或技能值" : strSkillName) + "没有变化!";
		}
		else {
			strAns += " 成功!\n你的" + (strSkillName.empty() ? "属性或技能值" : strSkillName) + "增加1D10=";
			RD rdAddVal("D10");
			rdAddVal.Roll();
			strAns += to_string(rdAddVal.intTotal) + "点,当前为" + to_string(intCurrentVal + rdAddVal.intTotal) + "点";
		}
		
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
		mutexMsg.unlock();
		
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "jrrp") {
		char cstrDate[100] = {};
		time_t time_tTime = 0;
		time(&time_tTime);
		tm tmTime;
		localtime_s(&tmTime, &time_tTime);
		strftime(cstrDate, 100, "%F", &tmTime);
		if (JRRP.count(eve.fromQQ) && JRRP[eve.fromQQ].Date == cstrDate) {
			string strReply = strNickName + "今天的人品值是:" + to_string(JRRP[eve.fromQQ].RPVal);
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
			mutexMsg.unlock();
			
		}
		else {
			normal_distribution<double> NormalDistribution(60, 15);
			mt19937 Generator(static_cast<unsigned int> (GetCycleCount()));
			int JRRPRes;
			do {
				JRRPRes = static_cast<int> (NormalDistribution(Generator));
			} while (JRRPRes <= 0 || JRRPRes > 100);
			JRRP[eve.fromQQ].Date = cstrDate;
			JRRP[eve.fromQQ].RPVal = JRRPRes;
			string strReply(strNickName + "今天的人品值是:" + to_string(JRRP[eve.fromQQ].RPVal));
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
			mutexMsg.unlock();
			
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "rules") {
		intMsgCnt += 5;
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string strSearch = eve.message.substr(intMsgCnt);
		for (auto &n : strSearch)n = toupper(n);
		for (int i = 0; i != strSearch.length(); ++i)if (strSearch[i] == ' ') {
			strSearch.erase(strSearch.begin() + i);
			i--;
		}
		strSearch = "||" + strSearch + "||";
		int Loc = strRules.find(strSearch);
		if (Loc != string::npos || strSearch == "||战斗||" || strSearch == "||战斗流程||" || strSearch == "||战斗伤害||") {
			if (strSearch == "||战斗||" || strSearch == "||战斗流程||") {
				string IMAGE = "[CQ:image,file=";
				string IMAGENAME = "Combat.jpg";
				IMAGE += msg_encode(IMAGENAME, true) + "]";
				int res = sendPrivateMsg(eve.fromQQ, IMAGE);
				if (res < 0) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ, "未找到对应的规则信息!",PrivateMsg });
					mutexMsg.unlock();
				}
			}
			else if (strSearch == "||战斗伤害||") {
				string IMAGE = "[CQ:image,file=";
				string IMAGENAME = "Combat1.jpg";
				IMAGE += msg_encode(IMAGENAME, true) + "]";
				int res = sendPrivateMsg(eve.fromQQ, IMAGE);
				if (res < 0) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ, "未找到对应的规则信息!",PrivateMsg });
					mutexMsg.unlock();
				}
			}
			else {
				int LocStart = strRules.find("]", Loc) + 1;
				string strReply = strRules.substr(LocStart, strRules.find("[", LocStart) - LocStart - 1);
				
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
				mutexMsg.unlock();
				
			}
		}
		else {
			
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ, "未找到对应的规则信息!",PrivateMsg });
			mutexMsg.unlock();
			
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "me") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strGroupID;
		while (isdigit(strLowerMessage[intMsgCnt])) {
			strGroupID += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strAction = strLowerMessage.substr(intMsgCnt);

		for (auto i : strGroupID) {
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strGroupIDInvalid,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
		}
		if (strGroupID.empty()) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,"群号不能为空!",PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (strAction.empty()) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,"动作不能为空!",PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		long long llGroupID = Convert<long long>(strGroupID);
		if (DisabledGroup.count(llGroupID)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strDisabledErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (DisabledMEGroup.count(llGroupID)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strMEDisabledErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		pair<long long, long long>pairQQGroup(eve.fromQQ, llGroupID);
		string strReply = (GroupName.count(pairQQGroup) ? GroupName[pairQQGroup] : getGroupMemberInfo(llGroupID, eve.fromQQ).名片.empty() ? getStrangerInfo(eve.fromQQ).nick : getGroupMemberInfo(llGroupID, eve.fromQQ).名片) + strAction;
		int intSendRes = sendGroupMsg(llGroupID, strReply);
		if (intSendRes < 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSendErr,PrivateMsg });
			mutexMsg.unlock();
		}
		else {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,"命令执行成功!",PrivateMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "set") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strDefaultDice = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strDefaultDice[0] == '0')strDefaultDice.erase(strDefaultDice.begin());
		if (strDefaultDice.empty())strDefaultDice = "100";
		for (auto charNumElement:strDefaultDice)
			if (!isdigit(charNumElement)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strSetInvalid,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
		if (strDefaultDice.length() > 5) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strSetTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		int intDefaultDice = Convert<int>(strDefaultDice);
		if (intDefaultDice == 100)DefaultDice.erase(eve.fromQQ);
		else DefaultDice[eve.fromQQ] = intDefaultDice;
		string strSetSuccessReply = "已将" + strNickName + "的默认骰类型更改为D" + strDefaultDice;
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strSetSuccessReply,PrivateMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "coc6d") {
		string strReply = strNickName;
		COC6D(strReply);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "coc6") {
		intMsgCnt += 4;
		if (strLowerMessage[intMsgCnt] == 's')intMsgCnt++;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterInvalid,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterCannotBeZero,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		COC6(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "dnd") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterInvalid,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterCannotBeZero,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		DND(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "coc7d"|| strLowerMessage.substr(intMsgCnt, 4) == "cocd") {
		string strReply = strNickName;
		COC7D(strReply);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "coc") {
		intMsgCnt += 3;
		if (strLowerMessage[intMsgCnt] == '7')intMsgCnt++;
		if (strLowerMessage[intMsgCnt] == 's')intMsgCnt++;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterInvalid,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterTooBig,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strCharacterCannotBeZero,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		COC7(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromQQ,strReply,PrivateMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage[intMsgCnt] == 'r' || strLowerMessage[intMsgCnt] == 'o' || strLowerMessage[intMsgCnt]=='d') {
		intMsgCnt += 1;
		bool boolDetail = true;
		if (eve.message[intMsgCnt] == 's'){
			boolDetail = false;
			intMsgCnt++;
		}
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string strMainDice;
		string strReason;
		bool tmpContainD = false;
		int intTmpMsgCnt;
		for (intTmpMsgCnt = intMsgCnt; intTmpMsgCnt != eve.message.length() && eve.message[intTmpMsgCnt] != ' '; intTmpMsgCnt++) {
			if (strLowerMessage[intTmpMsgCnt] == 'd' || strLowerMessage[intTmpMsgCnt] == 'p' || strLowerMessage[intTmpMsgCnt] == 'b' || strLowerMessage[intTmpMsgCnt] == '#' || strLowerMessage[intTmpMsgCnt] == 'f' || strLowerMessage[intTmpMsgCnt] == 'a')tmpContainD = true;
			if (!isdigit(strLowerMessage[intTmpMsgCnt]) && strLowerMessage[intTmpMsgCnt] != 'd' &&strLowerMessage[intTmpMsgCnt] != 'k'&&strLowerMessage[intTmpMsgCnt] != 'p'&&strLowerMessage[intTmpMsgCnt] != 'b'&&strLowerMessage[intTmpMsgCnt] != 'f'&& strLowerMessage[intTmpMsgCnt] != '+'&&strLowerMessage[intTmpMsgCnt] != '-' && strLowerMessage[intTmpMsgCnt] != '#' && strLowerMessage[intTmpMsgCnt] != 'a') {
				break;
			}
		}
		if (tmpContainD) {
			strMainDice = strLowerMessage.substr(intMsgCnt, intTmpMsgCnt - intMsgCnt);
			while (eve.message[intTmpMsgCnt] == ' ')intTmpMsgCnt++;
			strReason = eve.message.substr(intTmpMsgCnt);
		}
		else strReason = eve.message.substr(intMsgCnt);

		int intTurnCnt = 1;
		if (strMainDice.find("#") != string::npos) {
			string strTurnCnt = strMainDice.substr(0, strMainDice.find("#"));
			if (strTurnCnt.empty())strTurnCnt = "1";
			strMainDice = strMainDice.substr(strMainDice.find("#") + 1);
			RD rdTurnCnt(strTurnCnt, eve.fromQQ);
			int intRdTurnCntRes = rdTurnCnt.Roll();
			if (intRdTurnCntRes == Value_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strValueErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == Input_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strInputErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroDice_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroDiceErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroType_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroTypeErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == DiceTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strDiceTooBigErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == TypeTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strTypeTooBigErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == AddDiceVal_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAddDiceValErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes != 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strUnknownErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			if (rdTurnCnt.intTotal > 10) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strRollTimeExceeded,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			else if (rdTurnCnt.intTotal <= 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strRollTimeErr,PrivateMsg });
				mutexMsg.unlock();
				return;
			}
			intTurnCnt = rdTurnCnt.intTotal;
			if (strTurnCnt.find("d") != string::npos) {
				string strTurnNotice = strNickName + "的掷骰轮数: " + rdTurnCnt.FormShortString() + "轮";

				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strTurnNotice,PrivateMsg });
				mutexMsg.unlock();

			}
		}
		RD rdMainDice(strMainDice, eve.fromQQ);

		int intFirstTimeRes = rdMainDice.Roll();
		if (intFirstTimeRes == Value_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strValueErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == Input_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strInputErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroDice_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroDiceErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroType_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strZeroTypeErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == DiceTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strDiceTooBigErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == TypeTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strTypeTooBigErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == AddDiceVal_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strAddDiceValErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strUnknownErr,PrivateMsg });
			mutexMsg.unlock();
			return;
		}
		if (!boolDetail&&intTurnCnt != 1) {
			string strAns = strNickName + "骰出了: " + to_string(intTurnCnt) + "次" + rdMainDice.strDice + ": { ";
			if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
			vector<int> vintExVal;
			while (intTurnCnt--) {
				rdMainDice.Roll();
				strAns += to_string(rdMainDice.intTotal);
				if (intTurnCnt != 0)strAns += ",";
				if ((rdMainDice.strDice == "D100" || rdMainDice.strDice == "1D100") && (rdMainDice.intTotal <= 5 || rdMainDice.intTotal >= 96))vintExVal.push_back(rdMainDice.intTotal);
			}
			strAns += " }";
			if (!vintExVal.empty()) {
				strAns += ",极值: ";
				for (auto it = vintExVal.cbegin(); it != vintExVal.cend(); it++) {
					strAns += to_string(*it);
					if (it != vintExVal.cend() - 1)strAns += ",";
				}
			}
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
			mutexMsg.unlock();
		}
		else {
			while (intTurnCnt--) {
				rdMainDice.Roll();
				string strAns = strNickName + "骰出了: " + (boolDetail ? rdMainDice.FormCompleteString() : rdMainDice.FormShortString());
				if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
				mutexMsg.unlock();

			}
		}
	}
	else {
		if (isalpha(eve.message[intMsgCnt])) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromQQ,"命令输入错误!",PrivateMsg });
			mutexMsg.unlock();
		}
	}
}
EVE_GroupMsg_EX(__eventGroupMsg) {
	init(eve.message);
	if (eve.message[0] != '.')return;
	int intMsgCnt = 1;
	while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
	eve.message_block();
	pair<long long, long long>pairQQGroup(eve.fromQQ, eve.fromGroup);
	string strNickName = GroupName.count(pairQQGroup)? GroupName[pairQQGroup]: (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).名片.empty() ? getStrangerInfo(eve.fromQQ).nick : getGroupMemberInfo(eve.fromGroup, eve.fromQQ).名片);
	string strLowerMessage = eve.message;
	transform(strLowerMessage.begin(), strLowerMessage.end(), strLowerMessage.begin(), tolower);
	if (strLowerMessage.substr(intMsgCnt, 3) == "bot") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string Command;
		while (intMsgCnt != strLowerMessage.length()&&!isdigit(strLowerMessage[intMsgCnt])&&!isspace(strLowerMessage[intMsgCnt])) {
			Command += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string QQNum = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (Command == "on") {
			if (QQNum.empty() || QQNum == to_string(getLoginQQ())) {
				if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
					if (DisabledGroup.count(eve.fromGroup)) {
						DisabledGroup.erase(eve.fromGroup);
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ eve.fromGroup,"成功开启本机器人!",GroupMsg });
						mutexMsg.unlock();
					}
					else {
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ eve.fromGroup,"本机器人已经处于开启状态!",GroupMsg });
						mutexMsg.unlock();
					}
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"访问被拒绝,您没有足够的权限!",GroupMsg });
					mutexMsg.unlock();
				}
			}
		}
		else if (Command == "off") {
			if (QQNum.empty() || QQNum == to_string(getLoginQQ())) {
				if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
					if (!DisabledGroup.count(eve.fromGroup)) {
						DisabledGroup.insert(eve.fromGroup);
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ eve.fromGroup,"成功关闭本机器人!",GroupMsg });
						mutexMsg.unlock();
					}
					else {
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ eve.fromGroup,"本机器人已经处于关闭状态!",GroupMsg });
						mutexMsg.unlock();
					}
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"访问被拒绝,您没有足够的权限!",GroupMsg });
					mutexMsg.unlock();
				}
			}
		}
		else{
			if (QQNum == "" || QQNum == to_string(getLoginQQ())) {
				if (DisabledGroup.count(eve.fromGroup)) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"机器人处于关闭状态!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"机器人处于开启状态!",GroupMsg });
					mutexMsg.unlock();
				}
			}
		}
		return;
	}
	if (DisabledGroup.count(eve.fromGroup))return;
	if (strLowerMessage.substr(intMsgCnt, 4) == "help") {
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strHlpMsg,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage[intMsgCnt] == 'w') {
		intMsgCnt++;
		bool boolDetail = false;
		if (strLowerMessage[intMsgCnt] == 'w') {
			intMsgCnt++;
			boolDetail = true;
		}
		bool isHidden = false;
		if (strLowerMessage[intMsgCnt] == 'h') {
			isHidden = true;
			intMsgCnt += 1;
		}
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;

		string strMainDice;
		string strReason;
		int intTmpMsgCnt;
		for (intTmpMsgCnt = intMsgCnt; intTmpMsgCnt != eve.message.length() && eve.message[intTmpMsgCnt] != ' '; intTmpMsgCnt++) {
			if (!isdigit(strLowerMessage[intTmpMsgCnt]) && strLowerMessage[intTmpMsgCnt] != 'd' &&strLowerMessage[intTmpMsgCnt] != 'k'&&strLowerMessage[intTmpMsgCnt] != 'p'&&strLowerMessage[intTmpMsgCnt] != 'b'&&strLowerMessage[intTmpMsgCnt] != 'f'&& strLowerMessage[intTmpMsgCnt] != '+'&&strLowerMessage[intTmpMsgCnt] != '-' && strLowerMessage[intTmpMsgCnt] != '#'&& strLowerMessage[intTmpMsgCnt] != 'a') {
				break;
			}
		}
		strMainDice = strLowerMessage.substr(intMsgCnt, intTmpMsgCnt - intMsgCnt);
		while (eve.message[intTmpMsgCnt] == ' ')intTmpMsgCnt++;
		strReason = eve.message.substr(intTmpMsgCnt);


		int intTurnCnt = 1;
		if (strMainDice.find("#") != string::npos) {
			string strTurnCnt = strMainDice.substr(0, strMainDice.find("#"));
			if (strTurnCnt.empty())strTurnCnt = "1";
			strMainDice = strMainDice.substr(strMainDice.find("#") + 1);
			RD rdTurnCnt(strTurnCnt, eve.fromQQ);
			int intRdTurnCntRes = rdTurnCnt.Roll();
			if (intRdTurnCntRes == Value_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strValueErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == Input_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strInputErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroDice_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroDiceErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroType_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroTypeErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == DiceTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strDiceTooBigErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == TypeTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strTypeTooBigErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == AddDiceVal_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strAddDiceValErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes != 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strUnknownErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			if (rdTurnCnt.intTotal > 10) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strRollTimeExceeded ,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (rdTurnCnt.intTotal <= 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strRollTimeErr ,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			intTurnCnt = rdTurnCnt.intTotal;
			if (strTurnCnt.find("d") != string::npos) {
				string strTurnNotice = strNickName + "的掷骰轮数: " + rdTurnCnt.FormShortString() + "轮";
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,strTurnNotice,GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strTurnNotice,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveGroup.equal_range(eve.fromGroup);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strTurnNotice,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}


			}
		}
		string strFirstDice = strMainDice.substr(0, strMainDice.find('+') < strMainDice.find('-') ? strMainDice.find('+') : strMainDice.find('-'));
		bool boolAdda10 = true;
		for (auto i : strFirstDice) {
			if (!isdigit(i)) {
				boolAdda10 = false;
				break;
			}
		}
		if (boolAdda10)strMainDice.insert(strFirstDice.length(), "a10");
		RD rdMainDice(strMainDice, eve.fromQQ);

		int intFirstTimeRes = rdMainDice.Roll();
		if (intFirstTimeRes == Value_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strValueErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == Input_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strInputErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroDice_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroDiceErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroType_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroTypeErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == DiceTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strDiceTooBigErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == TypeTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strTypeTooBigErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == AddDiceVal_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strAddDiceValErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strUnknownErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (!boolDetail && intTurnCnt != 1) {
			string strAns = strNickName + "骰出了: " + to_string(intTurnCnt) + "次" + rdMainDice.strDice + ": { ";
			if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
			vector<int> vintExVal;
			while (intTurnCnt--) {
				rdMainDice.Roll();
				strAns += to_string(rdMainDice.intTotal);
				if (intTurnCnt != 0)strAns += ",";
				if ((rdMainDice.strDice == "D100" || rdMainDice.strDice == "1D100") && (rdMainDice.intTotal <= 5 || rdMainDice.intTotal >= 96))vintExVal.push_back(rdMainDice.intTotal);
			}
			strAns += " }";
			if (!vintExVal.empty()) {
				strAns += ",极值: ";
				for (auto it = vintExVal.cbegin(); it != vintExVal.cend(); it++) {
					strAns += to_string(*it);
					if (it != vintExVal.cend() - 1)strAns += ",";
				}
			}
			if (!isHidden) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
				mutexMsg.unlock();
				auto range = ObserveGroup.equal_range(eve.fromGroup);
				for (auto it = range.first; it != range.second; it++) {
					if (it->second != eve.fromQQ) {
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
						mutexMsg.unlock();
					}
				}
			}

		}
		else {
			while (intTurnCnt--) {
				rdMainDice.Roll();
				string strAns = strNickName + "骰出了: " + (boolDetail ? rdMainDice.FormCompleteString() : rdMainDice.FormShortString());
				if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveGroup.equal_range(eve.fromGroup);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}

			}
		}
		if (isHidden) {
			string strReply = strNickName + "进行了一次暗骰";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
			mutexMsg.unlock();
		}

	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "ob") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string Command = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (Command == "on") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				if (DisabledOBGroup.count(eve.fromGroup)) {
					DisabledOBGroup.erase(eve.fromGroup);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"成功在本群中启用旁观模式!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup, "本群中旁观模式没有被禁用!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup, "你没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
			return;
		}
		else if (Command == "off") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				if (!DisabledOBGroup.count(eve.fromGroup)) {
					DisabledOBGroup.insert(eve.fromGroup);
					ObserveGroup.clear();
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"成功在本群中禁用旁观模式!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup, "本群中旁观模式没有被启用!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup, "你没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
			return;
		}
		if (DisabledOBGroup.count(eve.fromGroup)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup, "在本群中旁观模式已被禁用!",GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (Command == "list") {
			string Msg = "当前的旁观者有:";
			auto Range = ObserveGroup.equal_range(eve.fromGroup);
			for (auto it = Range.first; it != Range.second; ++it) {
				pair<long long, long long> ObGroup;
				ObGroup.first = it->second;
				ObGroup.second = eve.fromGroup;
				Msg += "\n" + (GroupName.count(ObGroup) ? GroupName[ObGroup] : getGroupMemberInfo(eve.fromGroup, it->second).名片.empty() ? getStrangerInfo(it->second).nick : getGroupMemberInfo(eve.fromGroup, it->second).名片) + "(" + to_string(it->second) + ")";
			}
			string strReply = Msg == "当前的旁观者有:" ? "当前暂无旁观者" : Msg;
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
			mutexMsg.unlock();
		}
		else if (Command == "clr") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				ObserveGroup.erase(eve.fromGroup);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,"成功删除所有旁观者!",GroupMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,"你没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
		}
		else if (Command == "exit") {
			auto Range = ObserveGroup.equal_range(eve.fromGroup);
			for (auto it = Range.first; it != Range.second; ++it) {
				if (it->second == eve.fromQQ) {
					ObserveGroup.erase(it);
					string strReply = strNickName + "成功退出旁观模式!";
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
					mutexMsg.unlock();
					eve.message_block();
					return;
				}
			}
			string strReply = strNickName + "没有加入旁观模式!";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
			mutexMsg.unlock();
		}
		else {
			auto Range = ObserveGroup.equal_range(eve.fromGroup);
			for (auto it = Range.first; it != Range.second; ++it) {
				if (it->second == eve.fromQQ) {
					string strReply = strNickName + "已经处于旁观模式!";
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
					mutexMsg.unlock();
					eve.message_block();
					return;
				}
			}
			ObserveGroup.insert(make_pair(eve.fromGroup, eve.fromQQ));
			string strReply = strNickName + "成功加入旁观模式!";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "ti") {
		string strAns = strNickName + "的疯狂发作-临时症状:\n";
		TempInsane(strAns);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "li") {
		string strAns = strNickName + "的疯狂发作-总结症状:\n";
		LongInsane(strAns);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "sc") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string SanCost = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		intMsgCnt += SanCost.length();
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string San = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (SanCost == "" || SanCost.find("/") == string::npos) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strSCInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (San == "") {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strSanInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		RD rdTest1(SanCost.substr(0, SanCost.find("/")));
		RD rdTest2(SanCost.substr(SanCost.find("/") + 1));
		if (rdTest1.Roll() != 0 || rdTest2.Roll() != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strSCInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		for (auto i : San)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strSanInvalid,GroupMsg });
				mutexMsg.unlock();
				return;
			}
		if (San.length() >= 3) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strSanInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		int intSan = Convert<int>(San);
		if (intSan == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strSanInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		string strAns = strNickName + "的Sancheck:\n1D100=";
		RD rdD100("D100");
		rdD100.Roll();
		strAns += to_string(rdD100.intTotal);

		if (rdD100.intTotal <= intSan) {
			strAns += " 成功\n你的San值减少" + SanCost.substr(0, SanCost.find("/"));
			RD rdSan(SanCost.substr(0, SanCost.find("/")));
			rdSan.Roll();
			if (SanCost.substr(0, SanCost.find("/")).find("d") != string::npos)strAns += "=" + to_string(rdSan.intTotal);
			strAns += +"点,当前剩余" + to_string(intSan - rdSan.intTotal) + "点";
		}
		else {
			strAns += " 失败\n你的San值减少" + SanCost.substr(SanCost.find("/") + 1);
			RD rdSan(SanCost.substr(SanCost.find("/") + 1));
			rdSan.Roll();
			if (SanCost.substr(SanCost.find("/") + 1).find("d") != string::npos)strAns += "=" + to_string(rdSan.intTotal);
			strAns += +"点,当前剩余" + to_string(intSan - rdSan.intTotal) + "点";
		}
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "en") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strSkillName;
		while (intMsgCnt != eve.message.length()&&!isdigit(eve.message[intMsgCnt]) && !isspace(eve.message[intMsgCnt])) {
			strSkillName += eve.message[intMsgCnt];
			intMsgCnt++;
		}
		while (isspace(eve.message[intMsgCnt]))intMsgCnt++;
		string strCurrentValue;
		while (isdigit(eve.message[intMsgCnt])) {
			strCurrentValue += eve.message[intMsgCnt];
			intMsgCnt++;
		}
		if (strCurrentValue.empty()) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strEnValInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (strCurrentValue.length() >= 3) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strEnValInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		int intCurrentVal = Convert<int>(strCurrentValue);
		if (intCurrentVal == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strEnValInvalid,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		string strAns = strNickName + "的" + strSkillName + "增强或成长检定:\n1D100=";
		RD rdD100("D100");
		rdD100.Roll();
		strAns += to_string(rdD100.intTotal);

		if (rdD100.intTotal <= intCurrentVal) {
			strAns += " 失败!\n你的" + (strSkillName.empty() ? "属性或技能值" : strSkillName) + "没有变化!";
		}
		else {
			strAns += " 成功!\n你的" + (strSkillName.empty() ? "属性或技能值" : strSkillName) + "增加1D10=";
			RD rdAddVal("D10");
			rdAddVal.Roll();
			strAns += to_string(rdAddVal.intTotal) + "点,当前为" + to_string(intCurrentVal + rdAddVal.intTotal) + "点";
		}
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "jrrp") {
		intMsgCnt += 4;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string Command = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (Command == "on") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				if (DisabledJRRPGroup.count(eve.fromGroup)) {
					DisabledJRRPGroup.erase(eve.fromGroup);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"成功在本群中启用JRRP!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup, "在本群中JRRP没有被禁用!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup, "您没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
			return;
		}
		else if (Command == "off") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				if (!DisabledJRRPGroup.count(eve.fromGroup)) {
					DisabledJRRPGroup.insert(eve.fromGroup);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"成功在本群中禁用JRRP!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup, "在本群中JRRP没有被启用!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup, "您没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
			return;
		}
		if (DisabledJRRPGroup.count(eve.fromGroup)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup, "在本群中JRRP功能已被禁用",GroupMsg });
			mutexMsg.unlock();
			return;
		}
		char cstrDate[100] = {};
		time_t time_tTime = 0;
		time(&time_tTime);
		tm tmTime;
		localtime_s(&tmTime, &time_tTime);
		strftime(cstrDate, 100, "%F", &tmTime);
		if (JRRP.count(eve.fromQQ) && JRRP[eve.fromQQ].Date == cstrDate) {
			string strReply = strNickName + "今天的人品值是:" + to_string(JRRP[eve.fromQQ].RPVal);
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup, strReply,GroupMsg });
			mutexMsg.unlock();
		}
		else {
			normal_distribution<double> NormalDistribution(60, 15);
			mt19937 Generator(static_cast<unsigned int> (GetCycleCount()));
			int JRRPRes;
			do {
				JRRPRes = static_cast<int> (NormalDistribution(Generator));
			} while (JRRPRes <= 0 || JRRPRes > 100);
			JRRP[eve.fromQQ].Date = cstrDate;
			JRRP[eve.fromQQ].RPVal = JRRPRes;
			string strReply(strNickName + "今天的人品值是:" + to_string(JRRP[eve.fromQQ].RPVal));
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup, strReply,GroupMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "nn") {
		intMsgCnt += 2;
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string name = eve.message.substr(intMsgCnt);
		if (!name.empty()) {
			GroupName[pairQQGroup] = name;
			string strReply = "已将" + strNickName + "的名称更改为" + name;
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup, strReply,GroupMsg });
			mutexMsg.unlock();
		}
		else {
			if (GroupName.count(pairQQGroup)) {
				GroupName.erase(pairQQGroup);
				string strReply = "已将" + strNickName + "的名称删除";
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup, strReply,GroupMsg });
				mutexMsg.unlock();
			}
			else {
				string strReply = strNickName + strNameDelErr;
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup, strReply,GroupMsg });
				mutexMsg.unlock();
			}
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "rules") {
		intMsgCnt += 5;
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string strSearch = eve.message.substr(intMsgCnt);
		for (auto &n : strSearch)n = toupper(n);
		for (int i = 0; i != strSearch.length(); ++i)if (strSearch[i] == ' ') {
			strSearch.erase(strSearch.begin() + i);
			i--;
		}
		strSearch = "||" + strSearch + "||";
		int Loc = strRules.find(strSearch);
		if (Loc != string::npos || strSearch == "||战斗||" || strSearch == "||战斗流程||" || strSearch == "||战斗伤害||") {
			if (strSearch == "||战斗||" || strSearch == "||战斗流程||") {
				string IMAGE = "[CQ:image,file=";
				string IMAGENAME = "Combat.jpg";
				IMAGE += msg_encode(IMAGENAME, true) + "]";
				int res = sendGroupMsg(eve.fromGroup, IMAGE);
				if (res < 0) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"未找到对应的规则信息!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else if (strSearch == "||战斗伤害||") {
				string IMAGE = "[CQ:image,file=";
				string IMAGENAME = "Combat1.jpg";
				IMAGE += msg_encode(IMAGENAME, true) + "]";
				int res = sendGroupMsg(eve.fromGroup, IMAGE);
				if (res < 0) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"未找到对应的规则信息!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else {
				int LocStart = strRules.find("]", Loc) + 1;
				string strReply = strRules.substr(LocStart, strRules.find("[", LocStart) - LocStart - 1);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
				mutexMsg.unlock();
			}
		}
		else {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,"未找到对应的规则信息!",GroupMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "me") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strAction = strLowerMessage.substr(intMsgCnt);
		if (strAction == "on") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				if (DisabledMEGroup.count(eve.fromGroup)) {
					DisabledMEGroup.erase(eve.fromGroup);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"成功在本群中启用.me命令!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"在本群中.me命令没有被禁用!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else{
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,"您没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
			return;
		}
		else if (strAction == "off") {
			if (getGroupMemberInfo(eve.fromGroup, eve.fromQQ).permissions >= 2) {
				if (!DisabledMEGroup.count(eve.fromGroup)) {
					DisabledMEGroup.insert(eve.fromGroup);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"成功在本群中禁用.me命令!",GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,"在本群中.me命令没有被启用!",GroupMsg });
					mutexMsg.unlock();
				}
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,"您没有权限执行此命令!",GroupMsg });
				mutexMsg.unlock();
			}
			return;
		}
		if (DisabledMEGroup.count(eve.fromGroup)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,"在本群中.me命令已被禁用!",GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (strAction.empty()) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,"动作不能为空!",GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (DisabledMEGroup.count(eve.fromGroup)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strMEDisabledErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName + strAction;
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "set") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strDefaultDice = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strDefaultDice[0] == '0')strDefaultDice.erase(strDefaultDice.begin());
			if (strDefaultDice.empty())strDefaultDice = "100";
		for (auto charNumElement : strDefaultDice)
			if (!isdigit(charNumElement)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strSetInvalid,GroupMsg });
				mutexMsg.unlock();
				return;
			}
		if (strDefaultDice.length() > 5) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strSetTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		int intDefaultDice = Convert<int>(strDefaultDice);
		if (intDefaultDice == 100)DefaultDice.erase(eve.fromQQ);
		else DefaultDice[eve.fromQQ] = intDefaultDice;
		string strSetSuccessReply = "已将" + strNickName + "的默认骰类型更改为D" + strDefaultDice;
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strSetSuccessReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "coc6d") {
		string strReply = strNickName;
		COC6D(strReply);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "coc6") {
		intMsgCnt += 4;
		if (strLowerMessage[intMsgCnt] == 's')intMsgCnt++;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterInvalid,GroupMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterCannotBeZero,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		COC6(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "dnd") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterInvalid,GroupMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterCannotBeZero,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		DND(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "coc7d" || strLowerMessage.substr(intMsgCnt, 4) == "cocd") {
		string strReply = strNickName;
		COC7D(strReply);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "coc") {
		intMsgCnt += 3;
		if (strLowerMessage[intMsgCnt] == '7')intMsgCnt++;
		if (strLowerMessage[intMsgCnt] == 's')intMsgCnt++;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterInvalid,GroupMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterTooBig,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strCharacterCannotBeZero,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		COC7(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage[intMsgCnt] == 'r' || strLowerMessage[intMsgCnt] == 'o'|| strLowerMessage[intMsgCnt] == 'h' || strLowerMessage[intMsgCnt] == 'd') {
		bool isHidden = false;
		if (strLowerMessage[intMsgCnt] == 'h')isHidden = true;
		intMsgCnt += 1;
		bool boolDetail = true;
		if (eve.message[intMsgCnt] == 's') {
			boolDetail = false;
			intMsgCnt++;
		}
		if (strLowerMessage[intMsgCnt] == 'h') {
			isHidden = true;
			intMsgCnt += 1;
		}
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string strMainDice;
		string strReason;
		bool tmpContainD = false;
		int intTmpMsgCnt;
		for (intTmpMsgCnt = intMsgCnt; intTmpMsgCnt != eve.message.length() && eve.message[intTmpMsgCnt] != ' '; intTmpMsgCnt++) {
			if (strLowerMessage[intTmpMsgCnt] == 'd' || strLowerMessage[intTmpMsgCnt] == 'p' || strLowerMessage[intTmpMsgCnt] == 'b' || strLowerMessage[intTmpMsgCnt] == '#' || strLowerMessage[intTmpMsgCnt] == 'f' || strLowerMessage[intTmpMsgCnt] == 'a')tmpContainD = true;
			if (!isdigit(strLowerMessage[intTmpMsgCnt]) && strLowerMessage[intTmpMsgCnt] != 'd' &&strLowerMessage[intTmpMsgCnt] != 'k'&&strLowerMessage[intTmpMsgCnt] != 'p'&&strLowerMessage[intTmpMsgCnt] != 'b'&&strLowerMessage[intTmpMsgCnt] != 'f'&& strLowerMessage[intTmpMsgCnt] != '+'&&strLowerMessage[intTmpMsgCnt] != '-' && strLowerMessage[intTmpMsgCnt] != '#' && strLowerMessage[intTmpMsgCnt] != 'a') {
				break;
			}
		}
		if (tmpContainD) {
			strMainDice = strLowerMessage.substr(intMsgCnt, intTmpMsgCnt - intMsgCnt);
			while (eve.message[intTmpMsgCnt] == ' ')intTmpMsgCnt++;
			strReason = eve.message.substr(intTmpMsgCnt);
		}
		else strReason = eve.message.substr(intMsgCnt);

		int intTurnCnt = 1;
		if (strMainDice.find("#") != string::npos) {
			string strTurnCnt = strMainDice.substr(0, strMainDice.find("#"));
			if (strTurnCnt.empty())strTurnCnt = "1";
			strMainDice = strMainDice.substr(strMainDice.find("#") + 1);
			RD rdTurnCnt(strTurnCnt, eve.fromQQ);
			int intRdTurnCntRes = rdTurnCnt.Roll();
			if (intRdTurnCntRes == Value_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strValueErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == Input_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strInputErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroDice_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroDiceErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroType_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroTypeErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == DiceTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strDiceTooBigErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == TypeTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strTypeTooBigErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == AddDiceVal_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strAddDiceValErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes != 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strUnknownErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			if (rdTurnCnt.intTotal > 10) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strRollTimeExceeded,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			else if (rdTurnCnt.intTotal <= 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strRollTimeErr,GroupMsg });
				mutexMsg.unlock();
				return;
			}
			intTurnCnt = rdTurnCnt.intTotal;
			if (strTurnCnt.find("d") != string::npos) {
				string strTurnNotice = strNickName + "的掷骰轮数: " + rdTurnCnt.FormShortString() + "轮";
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,strTurnNotice,GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strTurnNotice,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveGroup.equal_range(eve.fromGroup);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strTurnNotice,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}


			}
		}
		RD rdMainDice(strMainDice, eve.fromQQ);

		int intFirstTimeRes = rdMainDice.Roll();
		if (intFirstTimeRes == Value_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strValueErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == Input_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strInputErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroDice_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroDiceErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroType_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strZeroTypeErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == DiceTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strDiceTooBigErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == TypeTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strTypeTooBigErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == AddDiceVal_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strAddDiceValErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strUnknownErr,GroupMsg });
			mutexMsg.unlock();
			return;
		}
		if (!boolDetail&&intTurnCnt != 1) {
			string strAns = strNickName + "骰出了: " + to_string(intTurnCnt) + "次" + rdMainDice.strDice + ": { ";
			if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
			vector<int> vintExVal;
			while (intTurnCnt--) {
				rdMainDice.Roll();
				strAns += to_string(rdMainDice.intTotal);
				if (intTurnCnt != 0)strAns += ",";
				if ((rdMainDice.strDice == "D100" || rdMainDice.strDice == "1D100") && (rdMainDice.intTotal <= 5 || rdMainDice.intTotal >= 96))vintExVal.push_back(rdMainDice.intTotal);
			}
			strAns += " }";
			if (!vintExVal.empty()) {
				strAns += ",极值: ";
				for (auto it = vintExVal.cbegin(); it != vintExVal.cend(); it++) {
					strAns += to_string(*it);
					if (it != vintExVal.cend() - 1)strAns += ",";
				}
			}
			if (!isHidden) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
				mutexMsg.unlock();
				auto range = ObserveGroup.equal_range(eve.fromGroup);
				for (auto it = range.first; it != range.second; it++) {
					if (it->second != eve.fromQQ) {
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
						mutexMsg.unlock();
					}
				}
			}

		}
		else {
			while (intTurnCnt--) {
				rdMainDice.Roll();
				string strAns = strNickName + "骰出了: " + (boolDetail ? rdMainDice.FormCompleteString() : rdMainDice.FormShortString());
				if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromGroup,strAns,GroupMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveGroup.equal_range(eve.fromGroup);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}

			}
		}
		if (isHidden) {
			string strReply = strNickName + "进行了一次暗骰";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,strReply,GroupMsg });
			mutexMsg.unlock();
		}
	}
	else {
		if (isalpha(eve.message[intMsgCnt])) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromGroup,"命令输入错误!",GroupMsg });
			mutexMsg.unlock();
		}
	}
}
EVE_DiscussMsg_EX(__eventDiscussMsg) {
	init(eve.message);
	if (eve.message[0] != '.')return;
	int intMsgCnt = 1;
	while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
	eve.message_block();
	pair<long long, long long>pairQQDiscuss(eve.fromQQ, eve.fromDiscuss);
	string strNickName = DiscussName.count(pairQQDiscuss) ? DiscussName[pairQQDiscuss] : getStrangerInfo(eve.fromQQ).nick;
	string strLowerMessage = eve.message;
	transform(strLowerMessage.begin(), strLowerMessage.end(), strLowerMessage.begin(), tolower);
	if (strLowerMessage.substr(intMsgCnt, 3) == "bot") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string Command;
		while (intMsgCnt!= strLowerMessage.length()&&!isdigit(strLowerMessage[intMsgCnt]) && !isspace(strLowerMessage[intMsgCnt])) {
			Command += strLowerMessage[intMsgCnt];
			intMsgCnt++;
		}
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string QQNum = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (Command == "on") {
			if (QQNum.empty() || QQNum == to_string(getLoginQQ())) {
				if (DisabledDiscuss.count(eve.fromDiscuss)) {
					DisabledDiscuss.erase(eve.fromDiscuss);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,"成功开启本机器人!",DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,"本机器人已经处于开启状态!",DiscussMsg });
					mutexMsg.unlock();
				}
			}
		}
		else if (Command == "off") {
			if (QQNum.empty() || QQNum == to_string(getLoginQQ())) {
				if (!DisabledDiscuss.count(eve.fromDiscuss)) {
					DisabledDiscuss.insert(eve.fromDiscuss);
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,"成功关闭本机器人!",DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,"本机器人已经处于关闭状态!",DiscussMsg });
					mutexMsg.unlock();
				}
			}
		}
		else {
			if (QQNum == "" || QQNum == to_string(getLoginQQ())) {
				if (DisabledDiscuss.count(eve.fromDiscuss)) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss, "机器人处于关闭状态!",DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss, "机器人处于开启状态!",DiscussMsg });
					mutexMsg.unlock();
				} 
			}
		}
		return;
	}
	if (DisabledDiscuss.count(eve.fromDiscuss))return;
	if (strLowerMessage.substr(intMsgCnt, 4) == "help") {
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss, strHlpMsg,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage[intMsgCnt] == 'w') {
		intMsgCnt++;
		bool boolDetail = false;
		if (strLowerMessage[intMsgCnt] == 'w') {
			intMsgCnt++;
			boolDetail = true;
		}
		bool isHidden = false;
		if (strLowerMessage[intMsgCnt] == 'h') {
			isHidden = true;
			intMsgCnt += 1;
		}
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;

		string strMainDice;
		string strReason;
		int intTmpMsgCnt;
		for (intTmpMsgCnt = intMsgCnt; intTmpMsgCnt != eve.message.length() && eve.message[intTmpMsgCnt] != ' '; intTmpMsgCnt++) {
			if (!isdigit(strLowerMessage[intTmpMsgCnt]) && strLowerMessage[intTmpMsgCnt] != 'd' &&strLowerMessage[intTmpMsgCnt] != 'k'&&strLowerMessage[intTmpMsgCnt] != 'p'&&strLowerMessage[intTmpMsgCnt] != 'b'&&strLowerMessage[intTmpMsgCnt] != 'f'&& strLowerMessage[intTmpMsgCnt] != '+'&&strLowerMessage[intTmpMsgCnt] != '-' && strLowerMessage[intTmpMsgCnt] != '#'&& strLowerMessage[intTmpMsgCnt] != 'a') {
				break;
			}
		}
		strMainDice = strLowerMessage.substr(intMsgCnt, intTmpMsgCnt - intMsgCnt);
		while (eve.message[intTmpMsgCnt] == ' ')intTmpMsgCnt++;
		strReason = eve.message.substr(intTmpMsgCnt);


		int intTurnCnt = 1;
		if (strMainDice.find("#") != string::npos) {
			string strTurnCnt = strMainDice.substr(0, strMainDice.find("#"));
			if (strTurnCnt.empty())strTurnCnt = "1";
			strMainDice = strMainDice.substr(strMainDice.find("#") + 1);
			RD rdTurnCnt(strTurnCnt, eve.fromQQ);
			int intRdTurnCntRes = rdTurnCnt.Roll();
			if (intRdTurnCntRes == Value_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strValueErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == Input_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strInputErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroDice_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroDiceErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroType_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroTypeErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == DiceTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strDiceTooBigErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == TypeTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strTypeTooBigErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == AddDiceVal_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAddDiceValErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes != 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strUnknownErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			if (rdTurnCnt.intTotal > 10) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strRollTimeExceeded ,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (rdTurnCnt.intTotal <= 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strRollTimeErr ,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			intTurnCnt = rdTurnCnt.intTotal;
			if (strTurnCnt.find("d") != string::npos) {
				string strTurnNotice = strNickName + "的掷骰轮数: " + rdTurnCnt.FormShortString() + "轮";
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,strTurnNotice,DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strTurnNotice,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveDiscuss.equal_range(eve.fromDiscuss);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strTurnNotice,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}


			}
		}
		string strFirstDice = strMainDice.substr(0, strMainDice.find('+') < strMainDice.find('-') ? strMainDice.find('+') : strMainDice.find('-'));
		bool boolAdda10 = true;
		for (auto i : strFirstDice) {
			if (!isdigit(i)) {
				boolAdda10 = false;
				break;
			}
		}
		if (boolAdda10)strMainDice.insert(strFirstDice.length(), "a10");
		RD rdMainDice(strMainDice, eve.fromQQ);

		int intFirstTimeRes = rdMainDice.Roll();
		if (intFirstTimeRes == Value_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strValueErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == Input_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strInputErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroDice_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroDiceErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroType_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroTypeErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == DiceTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strDiceTooBigErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == TypeTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strTypeTooBigErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == AddDiceVal_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAddDiceValErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strUnknownErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (!boolDetail && intTurnCnt != 1) {
			string strAns = strNickName + "骰出了: " + to_string(intTurnCnt) + "次" + rdMainDice.strDice + ": { ";
			if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
			vector<int> vintExVal;
			while (intTurnCnt--) {
				rdMainDice.Roll();
				strAns += to_string(rdMainDice.intTotal);
				if (intTurnCnt != 0)strAns += ",";
				if ((rdMainDice.strDice == "D100" || rdMainDice.strDice == "1D100") && (rdMainDice.intTotal <= 5 || rdMainDice.intTotal >= 96))vintExVal.push_back(rdMainDice.intTotal);
			}
			strAns += " }";
			if (!vintExVal.empty()) {
				strAns += ",极值: ";
				for (auto it = vintExVal.cbegin(); it != vintExVal.cend(); it++) {
					strAns += to_string(*it);
					if (it != vintExVal.cend() - 1)strAns += ",";
				}
			}
			if (!isHidden) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAns,DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
				mutexMsg.unlock();
				auto range = ObserveDiscuss.equal_range(eve.fromDiscuss);
				for (auto it = range.first; it != range.second; it++) {
					if (it->second != eve.fromQQ) {
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
						mutexMsg.unlock();
					}
				}
			}

		}
		else {
			while (intTurnCnt--) {
				rdMainDice.Roll();
				string strAns = strNickName + "骰出了: " + (boolDetail ? rdMainDice.FormCompleteString() : rdMainDice.FormShortString());
				if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAns,DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveDiscuss.equal_range(eve.fromDiscuss);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}

			}
		}
		if (isHidden) {
			string strReply = strNickName + "进行了一次暗骰";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
			mutexMsg.unlock();
		}

	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "ob") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string Command = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (Command == "on") {
			if (DisabledOBDiscuss.count(eve.fromDiscuss)) {
				DisabledOBDiscuss.erase(eve.fromDiscuss);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,  "成功在本多人聊天中启用旁观模式!",DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,  "在本多人聊天中旁观模式没有被禁用!",DiscussMsg });
				mutexMsg.unlock();

			}
			return;
		}
		else if (Command == "off") {
			if (!DisabledOBDiscuss.count(eve.fromDiscuss)) {
				DisabledOBDiscuss.insert(eve.fromDiscuss);
				ObserveDiscuss.clear();
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,  "成功在本多人聊天中禁用旁观模式!",DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,  "在本多人聊天中旁观模式没有被启用!",DiscussMsg });
				mutexMsg.unlock();
			}
			return;
		}
		if (DisabledOBDiscuss.count(eve.fromDiscuss)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,  "在本多人聊天中旁观模式已被禁用!",DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (Command == "list") {
			string Msg = "当前的旁观者有:";
			auto Range = ObserveDiscuss.equal_range(eve.fromDiscuss);
			for (auto it = Range.first; it != Range.second; ++it) {
				pair<long long, long long> ObDiscuss;
				ObDiscuss.first = it->second;
				ObDiscuss.second = eve.fromDiscuss;
				Msg += "\n" + (DiscussName.count(ObDiscuss) ? DiscussName[ObDiscuss] : getStrangerInfo(it->second).nick) + "(" + to_string(it->second) + ")";
			}
			string strReply = Msg == "当前的旁观者有:" ? "当前暂无旁观者" : Msg;
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strReply,DiscussMsg });
			mutexMsg.unlock();
		}
		else if (Command == "clr") {
			ObserveDiscuss.erase(eve.fromDiscuss);
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,  "成功删除所有旁观者!",DiscussMsg });
			mutexMsg.unlock();
		}
		else if (Command == "exit") {
			auto Range = ObserveDiscuss.equal_range(eve.fromDiscuss);
			for (auto it = Range.first; it != Range.second; ++it) {
				if (it->second == eve.fromQQ) {
					ObserveDiscuss.erase(it);
					string strReply = strNickName + "成功退出旁观模式!";
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strReply,DiscussMsg });
					mutexMsg.unlock();
					eve.message_block();
					return;
				}
			}
			string strReply = strNickName + "没有加入旁观模式!";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strReply,DiscussMsg });
			mutexMsg.unlock();
		}
		else {
			auto Range = ObserveDiscuss.equal_range(eve.fromDiscuss);
			for (auto it = Range.first; it != Range.second; ++it) {
				if (it->second == eve.fromQQ) {
					string strReply = strNickName + "已经处于旁观模式!";
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strReply,DiscussMsg });
					mutexMsg.unlock();
					eve.message_block();
					return;
				}
			}
			ObserveDiscuss.insert(make_pair(eve.fromDiscuss, eve.fromQQ));
			string strReply = strNickName + "成功加入旁观模式!";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strReply,DiscussMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "ti") {
		string strAns = strNickName + "的疯狂发作-临时症状:\n";
		TempInsane(strAns);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strAns,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "li") {
		string strAns = strNickName + "的疯狂发作-总结症状:\n";
		LongInsane(strAns);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,  strAns,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "sc") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string SanCost = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		intMsgCnt += SanCost.length();
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string San = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (SanCost == "" || SanCost.find("/") == string::npos) {

			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSCInvalid,DiscussMsg });
			mutexMsg.unlock();

			return;
		}
		if (San == "") {

			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSanInvalid,DiscussMsg });
			mutexMsg.unlock();

			return;
		}
		RD rdTest1(SanCost.substr(0, SanCost.find("/")));
		RD rdTest2(SanCost.substr(SanCost.find("/") + 1));
		if (rdTest1.Roll() != 0 || rdTest2.Roll() != 0) {

			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSCInvalid,DiscussMsg });
			mutexMsg.unlock();

			return;
		}
		for (auto i : San)
			if (!isdigit(i)) {

				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSanInvalid,DiscussMsg });
				mutexMsg.unlock();

				return;
			}
		if (San.length() >= 3) {

			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSanInvalid,DiscussMsg });
			mutexMsg.unlock();

			return;
		}
		int intSan = Convert<int>(San);
		if (intSan == 0) {

			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSanInvalid,DiscussMsg });
			mutexMsg.unlock();

			return;
		}
		string strAns = strNickName + "的Sancheck:\n1D100=";
		RD rdD100("D100");
		rdD100.Roll();
		strAns += to_string(rdD100.intTotal);

		if (rdD100.intTotal <= intSan) {
			strAns += " 成功\n你的San值减少" + SanCost.substr(0, SanCost.find("/"));
			RD rdSan(SanCost.substr(0, SanCost.find("/")));
			rdSan.Roll();
			if (SanCost.substr(0, SanCost.find("/")).find("d") != string::npos)strAns += "=" + to_string(rdSan.intTotal);
			strAns += +"点,当前剩余" + to_string(intSan - rdSan.intTotal) + "点";
		}
		else {
			strAns += " 失败\n你的San值减少" + SanCost.substr(SanCost.find("/") + 1);
			RD rdSan(SanCost.substr(SanCost.find("/") + 1));
			rdSan.Roll();
			if (SanCost.substr(SanCost.find("/") + 1).find("d") != string::npos)strAns += "=" + to_string(rdSan.intTotal);
			strAns += +"点,当前剩余" + to_string(intSan - rdSan.intTotal) + "点";
		}
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAns,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "en") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strSkillName;
		while (intMsgCnt != eve.message.length()&&!isdigit(eve.message[intMsgCnt]) && !isspace(eve.message[intMsgCnt])) {
			strSkillName += eve.message[intMsgCnt];
			intMsgCnt++;
		}
		while (isspace(eve.message[intMsgCnt]))intMsgCnt++;
		string strCurrentValue;
		while (isdigit(eve.message[intMsgCnt])) {
			strCurrentValue += eve.message[intMsgCnt];
			intMsgCnt++;
		}
		if (strCurrentValue.empty()) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strEnValInvalid,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (strCurrentValue.length() >= 3) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strEnValInvalid,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		int intCurrentVal = Convert<int>(strCurrentValue);
		if (intCurrentVal == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strEnValInvalid,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		string strAns = strNickName + "的" + strSkillName + "增强或成长检定:\n1D100=";
		RD rdD100("D100");
		rdD100.Roll();
		strAns += to_string(rdD100.intTotal);

		if (rdD100.intTotal <= intCurrentVal) {
			strAns += " 失败!\n你的" + (strSkillName.empty() ? "属性或技能值" : strSkillName) + "没有变化!";
		}
		else {
			strAns += " 成功!\n你的" + (strSkillName.empty() ? "属性或技能值" : strSkillName) + "增加1D10=";
			RD rdAddVal("D10");
			rdAddVal.Roll();
			strAns += to_string(rdAddVal.intTotal) + "点,当前为" + to_string(intCurrentVal + rdAddVal.intTotal) + "点";
		}
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAns,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "jrrp") {
		intMsgCnt += 4;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string Command = strLowerMessage.substr(intMsgCnt, eve.message.find(' ', intMsgCnt) - intMsgCnt);
		if (Command == "on") {
			if (DisabledJRRPDiscuss.count(eve.fromDiscuss)) {
				DisabledJRRPDiscuss.erase(eve.fromDiscuss);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"成功在此多人聊天中启用JRRP!",DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"在此多人聊天中JRRP没有被禁用!",DiscussMsg });
				mutexMsg.unlock();
			}
			return;
		}
		else if (Command == "off") {
			if (!DisabledJRRPDiscuss.count(eve.fromDiscuss)) {
				DisabledJRRPDiscuss.insert(eve.fromDiscuss);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"成功在此多人聊天中禁用JRRP!",DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"在此多人聊天中JRRP没有被启用!",DiscussMsg });
				mutexMsg.unlock();
			}
			return;
		}
		if (DisabledJRRPDiscuss.count(eve.fromDiscuss)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,"在此多人聊天中JRRP已被禁用!",DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		char cstrDate[100] = {};
		time_t time_tTime = 0;
		time(&time_tTime);
		tm tmTime;
		localtime_s(&tmTime, &time_tTime);
		strftime(cstrDate, 100, "%F", &tmTime);
		if (JRRP.count(eve.fromQQ) && JRRP[eve.fromQQ].Date == cstrDate) {
			string strReply = strNickName + "今天的人品值是:" + to_string(JRRP[eve.fromQQ].RPVal);
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
			mutexMsg.unlock();
		}
		else {
			normal_distribution<double> NormalDistribution(60, 15);
			mt19937 Generator(static_cast<unsigned int> (GetCycleCount()));
			int JRRPRes;
			do {
				JRRPRes = static_cast<int> (NormalDistribution(Generator));
			} while (JRRPRes <= 0 || JRRPRes > 100);
			JRRP[eve.fromQQ].Date = cstrDate;
			JRRP[eve.fromQQ].RPVal = JRRPRes;
			string strReply(strNickName + "今天的人品值是:" + to_string(JRRP[eve.fromQQ].RPVal));
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "nn") {
		intMsgCnt += 2;
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string name = eve.message.substr(intMsgCnt);
		if (!name.empty()) {
			DiscussName[pairQQDiscuss] = name;
			string strReply = "已将" + strNickName + "的名称更改为" + name;
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
			mutexMsg.unlock();
		}
		else {
			if (DiscussName.count(pairQQDiscuss)) {
				DiscussName.erase(pairQQDiscuss);
				string strReply = "已将" + strNickName + "的名称删除";
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				string strReply = strNickName + strNameDelErr;
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
				mutexMsg.unlock();
			}
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "rules") {
		intMsgCnt += 5;
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string strSearch = eve.message.substr(intMsgCnt);
		for (auto &n : strSearch)n = toupper(n);
		for (int i = 0; i != strSearch.length(); ++i)if (strSearch[i] == ' ') {
			strSearch.erase(strSearch.begin() + i);
			i--;
		}
		strSearch = "||" + strSearch + "||";
		int Loc = strRules.find(strSearch);
		if (Loc != string::npos || strSearch == "||战斗||" || strSearch == "||战斗流程||" || strSearch == "||战斗伤害||") {
			if (strSearch == "||战斗||" || strSearch == "||战斗流程||") {
				string IMAGE = "[CQ:image,file=";
				string IMAGENAME = "Combat.jpg";
				IMAGE += msg_encode(IMAGENAME, true) + "]";
				int res = sendDiscussMsg(eve.fromDiscuss, IMAGE);
				if (res < 0) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,"未找到对应的规则信息!",DiscussMsg });
					mutexMsg.unlock();
				}
			}
			else if (strSearch == "||战斗伤害||") {
				string IMAGE = "[CQ:image,file=";
				string IMAGENAME = "Combat1.jpg";
				IMAGE += msg_encode(IMAGENAME, true) + "]";
				int res = sendDiscussMsg(eve.fromDiscuss, IMAGE);
				if (res < 0) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,"未找到对应的规则信息!",DiscussMsg });
					mutexMsg.unlock();
				}
			}
			else {
				int LocStart = strRules.find("]", Loc) + 1;
				string strReply = strRules.substr(LocStart, strRules.find("[", LocStart) - LocStart - 1);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
				mutexMsg.unlock();
			}
		}
		else {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,"未找到对应的规则信息!",DiscussMsg });
			mutexMsg.unlock();
		}
	}
	else if (strLowerMessage.substr(intMsgCnt, 2) == "me") {
		intMsgCnt += 2;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strAction = strLowerMessage.substr(intMsgCnt);
		if (strAction == "on") {
			if (DisabledMEDiscuss.count(eve.fromDiscuss)) {
				DisabledMEDiscuss.erase(eve.fromDiscuss);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"成功在本多人聊天中启用.me命令!",DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"在本多人聊天中.me命令没有被禁用!",DiscussMsg });
				mutexMsg.unlock();
			}
			return;
		}
		else if (strAction == "off") {
			if (!DisabledMEDiscuss.count(eve.fromDiscuss)) {
				DisabledMEDiscuss.insert(eve.fromDiscuss);
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"成功在本多人聊天中禁用.me命令!",DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,"在本多人聊天中.me命令没有被启用!",DiscussMsg });
				mutexMsg.unlock();
			}
			return;
		}
		if (DisabledMEDiscuss.count(eve.fromDiscuss)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,"在本多人聊天中.me命令已被禁用!",DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (strAction.empty()) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,"动作不能为空!",DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (DisabledMEDiscuss.count(eve.fromDiscuss)) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strMEDisabledErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName + strAction;
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "set") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strDefaultDice = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strDefaultDice[0] == '0')strDefaultDice.erase(strDefaultDice.begin());
		if (strDefaultDice.empty())strDefaultDice = "100";
		for (auto charNumElement : strDefaultDice)
			if (!isdigit(charNumElement)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSetInvalid,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
		if (strDefaultDice.length() > 5) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSetTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		int intDefaultDice = Convert<int>(strDefaultDice);
		if (intDefaultDice == 100)DefaultDice.erase(eve.fromQQ);
		else DefaultDice[eve.fromQQ] = intDefaultDice;
		string strSetSuccessReply = "已将" + strNickName + "的默认骰类型更改为D" + strDefaultDice;
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strSetSuccessReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "coc6d") {
		string strReply = strNickName;
		COC6D(strReply);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 4) == "coc6") {
		intMsgCnt += 4;
		if (strLowerMessage[intMsgCnt] == 's')intMsgCnt++;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterInvalid,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterCannotBeZero,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		COC6(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "dnd") {
		intMsgCnt += 3;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterInvalid,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterCannotBeZero,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		DND(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 5) == "coc7d" || strLowerMessage.substr(intMsgCnt, 4) == "cocd") {
		string strReply = strNickName;
		COC7D(strReply);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage.substr(intMsgCnt, 3) == "coc") {
		intMsgCnt += 3;
		if (strLowerMessage[intMsgCnt] == '7')intMsgCnt++;
		if (strLowerMessage[intMsgCnt] == 's')intMsgCnt++;
		while (strLowerMessage[intMsgCnt] == ' ')intMsgCnt++;
		string strNum = strLowerMessage.substr(intMsgCnt, strLowerMessage.find(" ", intMsgCnt) - intMsgCnt);
		while (strNum[0] == '0')strNum.erase(strNum.begin());
		for (auto i : strNum)
			if (!isdigit(i)) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterInvalid,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
		if (strNum.length() > 2) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		int intNum = Convert<int>(strNum);
		if (strNum.empty())intNum = 1;
		if (intNum > 10) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterTooBig,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (intNum == 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strCharacterCannotBeZero,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		string strReply = strNickName;
		COC7(strReply, intNum);
		mutexMsg.lock();
		SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
		mutexMsg.unlock();
	}
	else if (strLowerMessage[intMsgCnt] == 'r' || strLowerMessage[intMsgCnt] == 'o' || strLowerMessage[intMsgCnt] == 'h' || strLowerMessage[intMsgCnt] == 'd') {
		bool isHidden = false;
		if (strLowerMessage[intMsgCnt] == 'h')isHidden = true;
		intMsgCnt += 1;
		bool boolDetail = true;
		if (eve.message[intMsgCnt] == 's') {
			boolDetail = false;
			intMsgCnt++;
		}
		if (strLowerMessage[intMsgCnt] == 'h') {
			isHidden = true;
			intMsgCnt += 1;
		}
		while (eve.message[intMsgCnt] == ' ')intMsgCnt++;
		string strMainDice;
		string strReason;
		bool tmpContainD = false;
		int intTmpMsgCnt;
		for (intTmpMsgCnt = intMsgCnt; intTmpMsgCnt != eve.message.length() && eve.message[intTmpMsgCnt] != ' '; intTmpMsgCnt++) {
			if (strLowerMessage[intTmpMsgCnt] == 'd' || strLowerMessage[intTmpMsgCnt] == 'p' || strLowerMessage[intTmpMsgCnt] == 'b' || strLowerMessage[intTmpMsgCnt] == '#' || strLowerMessage[intTmpMsgCnt] == 'f' || strLowerMessage[intTmpMsgCnt] == 'a')tmpContainD = true;
			if (!isdigit(strLowerMessage[intTmpMsgCnt]) && strLowerMessage[intTmpMsgCnt] != 'd' &&strLowerMessage[intTmpMsgCnt] != 'k'&&strLowerMessage[intTmpMsgCnt] != 'p'&&strLowerMessage[intTmpMsgCnt] != 'b'&&strLowerMessage[intTmpMsgCnt] != 'f'&& strLowerMessage[intTmpMsgCnt] != '+'&&strLowerMessage[intTmpMsgCnt] != '-' && strLowerMessage[intTmpMsgCnt] != '#' && strLowerMessage[intTmpMsgCnt] != 'a') {
				break;
			}
		}
		if (tmpContainD) {
			strMainDice = strLowerMessage.substr(intMsgCnt, intTmpMsgCnt - intMsgCnt);
			while (eve.message[intTmpMsgCnt] == ' ')intTmpMsgCnt++;
			strReason = eve.message.substr(intTmpMsgCnt);
		}
		else strReason = eve.message.substr(intMsgCnt);

		int intTurnCnt = 1;
		if (strMainDice.find("#") != string::npos) {
			string strTurnCnt = strMainDice.substr(0, strMainDice.find("#"));
			if (strTurnCnt.empty())strTurnCnt = "1";
			strMainDice = strMainDice.substr(strMainDice.find("#") + 1);
			RD rdTurnCnt(strTurnCnt, eve.fromQQ);
			int intRdTurnCntRes = rdTurnCnt.Roll();
			if (intRdTurnCntRes == Value_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strValueErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == Input_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strInputErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroDice_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroDiceErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == ZeroType_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroTypeErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == DiceTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strDiceTooBigErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == TypeTooBig_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strTypeTooBigErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes == AddDiceVal_Err) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAddDiceValErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (intRdTurnCntRes != 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strUnknownErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			if (rdTurnCnt.intTotal > 10) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strRollTimeExceeded,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			else if (rdTurnCnt.intTotal <= 0) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strRollTimeErr,DiscussMsg });
				mutexMsg.unlock();
				return;
			}
			intTurnCnt = rdTurnCnt.intTotal;
			if (strTurnCnt.find("d") != string::npos) {
				string strTurnNotice = strNickName + "的掷骰轮数: " + rdTurnCnt.FormShortString() + "轮";
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,strTurnNotice,DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strTurnNotice,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveDiscuss.equal_range(eve.fromDiscuss);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strTurnNotice,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}


			}
		}
		RD rdMainDice(strMainDice, eve.fromQQ);

		int intFirstTimeRes = rdMainDice.Roll();
		if (intFirstTimeRes == Value_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strValueErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == Input_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strInputErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroDice_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroDiceErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == ZeroType_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strZeroTypeErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == DiceTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strDiceTooBigErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == TypeTooBig_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strTypeTooBigErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes == AddDiceVal_Err) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAddDiceValErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		else if (intFirstTimeRes != 0) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strUnknownErr,DiscussMsg });
			mutexMsg.unlock();
			return;
		}
		if (!boolDetail&&intTurnCnt != 1) {
			string strAns = strNickName + "骰出了: " + to_string(intTurnCnt) + "次" + rdMainDice.strDice + ": { ";
			if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
			vector<int> vintExVal;
			while (intTurnCnt--) {
				rdMainDice.Roll();
				strAns += to_string(rdMainDice.intTotal);
				if (intTurnCnt != 0)strAns += ",";
				if ((rdMainDice.strDice == "D100" || rdMainDice.strDice == "1D100") && (rdMainDice.intTotal <= 5 || rdMainDice.intTotal >= 96))vintExVal.push_back(rdMainDice.intTotal);
			}
			strAns += " }";
			if (!vintExVal.empty()) {
				strAns += ",极值: ";
				for (auto it = vintExVal.cbegin(); it != vintExVal.cend(); it++) {
					strAns += to_string(*it);
					if (it != vintExVal.cend() - 1)strAns += ",";
				}
			}
			if (!isHidden) {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAns,DiscussMsg });
				mutexMsg.unlock();
			}
			else {
				mutexMsg.lock();
				SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
				mutexMsg.unlock();
				auto range = ObserveDiscuss.equal_range(eve.fromDiscuss);
				for (auto it = range.first; it != range.second; it++) {
					if (it->second != eve.fromQQ) {
						mutexMsg.lock();
						SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
						mutexMsg.unlock();
					}
				}
			}

		}
		else {
			while (intTurnCnt--) {
				rdMainDice.Roll();
				string strAns = strNickName + "骰出了: " + (boolDetail ? rdMainDice.FormCompleteString() : rdMainDice.FormShortString());
				if (!strReason.empty())strAns.insert(0, "由于" + strReason + " ");
				if (!isHidden) {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromDiscuss,strAns,DiscussMsg });
					mutexMsg.unlock();
				}
				else {
					mutexMsg.lock();
					SendMsgQueue.push(MsgType{ eve.fromQQ,strAns,PrivateMsg });
					mutexMsg.unlock();
					auto range = ObserveDiscuss.equal_range(eve.fromDiscuss);
					for (auto it = range.first; it != range.second; it++) {
						if (it->second != eve.fromQQ) {
							mutexMsg.lock();
							SendMsgQueue.push(MsgType{ it->second,strAns,PrivateMsg });
							mutexMsg.unlock();
						}
					}
				}

			}
		}
		if (isHidden) {
			string strReply = strNickName + "进行了一次暗骰";
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,strReply,DiscussMsg });
			mutexMsg.unlock();
		}
	}
	else {
		if (isalpha(eve.message[intMsgCnt])) {
			mutexMsg.lock();
			SendMsgQueue.push(MsgType{ eve.fromDiscuss,"命令输入错误!",DiscussMsg });
			mutexMsg.unlock();
		}
	}
}
EVE_Disable(__eventDisable) {
	Enabled = false;
	ofstream ofstreamGroupName(strFileLoc + "GroupName.RDconf", ios::in | ios::trunc);
	for (auto it = GroupName.begin(); it != GroupName.end(); it++) {
		if (!(it->second).empty())
			ofstreamGroupName << it->first.first << " " << it->first.second << " " << it->second << endl;
	}
	ofstreamGroupName.close();
	ofstream ofstreamDiscussName(strFileLoc + "DiscussName.RDconf", ios::in | ios::trunc);
	for (auto it = DiscussName.begin(); it != DiscussName.end(); it++) {
		if (!(it->second).empty())
			ofstreamDiscussName << it->first.first << " " << it->first.second << " " << it->second << endl;
	}
	ofstreamDiscussName.close();

	ofstream ofstreamDisabledGroup(strFileLoc + "DisabledGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledGroup.begin(); it != DisabledGroup.end(); it++) {
		ofstreamDisabledGroup << *it << endl;
	}
	ofstreamDisabledGroup.close();

	ofstream ofstreamDisabledDiscuss(strFileLoc + "DisabledDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledDiscuss.begin(); it != DisabledDiscuss.end(); it++) {
		ofstreamDisabledDiscuss << *it << endl;
	}
	ofstreamDisabledDiscuss.close();
	ofstream ofstreamDisabledJRRPGroup(strFileLoc + "DisabledJRRPGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledJRRPGroup.begin(); it != DisabledJRRPGroup.end(); it++) {
		ofstreamDisabledJRRPGroup << *it << endl;
	}
	ofstreamDisabledJRRPGroup.close();

	ofstream ofstreamDisabledJRRPDiscuss(strFileLoc + "DisabledJRRPDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledJRRPDiscuss.begin(); it != DisabledJRRPDiscuss.end(); it++) {
		ofstreamDisabledJRRPDiscuss << *it << endl;
	}
	ofstreamDisabledJRRPDiscuss.close();

	ofstream ofstreamDisabledMEGroup(strFileLoc + "DisabledMEGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledMEGroup.begin(); it != DisabledMEGroup.end(); it++) {
		ofstreamDisabledMEGroup << *it << endl;
	}
	ofstreamDisabledMEGroup.close();

	ofstream ofstreamDisabledMEDiscuss(strFileLoc + "DisabledMEDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledMEDiscuss.begin(); it != DisabledMEDiscuss.end(); it++) {
		ofstreamDisabledMEDiscuss << *it << endl;
	}
	ofstreamDisabledMEDiscuss.close();

	ofstream ofstreamDisabledOBGroup(strFileLoc + "DisabledOBGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledOBGroup.begin(); it != DisabledOBGroup.end(); it++) {
		ofstreamDisabledOBGroup << *it << endl;
	}
	ofstreamDisabledOBGroup.close();

	ofstream ofstreamDisabledOBDiscuss(strFileLoc + "DisabledOBDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledOBDiscuss.begin(); it != DisabledOBDiscuss.end(); it++) {
		ofstreamDisabledOBDiscuss << *it << endl;
	}
	ofstreamDisabledOBDiscuss.close();

	ofstream ofstreamObserveGroup(strFileLoc + "ObserveGroup.RDconf", ios::in | ios::trunc);
	for (auto it = ObserveGroup.begin(); it != ObserveGroup.end(); it++) {
		ofstreamObserveGroup << it->first << " " << it->second << endl;
	}
	ofstreamObserveGroup.close();

	ofstream ofstreamObserveDiscuss(strFileLoc + "ObserveDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = ObserveDiscuss.begin(); it != ObserveDiscuss.end(); it++) {
		ofstreamObserveDiscuss << it->first << " " << it->second << endl;
	}
	ofstreamObserveDiscuss.close();
	ofstream ofstreamJRRP(strFileLoc + "JRRP.RDconf", ios::in | ios::trunc);
	for (auto it = JRRP.begin(); it != JRRP.end(); it++) {
		ofstreamJRRP << it->first << " " << it->second.Date << " " << it->second.RPVal << endl;
	}
	ofstreamJRRP.close();

	ofstream ofstreamDefault(strFileLoc + "Default.RDconf", ios::in | ios::trunc);
	for (auto it = DefaultDice.begin(); it != DefaultDice.end(); it++) {
		ofstreamDefault << it->first << " " << it->second << endl;
	}
	ofstreamDefault.close();
	JRRP.clear();
	DefaultDice.clear();
	GroupName.clear();
	DiscussName.clear();
	DisabledGroup.clear();
	DisabledDiscuss.clear();
	DisabledJRRPGroup.clear();
	DisabledJRRPDiscuss.clear();
	DisabledMEGroup.clear();
	DisabledMEDiscuss.clear();
	DisabledOBGroup.clear();
	DisabledOBDiscuss.clear();
	ObserveGroup.clear();
	ObserveDiscuss.clear();
	strFileLoc.clear();
	return 0;
}
EVE_Exit(__eventExit) {
	if (!Enabled)return 0;
	ofstream ofstreamGroupName(strFileLoc + "GroupName.RDconf", ios::in | ios::trunc);
	for (auto it = GroupName.begin(); it != GroupName.end(); it++) {
		if (!(it->second).empty())
			ofstreamGroupName << it->first.first << " " << it->first.second << " " << it->second << endl;
	}
	ofstreamGroupName.close();
	ofstream ofstreamDiscussName(strFileLoc + "DiscussName.RDconf", ios::in | ios::trunc);
	for (auto it = DiscussName.begin(); it != DiscussName.end(); it++) {
		if (!(it->second).empty())
			ofstreamDiscussName << it->first.first << " " << it->first.second << " " << it->second << endl;
	}
	ofstreamDiscussName.close();

	ofstream ofstreamDisabledGroup(strFileLoc + "DisabledGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledGroup.begin(); it != DisabledGroup.end(); it++) {
		ofstreamDisabledGroup << *it << endl;
	}
	ofstreamDisabledGroup.close();

	ofstream ofstreamDisabledDiscuss(strFileLoc + "DisabledDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledDiscuss.begin(); it != DisabledDiscuss.end(); it++) {
		ofstreamDisabledDiscuss << *it << endl;
	}
	ofstreamDisabledDiscuss.close();
	ofstream ofstreamDisabledJRRPGroup(strFileLoc + "DisabledJRRPGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledJRRPGroup.begin(); it != DisabledJRRPGroup.end(); it++) {
		ofstreamDisabledJRRPGroup << *it << endl;
	}
	ofstreamDisabledJRRPGroup.close();

	ofstream ofstreamDisabledJRRPDiscuss(strFileLoc + "DisabledJRRPDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledJRRPDiscuss.begin(); it != DisabledJRRPDiscuss.end(); it++) {
		ofstreamDisabledJRRPDiscuss << *it << endl;
	}
	ofstreamDisabledJRRPDiscuss.close();

	ofstream ofstreamDisabledMEGroup(strFileLoc + "DisabledMEGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledMEGroup.begin(); it != DisabledMEGroup.end(); it++) {
		ofstreamDisabledMEGroup << *it << endl;
	}
	ofstreamDisabledMEGroup.close();

	ofstream ofstreamDisabledMEDiscuss(strFileLoc + "DisabledMEDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledMEDiscuss.begin(); it != DisabledMEDiscuss.end(); it++) {
		ofstreamDisabledMEDiscuss << *it << endl;
	}
	ofstreamDisabledMEDiscuss.close();

	ofstream ofstreamDisabledOBGroup(strFileLoc + "DisabledOBGroup.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledOBGroup.begin(); it != DisabledOBGroup.end(); it++) {
		ofstreamDisabledOBGroup << *it << endl;
	}
	ofstreamDisabledOBGroup.close();

	ofstream ofstreamDisabledOBDiscuss(strFileLoc + "DisabledOBDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = DisabledOBDiscuss.begin(); it != DisabledOBDiscuss.end(); it++) {
		ofstreamDisabledOBDiscuss << *it << endl;
	}
	ofstreamDisabledOBDiscuss.close();

	ofstream ofstreamObserveGroup(strFileLoc + "ObserveGroup.RDconf", ios::in | ios::trunc);
	for (auto it = ObserveGroup.begin(); it != ObserveGroup.end(); it++) {
		ofstreamObserveGroup << it->first << " " << it->second << endl;
	}
	ofstreamObserveGroup.close();

	ofstream ofstreamObserveDiscuss(strFileLoc + "ObserveDiscuss.RDconf", ios::in | ios::trunc);
	for (auto it = ObserveDiscuss.begin(); it != ObserveDiscuss.end(); it++) {
		ofstreamObserveDiscuss << it->first << " " << it->second << endl;
	}
	ofstreamObserveDiscuss.close();
	ofstream ofstreamJRRP(strFileLoc + "JRRP.RDconf", ios::in | ios::trunc);
	for (auto it = JRRP.begin(); it != JRRP.end(); it++) {
		ofstreamJRRP << it->first << " " << it->second.Date << " " << it->second.RPVal << endl;
	}
	ofstreamJRRP.close();

	ofstream ofstreamDefault(strFileLoc + "Default.RDconf", ios::in | ios::trunc);
	for (auto it = DefaultDice.begin(); it != DefaultDice.end(); it++) {
		ofstreamDefault << it->first << " " << it->second << endl;
	}
	ofstreamDefault.close();
	return 0;
}
MUST_AppInfo_RETURN(CQAPPID);