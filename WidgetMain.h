#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QLineEdit>
#include <QWidget>

class WidgetMain : public QWidget
{
	Q_OBJECT

	QLineEdit *editYtDlpExe;
	QLineEdit *editDownloadPath;

public:
	WidgetMain(QWidget *parent = nullptr);
	void closeEvent(QCloseEvent *event) override;
	~WidgetMain();


};
#endif // WIDGETMAIN_H
