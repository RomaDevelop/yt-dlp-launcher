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

	auto glo = new QGridLayout(this);

	glo->addWidget(new QLabel("yt-dlp exe"), 0, 0);

	auto hloForRow0 = new QHBoxLayout;
	glo->addLayout(hloForRow0, 0, 1, 1, -1);

	editYtDlpExe = new QLineEdit;
	hloForRow0->addWidget(editYtDlpExe);

	auto btnChoseYtDlp = new QPushButton("...");
	btnChoseYtDlp->setFixedWidth(QFontMetrics(btnChoseYtDlp->font()).horizontalAdvance(btnChoseYtDlp->text()) + 10);
	btnChoseYtDlp->setToolTip("Выбрать");
	hloForRow0->addWidget(btnChoseYtDlp);

	auto btnOpenYtDlp = new QPushButton("");
	btnOpenYtDlp->setToolTip("Показать в проводнике");
	btnOpenYtDlp->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	hloForRow0->addWidget(btnOpenYtDlp);

	glo->addWidget(new QLabel("download path"), 1, 0);

	auto hloForRow1 = new QHBoxLayout;
	glo->addLayout(hloForRow1, 1, 1, 1, -1);

	editDownloadPath = new QLineEdit;
	hloForRow1->addWidget(editDownloadPath);

	auto btnChoseDowloadPath = new QPushButton("...");
	btnChoseDowloadPath->setFixedWidth(QFontMetrics(btnChoseDowloadPath->font()).horizontalAdvance(btnChoseDowloadPath->text()) + 10);
	btnChoseDowloadPath->setToolTip("Выбрать");
	hloForRow1->addWidget(btnChoseDowloadPath);

	auto btnOpenDowloadPath = new QPushButton("");
	btnOpenDowloadPath->setToolTip("Показать в проводнике");
	btnOpenDowloadPath->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	hloForRow1->addWidget(btnOpenDowloadPath);

	glo->addWidget(new QLabel("command"), 2, 0);
	auto command = new QLineEdit;
	glo->addWidget(command, 2, 1);

	auto hloForButtonExec = new QHBoxLayout;
	hloForButtonExec->addStretch();
	glo->addLayout(hloForButtonExec, 3, 0, 1, -1);
	auto btnLaunch = new QPushButton("Выполнить");
	hloForButtonExec->addWidget(btnLaunch);

	auto hloForButtonNote = new QHBoxLayout;
	hloForButtonNote->addStretch();
	glo->addLayout(hloForButtonNote, 4, 0, 1, -1);
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

	connect(btnChoseYtDlp, &QPushButton::clicked, [this](){
		if(auto file = QFileDialog::getOpenFileName(nullptr, "Открыть файл", "", "Executable Files (*.exe)"); file != "")
			editYtDlpExe->setText(file);
	});

	connect(btnOpenYtDlp, &QPushButton::clicked, [this](){
		MyQExecute::ShowInExplorer(editYtDlpExe->text());
	});

	connect(btnChoseDowloadPath, &QPushButton::clicked, [this](){
		if(auto dir = QFileDialog::getExistingDirectory(nullptr, "Выберите каталог"); dir != "")
			editDownloadPath->setText(dir);
	});

	connect(btnOpenDowloadPath, &QPushButton::clicked, [this](){
		MyQExecute::OpenDir(editDownloadPath->text());
	});

	connect(btnLaunch, &QPushButton::clicked, [this, command](){
		MyQFileDir::WriteFile(pathFiles + "/command.bat",
							editDownloadPath->text().left(2) + "\n"
							"cd \"" + editDownloadPath->text() + "\""
							"\n\"" + editYtDlpExe->text() + "\" " + command->text() + " \n"
							  + "pause\n");
		//MyQExecute::Execute(pathFiles + "/command.bat");
		system(("\"" + pathFiles + "/command.bat" + "\"").toStdString().c_str());
	});

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

