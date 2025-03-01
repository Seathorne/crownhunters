#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    // Create sql query model
    psModel = new QSqlQueryModel;
    psModel->setQuery("SELECT * FROM playersettings");
    ui->gameConfigTableView->setModel(psModel);

    // Validation for username input
    QRegularExpressionValidator* usernameValidator = new QRegularExpressionValidator(QRegularExpression("[0-9A-Za-z]{1,16}"), ui->usernameEdit);
    ui->usernameEdit->setValidator(usernameValidator);

    this->setClearTableButtonEnabled(false);
    connect(ui->clearTableButton, &QAbstractButton::clicked, this, &ConfigDialog::clearTable);
    connect(ui->refreshTableButton, &QAbstractButton::clicked, this, &ConfigDialog::refreshTable);

    // Disables Change Username button on launch. Button will become enabled when user saves username.
    ui->changeUsernameButton->setDisabled(1);

    // Disables Next button on launch. Button will become enabled when user saves username.
    ui->nextButton_Config->setDisabled(1);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setClearTableButtonEnabled(bool value)
{
    ui->clearTableButton->setEnabled(value);
}

void ConfigDialog::on_clearButton_clicked()
{
    ui->usernameEdit->clear();
}

void ConfigDialog::on_saveButtonGameConfig_clicked()
{
    // Query object
    QSqlQuery qq;

    // CHECK USERNAME SECTION
    /* --------------------------------------------------------------------------------------- */

    // Set username
    QString playerName = ui->usernameEdit->text();
    qDebug() << "Username is" << playerName;

    // Check if username is ""
    if(playerName == "")
    {
        qDebug() << "Username not set";
        ui->saveButtonGameConfig->setChecked(0);
        QMessageBox msg(this);
        msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Please enter a Username.");
        return;
    }

    // Check if playername is already used
    if(qq.exec("SELECT DISTINCT PlayerName FROM playersettings"))
    {
        while(qq.next())
        {
            QString name = qq.value(0).toString();
            qDebug() << "This is the value(0): " << name;
            if(playerName == name)
            {
                ui->saveButtonGameConfig->setChecked(0);
                qDebug() << "Please use another name.";
                QMessageBox msg(this);
                msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Please choose another name.");
                return;
            }
        }
    }

    /* --------------------------------------------------------------------------------------- */

    // CHECK COLOR SECTION
    /* --------------------------------------------------------------------------------------- */

    // Check which box is checked to set color
    PlayerColor color;
    if(ui->whiteColor->isChecked())
    {
        color = PlayerColor::White;
    }
    else if(ui->blackColor->isChecked())
    {
        color = PlayerColor::Black;
    }
    else if(ui->redColor->isChecked())
    {
        color = PlayerColor::Red;
    }
    else if(ui->cyanColor->isChecked())
    {
        color = PlayerColor::Cyan;
    }
    else if(ui->greenColor->isChecked())
    {
        color = PlayerColor::Green;
    }
    else if(ui->magentaColor->isChecked())
    {
        color = PlayerColor::Magenta;
    }
    else if(ui->blueColor->isChecked())
    {
        color = PlayerColor::Blue;
    }
    else if(ui->grayColor->isChecked())
    {
        color = PlayerColor::White;
    }
    else
    {
        qDebug() << "Color not checked!";
        ui->saveButtonGameConfig->setChecked(0);
        QMessageBox msg(this);
        msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Color not selected. Please select a color.");
        return;
    }

    QString colorStr = colorString(color);

    // Check if playerColor is already used
    if(qq.exec("SELECT DISTINCT Color FROM playersettings"))
    {
        while(qq.next())
        {
            QString usedColor = qq.value(0).toString();
            qDebug() << "This is the value(0): " << usedColor;
            if(usedColor == colorStr)
            {
                ui->saveButtonGameConfig->setChecked(0);
                qDebug() << "Please choose another color.";
                QMessageBox msg(this);
                msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Please choose another color.");
                return;
            }
        }
    }

    /* --------------------------------------------------------------------------------------- */

    // INSERT/UPDATE TABLE
    /* --------------------------------------------------------------------------------------- */
    qq.prepare("INSERT INTO playersettings VALUES(:PlayerName, :Color)");
    qq.bindValue(":PlayerName", ui->usernameEdit->text());
    qq.bindValue(":Color", colorStr);

    if(!qq.exec())
    {
        qDebug() << "Error inserting player settings.";
        return;
    }

    // update contents in table view
    psModel->setQuery("SELECT * FROM playersettings");
    ui->gameConfigTableView->setModel(psModel);

    // Insert PlayerName into rankings table
    qq.prepare("INSERT INTO rankings VALUES(:PlayerName, :Wins, :Loses)");
    qq.bindValue(":PlayerName", ui->usernameEdit->text());
    qq.bindValue(":Wins", 0);
    qq.bindValue(":Loses", 0);

    if(!qq.exec())
    {
        qDebug() << "Error inserting data into rankings table.";
        return;
    }

    // If we make it here, username/color were able to be inserted to database and can be set
    this->setUsername(playerName);
    this->setColor(color);

    /* --------------------------------------------------------------------------------------- */

    // Clear username field
    ui->usernameEdit->clear();

    // Disable button
    ui->saveButtonGameConfig->setDisabled(1);

    // Enable Change Username and Next button
    ui->changeUsernameButton->setEnabled(1);
    ui->nextButton_Config->setEnabled(1);

    refreshTable();

    // Update username label
    emit updateHomeLabel1();
}

void ConfigDialog::on_backButton_Config_clicked()
{
    emit homeClicked();
}

void ConfigDialog::on_changeUsernameButton_clicked()
{
    ui->changeUsernameButton->setChecked(0);
    refreshTable();
    setUsername(ui->usernameEdit->text());
    emit changeUsername();
}

void ConfigDialog::refreshTable()
{
    psModel->setQuery("SELECT * FROM playersettings");
    ui->gameConfigTableView->setModel(psModel);
}

void ConfigDialog::setUsername(QString const& value)
{
    _username = value;
    refreshTable();
    emit usernameChanged(value);
}

void ConfigDialog::setColor(PlayerColor value)
{
    _color = value;
    refreshTable();
    emit colorChanged(value);
}

void ConfigDialog::clearTable()
{
    QSqlQuery q;

    // Clears playersettings table when game is closed
    if(q.exec("DELETE FROM playersettings") == false)
    {
        qDebug() << "Error clearing existing table.";
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
        return;
    }

    // Refresh table view
    refreshTable();

    // Enable save button
    ui->saveButtonGameConfig->setEnabled(1);
    ui->saveButtonGameConfig->setChecked(0);
}

void ConfigDialog::on_nextButton_Config_clicked()
{
    emit nextClicked_Config();
}
