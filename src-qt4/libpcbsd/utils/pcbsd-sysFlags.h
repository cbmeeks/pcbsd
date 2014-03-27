#ifndef _PCBSD_SYSTEM_FLAGS_H
#define _PCBSD_SYSTEM_FLAGS_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QTextStream>

#include <unistd.h>

#include "pcbsd-utils.h"

// DEFINE THE FLAG FILES
#define FLAGDIR QString("/tmp/.pcbsdflags")
#define NETWORKRESTARTED QString("network-restarted")
#define PKGUPDATEAVAILABLE QString("pkg-update-available")
#define SYSUPDATEAVAILABLE QString("sys-update-available")
#define PBIUPDATEAVAILABLE QString("pbi-update-available")
#define WARDENUPDATEAVAILABLE QString("warden-update-available")

// DEFINE THE SIMPLE MESSAGES
#define MWORKING QString("working")
#define MERROR QString("error")
#define MSUCCESS QString("success")
#define MUPDATE QString("updateavailable")
#define MUPDATING QString("updating")

class SystemFlags{
public:
	enum SYSFLAG{ NetRestart, PkgUpdate, SysUpdate, PbiUpdate, WardenUpdate};
	enum SYSMESSAGE{ Working, Error, Success, UpdateAvailable, Updating};
	static void setFlag( SYSFLAG flag, SYSMESSAGE msg){
	  if(!QFile::exists(FLAGDIR)){
	    pcbsd::Utils::runShellCommand("mkdir -p -m 777 "+FLAGDIR);
	    if( !QFile::exists(FLAGDIR) ){ return; }
	  }
	  QString cmd = "echo '%2' > %1";
	  //Get the Message Type
	  QString contents;
	  switch(msg){
	    case Working:
		contents = MWORKING; break;
	    case Error:
		contents = MERROR; break;
	    case Success:
		contents = MSUCCESS; break;
	    case UpdateAvailable:
		contents = MUPDATE; break;
	    case Updating:
		contents = MUPDATING; break;
	    default:
		return; //invalid message
	  }
	  //Now get the flag type
	  QString user = "-"+QString( getlogin() );
	  QString file = FLAGDIR+"/";
	  switch(flag){
	    case NetRestart:
		file = NETWORKRESTARTED+user; break;
	    case PkgUpdate:
		file = PKGUPDATEAVAILABLE+user; break;
	    case SysUpdate:
		file = SYSUPDATEAVAILABLE+user; break;
	    case PbiUpdate:
		file = PBIUPDATEAVAILABLE+user; break;
	    case WardenUpdate:
		file = WARDENUPDATEAVAILABLE+user; break;
	    default:
		return; //invalid flag
	  }
	  cmd = cmd.arg(file, contents);
	  if(QFile::exists(file)){ pcbsd::Utils::runShellCommand("rm "+file); } //make sure watchers can see the change
	  pcbsd::Utils::runShellCommand(cmd);
	}

};

class SystemFlagWatcher : public QObject{
	Q_OBJECT
public:
	SystemFlagWatcher(QObject *parent = 0);
	~SystemFlagWatcher();

	void checkForRecent(int minutes);

private:
	QDateTime CDT;  //current date time
	QFileSystemWatcher *watcher;

private slots:
	void watchFlagDir();
	void watcherNotification();
	QString quickRead(QString filepath);

signals:
	void FlagChanged(SystemFlags::SYSFLAG, SystemFlags::SYSMESSAGE);
};

#endif