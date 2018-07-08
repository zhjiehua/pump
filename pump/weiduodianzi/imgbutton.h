#ifndef IMGBUTTON_H
#define IMGBUTTON_H

#include <QPushButton>

class ImgButton : public QPushButton
{
	Q_OBJECT

public:
	ImgButton(QWidget *parent = 0);
	~ImgButton();

	void setBkImage(QString strTemp);

protected:
	virtual void paintEvent(QPaintEvent * event);

private:
	QString m_bkPath;
};

#endif // IMGBUTTON_H
