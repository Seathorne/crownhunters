#ifndef CHANGEUSERNAME_H
#define CHANGEUSERNAME_H

#include "playercolor.h"

#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QRegularExpressionValidator>

namespace Ui {
class changeUsername;
}

class changeUsername : public QDialog
{
    Q_OBJECT

public:
    explicit changeUsername(QWidget *parent = nullptr);
    QString newUsername;
    QString newColor;
    QString currentUsername;
    QString currentColor;
    ~changeUsername();

public slots:
    QString setName(QString s);
    void setColor(QString color);


private slots:
    void on_resetUsernameButton_clicked();

    void on_resetUsernameButton_2_clicked();

private:
    Ui::changeUsername *ui;

signals:
    void updateHomeLabel2();
    void resetClicked();

    void usernameChanged(QString newValue);
    void colorChanged(PlayerColor newValue);
};

#endif // CHANGEUSERNAME_H
