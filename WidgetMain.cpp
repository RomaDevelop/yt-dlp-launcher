#include "WidgetMain.h"

#include <windows.h>

#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QApplication>
#include <QStyle>
#include <QFileDialog>
#include <QTimer>
#include <QComboBox>

#include "MyQDifferent.h"
#include "MyQExecute.h"
#include "MyQFileDir.h"
#include "MyQShortings.h"

QString pathFiles;
QString settingsFileName;
QString notesFileName;

WidgetMain::WidgetMain(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

	auto gloMain = new QGridLayout(this);

	gloMain->addWidget(new QLabel("yt-dlp exe"), 0, 0);

	auto hloForRow0 = new QHBoxLayout;
	gloMain->addLayout(hloForRow0, 0, 1, 1, -1);

	editYtDlpExe = new QLineEdit;
	hloForRow0->addWidget(editYtDlpExe);

	auto btnChoseYtDlp = new QPushButton("...");
	btnChoseYtDlp->setFixedWidth(QFontMetrics(btnChoseYtDlp->font()).horizontalAdvance(btnChoseYtDlp->text()) + 10);
	btnChoseYtDlp->setToolTip("Выбрать");
	hloForRow0->addWidget(btnChoseYtDlp);
	connect(btnChoseYtDlp, &QPushButton::clicked, [this](){
		if(auto file = QFileDialog::getOpenFileName(nullptr, "Открыть файл", "", "Executable Files (*.exe)"); file != "")
			editYtDlpExe->setText(file);
	});

	auto btnOpenYtDlp = new QPushButton("");
	btnOpenYtDlp->setToolTip("Показать в проводнике");
	btnOpenYtDlp->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	hloForRow0->addWidget(btnOpenYtDlp);
	connect(btnOpenYtDlp, &QPushButton::clicked, [this](){
		MyQExecute::ShowInExplorer(editYtDlpExe->text());
	});

	gloMain->addWidget(new QLabel("download path"), 1, 0);

	auto hloForRow1 = new QHBoxLayout;
	gloMain->addLayout(hloForRow1, 1, 1, 1, -1);

	editDownloadPath = new QLineEdit;
	hloForRow1->addWidget(editDownloadPath);

	auto btnChoseDowloadPath = new QPushButton("...");
	btnChoseDowloadPath->setFixedWidth(QFontMetrics(btnChoseDowloadPath->font()).horizontalAdvance(btnChoseDowloadPath->text()) + 10);
	btnChoseDowloadPath->setToolTip("Выбрать");
	hloForRow1->addWidget(btnChoseDowloadPath);
	connect(btnChoseDowloadPath, &QPushButton::clicked, [this](){
		if(auto dir = QFileDialog::getExistingDirectory(nullptr, "Выберите каталог"); dir != "")
			editDownloadPath->setText(dir);
	});

	auto btnOpenDowloadPath = new QPushButton("");
	btnOpenDowloadPath->setToolTip("Показать в проводнике");
	btnOpenDowloadPath->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	hloForRow1->addWidget(btnOpenDowloadPath);
	connect(btnOpenDowloadPath, &QPushButton::clicked, [this](){
		MyQExecute::OpenDir(editDownloadPath->text());
	});

	int row = gloMain->rowCount();
	gloMain->addWidget(new QLabel("name file"), row, 0);
	auto lineEditNameFile = new QLineEdit;
	lineEditNameFile->setToolTip("input name video to set or leave blank for default");
	gloMain->addWidget(lineEditNameFile, row, 1);

	row = gloMain->rowCount();
	gloMain->addWidget(new QLabel("resolution"), row, 0);
	auto comboResolution = new QComboBox;
	comboResolution->setEditable(true);
	comboResolution->addItems({"", "height:1080", "width:1920"});
	gloMain->addWidget(comboResolution, row, 1);

	row = gloMain->rowCount();
	gloMain->addWidget(new QLabel("cookies"), row, 0);
	auto comboBoxCookies = new QComboBox;
	comboBoxCookies->addItems({"", "firefox", "chrome"});
	gloMain->addWidget(comboBoxCookies, row, 1);

	row = gloMain->rowCount();
	gloMain->addWidget(new QLabel("link and other"), row, 0);
	auto lineEditLinkAndOther = new QLineEdit;
	gloMain->addWidget(lineEditLinkAndOther, row, 1);

	auto hloForButtonExec = new QHBoxLayout;
	hloForButtonExec->addStretch();
	gloMain->addLayout(hloForButtonExec, gloMain->rowCount(), 0, 1, -1);
	auto btnLaunch = new QPushButton("Выполнить");
	hloForButtonExec->addWidget(btnLaunch);
	connect(btnLaunch, &QPushButton::clicked, [this, lineEditNameFile, comboResolution, comboBoxCookies, lineEditLinkAndOther](){

		QString linkAndOther = lineEditLinkAndOther->text();
		QString downLoadPath = editDownloadPath->text();
		QString ytDlpExe = editYtDlpExe->text();
		if(linkAndOther.isEmpty() || !QFileInfo(ytDlpExe).isFile() || !QFileInfo(downLoadPath).isDir())
		{ QMbError("empty ytDlpExe or downLoadPath or linkAndOther"); return; }

		QString name = lineEditNameFile->text();
		if(!name.isEmpty()) name = " -o \""+name+".%%(ext)s\"";

		QString resolution = comboResolution->currentText();
		if(!resolution.isEmpty()) resolution = " -S \""+resolution+"\"";

		QString cookies = comboBoxCookies->currentText();
		if(!cookies.isEmpty()) cookies = " --cookies-from-browser "+cookies;

		QString command = "\"" + ytDlpExe + "\"" + name + resolution + cookies;
		command += " ";
		command += linkAndOther;
		command += "\n";

		QString bat = downLoadPath.left(2) + "\n";
		bat += "cd \"" + downLoadPath + "\"\n";
		bat += command;
		bat += "pause\n";

		MyQFileDir::WriteFile(pathFiles + "/command.bat", bat);
		system(("\"" + pathFiles + "/command.bat" + "\"").toStdString().c_str());
	});

	auto hloForButtonNote = new QHBoxLayout;
	hloForButtonNote->addStretch();
	gloMain->addLayout(hloForButtonNote, gloMain->rowCount(), 0, 1, -1);
	auto btnNote = new QPushButton("Заметка");
	hloForButtonNote->addWidget(btnNote);

	pathFiles = MyQDifferent::PathToExe() + "/files";
	QDir().mkdir(pathFiles);

	settingsFileName = pathFiles + "/settings.ini";
	if(QFileInfo(settingsFileName).isFile())
	{
		QSettings settings(settingsFileName, QSettings::IniFormat);
		restoreGeometry(settings.value("geo").toByteArray());
		editYtDlpExe->setText(settings.value("yt-dlp-exe").toString());
		editDownloadPath->setText(settings.value("downloads_path").toString());
	}




	struct Setting
	{
		QString caption;
		QString hint;
		QString command;
	};
	std::vector<Setting> settings;
	settings.push_back({"height", "Разрешение (будет скачано не более или точно такое)", "-S \"height:%\""});
	settings.push_back({"name video", "Имя скачиваемого файла (расширение не указывать)", "-o \"%\""});

	notesFileName = pathFiles + "/note.txt";
	connect(btnNote, &QPushButton::clicked, [this](){
		if(!QFileInfo::exists(notesFileName))
		{
			if(!MyQFileDir::WriteFile(notesFileName, ""))
				QMbc(this,"Error", "Can't create " + notesFileName);
		}

		if(QFileInfo(notesFileName).isFile())
			MyQExecute::Execute(pathFiles + "/note.txt");
		else QMbc(this,"Error", notesFileName + " is not file");
	});
}

void WidgetMain::closeEvent(QCloseEvent *) {
	QSettings settings(settingsFileName, QSettings::IniFormat);
	settings.setValue("geo", this->saveGeometry());
	settings.setValue("yt-dlp-exe", editYtDlpExe->text());
	settings.setValue("downloads_path", editDownloadPath->text());
}

WidgetMain::~WidgetMain()
{

}

