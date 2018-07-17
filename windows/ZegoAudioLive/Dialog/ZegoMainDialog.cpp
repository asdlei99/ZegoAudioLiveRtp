#include "ZegoMainDialog.h"
#include "Base/IncludeZegoAudioRoomApi.h"
#include "Base/ZegoAudioLiveDefines.h"
#include "Dialog/ZegoSettingsDialog.h"
#include "Dialog/ZegoAudioLiveDialog.h"

#include <QDesktopServices>

ZegoMainDialog::ZegoMainDialog(QWidget *parent)
	: ZegoDialog(parent)
{
	ui.setupUi(this);
	log_string_notice(qtoc(QStringLiteral("[%1]: main dialog create").arg(__FUNCTION__)));

	//�����뷿��Ų��ܽ��뷿��
	ui.m_bEnterRoom->setEnabled(false);
	connect(ui.m_edRoomID, &QLineEdit::textChanged, this, [this] { emit sigCheckEnterRoom(); });

	connect(this, &ZegoMainDialog::sigCheckEnterRoom, this, &ZegoMainDialog::OnCheckEnterRoom);

	ui.m_lbVersion->setText(QStringLiteral("v %1").arg(APP_VERSION));
}

ZegoMainDialog::~ZegoMainDialog()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: main dialog destroy").arg(__FUNCTION__)));
	mBase.UninitAVSDK();
}

void ZegoMainDialog::initDialog()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: main dialog init").arg(__FUNCTION__)));

	initButtonIcon();

	//���û������ļ��ж�ȡ������Ϣ�����Ҳ������½������ļ�
	mConfig.LoadConfig();

	m_strEdUserId = mConfig.GetUserId();
	m_strEdUserName = mConfig.GetUserName();

	updateAppVersionTitle();

	//����AudioSettings��˵������ֻ��userID��userName��mircoPhoneID
	SettingsPtr pCurSettings = mConfig.GetAudioSettings();

	if (pCurSettings != nullptr)
	{
		mBase.InitAVSDK(pCurSettings, m_strEdUserId, m_strEdUserName);
	}
}

void ZegoMainDialog::updateAppVersionTitle()
{
	AppVersion appVersion = mConfig.getAppVersion();
	switch (appVersion.m_versionMode)
	{
	case ZEGO_PROTOCOL_UDP:
		ui.m_title->setText(tr("AudioLive (���ڰ�)"));
		break;
	case ZEGO_PROTOCOL_UDP_INTERNATIONAL:
		ui.m_title->setText(tr("AudioLive (���ʰ�)"));
		break;
	case ZEGO_PROTOCOL_CUSTOM:
		ui.m_title->setText(tr("AudioLive (�Զ���)"));
		break;
	default:
		ui.m_title->setText(tr("AudioLive (δ֪)"));
		break;
	}
}

void ZegoMainDialog::initButtonIcon()
{
	ui.m_bClose->setButtonIcon("close");
	ui.m_bMin->setButtonIcon("min");

	ui.m_bJumpToNet->setButtonIcon("official");
	ui.m_bJumpToNet->setToolTip(tr("��������"));
}

void ZegoMainDialog::OnCheckEnterRoom()
{
	QString roomId = ui.m_edRoomID->text();
	roomId = roomId.simplified();

	if (!roomId.isEmpty())
		ui.m_bEnterRoom->setEnabled(true);
	else
		ui.m_bEnterRoom->setEnabled(false);
}

void ZegoMainDialog::on_m_bEnterRoom_clicked()
{
	QString strRoomID = ui.m_edRoomID->text();
	log_string_notice(qtoc(QStringLiteral("[%1]: enter audio live dialog, roomId: %2").arg(__FUNCTION__).arg(strRoomID)));

	RoomPtr pRoom = RoomPtr::create(strRoomID, QString("audio-room"), m_strEdUserId, m_strEdUserName);

	ZegoAudioLiveDialog dialog(pRoom, this);
	connect(&dialog, &ZegoAudioLiveDialog::sigCloseView, this, [this] {this->show(); });

	dialog.initDialog();
	this->hide();
	dialog.exec();
}

void ZegoMainDialog::on_m_bSettings_clicked()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: enter settings dialog").arg(__FUNCTION__)));

	ZegoSettingsDialog settings;
	settings.initDialog();
	settings.exec();

	updateAppVersionTitle();
}


void ZegoMainDialog::on_m_bClose_clicked()
{
	this->close();
}

void ZegoMainDialog::on_m_bMin_clicked()
{
	this->showMinimized();
}

void ZegoMainDialog::on_m_bJumpToNet_clicked()
{
	QDesktopServices::openUrl(QUrl(QLatin1String("https://www.zego.im")));
}
