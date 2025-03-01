#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include "changeusername.h"
#include "playercolor.h"

#include <QDialog>
#include <QRegularExpressionValidator>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QStatusBar>
#include <QDialog>
#include <QTextBrowser>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

    void setClearTableButtonEnabled(bool value);

    inline QString const& username() const
    {
        return _username;
    }

    inline PlayerColor color() const
    {
        return _color;
    }

    inline QString colorString(PlayerColor color) const
    {
        switch (color)
        {
        case PlayerColor::Red:
            return "Red";
        case PlayerColor::Blue:
            return "Blue";
        case PlayerColor::Green:
            return "Green";
        case PlayerColor::Magenta:
            return "Magenta";
        case PlayerColor::White:
            return "White";
        case PlayerColor::Black:
            return "Black";
        case PlayerColor::Cyan:
            return "Cyan";
        case PlayerColor::Gray:
            return "Gray";
        }

        return "";
    }

public slots:
    void refreshTable();

private slots:
    /*!
      * \brief Clears all configuration inputs
      * */
    void on_clearButton_clicked();

    /*!
      * \brief Returns to home screen
      * */
    void on_backButton_Config_clicked();

    /*!
      * \brief Adds name and color to database. Will also check if name/color is in use.
      * */
    void on_saveButtonGameConfig_clicked();

    /*!
      * \brief Allows user to change username
      * */
    void on_changeUsernameButton_clicked();

    void setUsername(QString const& value);

    void setColor(PlayerColor value);

    void clearTable();

    void on_nextButton_Config_clicked();

private:
    Ui::ConfigDialog *ui;
    QSqlQueryModel* psModel;
    PlayerColor _color;
    QString _username;

signals:
    void homeClicked();
    void nextClicked_Config();
    void updateHomeLabel1();
    void usernameChanged(QString const& newValue);
    void colorChanged(PlayerColor newValue);
    void changeUsername();

};

#endif // CONFIGURATIONDIALOG_H
