#include <QSharedPointer>
#include "ZegoBase.h"
#include "Base/IncludeZegoAudioRoomApi.h"
/*
	若本语句出现编译错误，请打开本文件目录下的AppIDHelper.h.in文件，将AppID和AppSign修改为自己从Zego官网申请好的AppID和AppSign，并将AppIDHelper.h.in修改为AppIDHelper.h，重新编译
*/
#include "AppIDHelper.h"

static unsigned int g_dwAppID_Udp = 0;
static unsigned char g_bufSignKey_Udp[] =
{
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

/* 以下两组AppID和AppSign，开发者无需关注 */
static unsigned int g_dwAppID_International = 0;
static unsigned char g_bufSignKey_International[] =
{
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

static unsigned long  g_dwAppID_Empty = 0;
static unsigned char g_bufSignKey_Empty[] =
{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

QZegoBase::QZegoBase(void)
{
	/*
		若本语句出现编译错误，请打开本文件目录下的AppIDHelper.h.in文件，将AppID和AppSign修改为自己从Zego官网申请好的AppID和AppSign，并将AppIDHelper.h.in修改为AppIDHelper.h，重新编译
	*/
	g_dwAppID_Udp = GetGlobalAppId();
	GetGlobalAppSign(g_bufSignKey_Udp);
	
	appIDs.push_back(g_dwAppID_Udp);
	appIDs.push_back(g_dwAppID_International);
	appIDs.push_back(g_dwAppID_Empty);

	appSigns.push_back(g_bufSignKey_Udp);
	appSigns.push_back(g_bufSignKey_International);
	appSigns.push_back(g_bufSignKey_Empty);

	m_pAVSignal = new QZegoSDKSignal;

}

QZegoBase::~QZegoBase(void)
{
	UninitAVSDK();

	delete m_pAVSignal;

}

bool QZegoBase::InitAVSDK(SettingsPtr pCurSetting, QString userID, QString userName)
{
	if (!IsAVSdkInited())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: SDK Init").arg(__FUNCTION__)));

		//Qstring对象.toLocal8Bit().data()用于将QString转为const char*
		AUDIOROOM::SetLogDir(nullptr);
		AUDIOROOM::SetVerbose(true);
		//AUDIOROOM::SetBusinessType(0);
		AUDIOROOM::SetUser(qtoc(userID), qtoc(userName));
		AUDIOROOM::SetUseTestEnv(isTestEnv);
		// ToDo: 需要通过代码获取网络类型
		//AUDIOROOM::SetNetType(2);
	
		AUDIOROOM::SetAudioRoomCallback(m_pAVSignal);
		AUDIOROOM::SetAudioLivePublisherCallback(m_pAVSignal);
		AUDIOROOM::SetAudioLivePlayerCallback(m_pAVSignal);
		AUDIOROOM::SetAudioDeviceStateCallback(m_pAVSignal);
		AUDIOROOM::SetAudioEngineCallback(m_pAVSignal);
		AUDIOROOM::SetAudioLiveEventCallback(m_pAVSignal);
		AUDIOROOM::SetAudioRecordCallback(m_pAVSignal);
		SOUNDLEVEL::SetSoundLevelCallback(m_pAVSignal);

		//默认UDP
		if (!AUDIOROOM::InitSDK(appIDs[key], appSigns[key], 32))
		{
			m_isInitedSDK = false;
			return false;
		}
	}

	//为了调用OnUserUpdate
	AUDIOROOM::SetUserStateUpdate(true);
	AUDIOROOM::SetLatencyMode(AV::ZEGO_LATENCY_MODE_LOW3);
	AUDIOROOM::EnableDTX(true);

	//AUDIOROOM::EnableAudioTrafficControl(isAudioBitrateControll);
	
	m_isInitedSDK = true;
	return true;
}

bool QZegoBase::InitAVSDKofCustom(SettingsPtr pCurSetting, QString userID, QString userName, unsigned long appID, unsigned char *appSign, int signLen)
{
	if (!IsAVSdkInited())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: SDK Init Custom.").arg(__FUNCTION__)));

		//Qstring对象.toLocal8Bit().data()用于将QString转为const char*
		AUDIOROOM::SetLogDir(nullptr);
		AUDIOROOM::SetVerbose(true);
		//AUDIOROOM::SetBusinessType(0);
		AUDIOROOM::SetUser(qtoc(userID), qtoc(userName));
		AUDIOROOM::SetUseTestEnv(isTestEnv);
		// ToDo: 需要通过代码获取网络类型
		//AUDIOROOM::SetNetType(2);

		AUDIOROOM::SetAudioRoomCallback(m_pAVSignal);
		AUDIOROOM::SetAudioLivePublisherCallback(m_pAVSignal);
		AUDIOROOM::SetAudioLivePlayerCallback(m_pAVSignal);
		AUDIOROOM::SetAudioDeviceStateCallback(m_pAVSignal);
		AUDIOROOM::SetAudioEngineCallback(m_pAVSignal);
		AUDIOROOM::SetAudioLiveEventCallback(m_pAVSignal);
		AUDIOROOM::SetAudioRecordCallback(m_pAVSignal);
		SOUNDLEVEL::SetSoundLevelCallback(m_pAVSignal);

		if (!AUDIOROOM::InitSDK(appID, appSign, signLen))
		{
			m_isInitedSDK = false;
			return false;
		}

		//为了调用OnUserUpdate
		AUDIOROOM::SetUserStateUpdate(true);
		AUDIOROOM::SetLatencyMode(AV::ZEGO_LATENCY_MODE_LOW3);
		AUDIOROOM::EnableDTX(true);

		//AUDIOROOM::EnableAudioTrafficControl(isAudioBitrateControll);

		m_isInitedSDK = true;
		return true;
	}
}

void QZegoBase::UninitAVSDK(void)
{
	if (IsAVSdkInited())
	{
		log_string_notice(qtoc(QStringLiteral("[%1]: SDK Uninit").arg(__FUNCTION__)));

		AUDIOROOM::SetAudioRoomCallback(nullptr);
		AUDIOROOM::SetAudioLivePublisherCallback(nullptr);
		AUDIOROOM::SetAudioLivePlayerCallback(nullptr);
		AUDIOROOM::SetAudioDeviceStateCallback(nullptr);
		AUDIOROOM::SetAudioEngineCallback(nullptr);
		AUDIOROOM::SetAudioLiveEventCallback(nullptr);
		AUDIOROOM::SetAudioRecordCallback(nullptr);
		SOUNDLEVEL::SetSoundLevelCallback(nullptr);

		AUDIOROOM::UnInitSDK();

		m_isInitedSDK = false;
	}
}

bool QZegoBase::IsAVSdkInited(void)
{
	return m_isInitedSDK;
}

QZegoSDKSignal* QZegoBase::GetAVSignal(void)
{
	return m_pAVSignal;
}

unsigned long QZegoBase::GetAppID(void)
{
	return appIDs[key];
}

unsigned long QZegoBase::GetAppIDwithKey(int tmpKey)
{
	return appIDs[tmpKey];
}

void QZegoBase::setKey(int pKey)
{
	key = pKey;

}

int QZegoBase::getKey()
{
	return key;
}

unsigned char* QZegoBase::GetAppSign()
{
	return appSigns[key];
}

void QZegoBase::setIsTestEnv(bool isTest)
{
	isTestEnv = isTest;
}

bool QZegoBase::getUseTestEnv()
{
	return isTestEnv;
}

void QZegoBase::setCustomAppID(unsigned long appid)
{
	appIDs[2] = appid;
}

void QZegoBase::setCustomAppSign(unsigned char *appsign)
{
	for (int i = 0; i < 32; i++)
	{
		appSigns[2][i] = appsign[i];
	}
}

void QZegoBase::setUseAudioBitrateControll(bool bUse)
{
	isAudioBitrateControll = bUse;
}

bool QZegoBase::getUseAudioBitrateControll()
{
	return isAudioBitrateControll;
}
