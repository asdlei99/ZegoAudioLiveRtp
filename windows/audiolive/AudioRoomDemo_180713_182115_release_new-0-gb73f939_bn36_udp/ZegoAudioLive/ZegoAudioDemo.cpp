#include "ZegoAudioDemo.h"

QZegoAudioDemoApp::QZegoAudioDemoApp()
{
	
}

//ȫ��Ψһ��base����
QZegoAudioDemoApp theApp;

QZegoBase& QZegoAudioDemoApp::GetBase()
{
	return m_base;
}

QZegoUserConfig& QZegoAudioDemoApp::GetConfig()
{
	return m_config;
}

QZegoSDKSignal * GetAVSignal()
{
	return theApp.GetBase().GetAVSignal();
}