#include "changeusername.h"
#include "ui_changeusername.h"

#include "configdialog.h"
#include "testwindow.h"

changeUsername::changeUsername(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeUsername)
{
    ui->setupUi(this);

    // Validation for New Username input
    QRegularExpressionValidator* newUsername = new QRegularExpressionValidator(QRegularExpression("[0-9A-Za-z]{1,16}"), ui->newUsernameLabel);
    ui->newUsernameLineEdit->setValidator(newUsername);
}

changeUsername::~changeUsername()
{
    delete ui;
}

void changeUsername::on_resetUsernameButton_clicked()
{
    QString currentUsername = ui->currentUsernameLineEdit->text();
    newUsername = ui->newUsernameLineEdit->text();
    QSqlQuery qqq;

    // Check if Current Username or New Username is "". If so, display error dialog
    if(currentUsername == "" or newUsername == "")
    {
        qDebug() << "Username not set";
        QMessageBox msg(this);
        msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Please enter Current Username AND New Username.");
        return;
    }

    // Check if playername is already used
    if(qqq.exec("SELECT DISTINCT PlayerName FROM playersettings"))
    {
        while(qqq.next())
        {
            QString name = qqq.value(0).toString();
            qDebug() << "This is the value(0): " << name;
            if(currentUsername == name)
            {
                qDebug() << "Username exists. Updating username" << currentUsername << "...";

                // Test if new username is in use
                if(qqq.exec("SELECT DISTINCT PlayerName FROM playersettings"))
                {
                    while(qqq.next())
                    {
                        QString name = qqq.value(0).toString();
                        if(newUsername == name)
                        {
                            qDebug() << "This is the value(0): " << name;
                            qDebug() << "Please use another name.";
                            return;
                        }
                    }
                }
            }
            else
            {
                qDebug() << "Username " << currentUsername << " doesn't exists.";
            }
        }
    }

    qDebug() << "New username is:" << newUsername;

    // CHECK COLOR SECTION
    /* --------------------------------------------------------------------------------------- */

    QString color;

    // Check which box is checked to set color
    if(ui->whiteColor1->isChecked())
    {
        qDebug() << "White is checked";
        newColor = "White";
    }

    else if(ui->blackColor1->isChecked())
    {
        qDebug() << "Black is checked";
        newColor = "Black";
    }

    else if(ui->redColor1->isChecked())
    {
        qDebug() << "Red is checked";
        newColor = "Red";
    }

    else if(ui->cyanColor1->isChecked())
    {
        qDebug() << "Cyan is checked";
        newColor = "Cyan";
    }

    else if(ui->greenColor1->isChecked())
    {
        qDebug() << "Green is checked";
        newColor = "Green";
    }

    else if(ui->magentaColor1->isChecked())
    {
        qDebug() << "Magenta is checked";
        newColor = "Magenta";
    }

    else if(ui->blueColor1->isChecked())
    {
        qDebug() << "Blue is checked";
        newColor = "Blue";
    }

    else if(ui->grayColor1->isChecked())
    {
        qDebug() << "Gray is checked";
        newColor = "Gray";
    }

    else {
        newColor = "";
        qDebug() << "Color remains unchanged.";
    }

    // Check if playerColor is already used
    if(qqq.exec("SELECT DISTINCT Color FROM playersettings"))
    {
        while(qqq.next())
        {
            color = qqq.value(0).toString();
            qDebug() << "This is the value(0): " << color;
            if(newColor == color)
            {
                qDebug() << "Please choose another color.";
                QMessageBox msg(this);
                msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Please choose another color.");
                return;
            }
        }

        qqq.prepare("UPDATE playersettings SET PlayerName = '"+newUsername+"'WHERE PlayerName = '"+currentUsername+"'");

        if(!qqq.exec())
        {
            qDebug() << "Error updating player settings.";
        }
        else
        {
            qDebug() << "Update to playersettings table successful. Username changed to" << newUsername;
            emit usernameChanged(newUsername);
        }

        qqq.prepare("UPDATE rankings SET PlayerName = '"+newUsername+"'WHERE PlayerName = '"+currentUsername+"'");

        if(!qqq.exec())
        {
            qDebug() << "Error updating player settings.";
        }
        else
        {
            qDebug() << "Update to rankings table successful. Username changed to" << newUsername;
        }

        if (newColor != "")
        {
            qqq.prepare("UPDATE playersettings SET Color = '"+newColor+"'WHERE PlayerName = '"+newUsername+"'");

            if(!qqq.exec())
            {
                qDebug() << "Error updating player settings.";
            }
            else
            {
                qDebug() << "Update to playersettings table successful. Color changed to" << newColor;
                emit colorChanged(parsePlayerColor(newColor));
            }
        }
    }

    /* --------------------------------------------------------------------------------------- */

    // Clear checked button and line edits
    ui->newUsernameLineEdit->clear();
    ui->whiteColor1->setChecked(0);
    ui->blackColor1->setChecked(0);
    ui->redColor1->setChecked(0);
    ui->cyanColor1->setChecked(0);
    ui->greenColor1->setChecked(0);
    ui->magentaColor1->setChecked(0);
    ui->blueColor1->setChecked(0);
    ui->grayColor1->setChecked(0);

    emit updateHomeLabel2();
    emit resetClicked();
}

void changeUsername::on_resetUsernameButton_2_clicked()
{
    // Clear checked button and line edits
    ui->newUsernameLineEdit->clear();
    ui->whiteColor1->setChecked(0);
    ui->blackColor1->setChecked(0);
    ui->redColor1->setChecked(0);
    ui->cyanColor1->setChecked(0);
    ui->greenColor1->setChecked(0);
    ui->magentaColor1->setChecked(0);
    ui->blueColor1->setChecked(0);
    ui->grayColor1->setChecked(0);

    emit resetClicked();
}

QString changeUsername::setName(QString s)
{
    ui->currentUsernameLineEdit->setText(s);
    return currentUsername = s;
}

void changeUsername::setColor(QString color)
{
    currentColor = color;
}
