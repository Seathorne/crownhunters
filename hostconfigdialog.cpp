#include "hostconfigdialog.h"
#include "ui_hostconfigdialog.h"

HostConfigDialog::HostConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostConfigDialog)
{
    ui->setupUi(this);

    //Set default values for slider and label
    ui->horizontalSlider->setValue(1);
    ui->gameTime->setNum(1);

    // Connect Slider with Label output
    connect(ui->horizontalSlider, &QSlider::valueChanged,
                     ui->gameTime, static_cast<void (QLabel::*)(int)>(&QLabel::setNum));
    // Set slider boundaries
    ui->horizontalSlider->setRange(1,5);
    ui->horizontalSlider->setTickInterval(1);

    // Save default host settings on startup
    this->on_clearButton_clicked();
    this->on_saveButtonHost_clicked();
}

HostConfigDialog::~HostConfigDialog()
{
    delete ui;
}

void HostConfigDialog::on_clearButton_clicked()
{
    ui->eightPlayerOption->click();
    ui->horizontalSlider->setValue(1);

    emit maxPlayersChanged(8);
    emit gameTimeChanged(1);
}

void HostConfigDialog::on_saveButtonHost_clicked()
{
    QSqlQuery qqqq;
    QString gameTimer;

    int playerNumber;

    // CHECK PLAYER NUMBER SECTION
    /* --------------------------------------------------------------------------------------- */

    // Check which box is checked to set player number
    if(ui->twoPlayerOption->isChecked())
    {
        qDebug() << "Two Players is checked";
        playerNumber = 2;
    }

    else if(ui->threePlayerOption->isChecked())
    {
        qDebug() << "Three Players is checked";
        playerNumber = 3;
    }

    else if(ui->fourPlayerOption->isChecked())
    {
        qDebug() << "Four Players is checked";
        playerNumber = 4;
    }

    else if(ui->fivePlayerOption->isChecked())
    {
        qDebug() << "Five Players is checked";
        playerNumber = 5;
    }

    else if(ui->sixPlayerOption->isChecked())
    {
        qDebug() << "Six Players is checked";
        playerNumber = 6;
    }

    else if(ui->sevenPlayerOption->isChecked())
    {
        qDebug() << "Seven Players is checked";
        playerNumber = 7;
    }

    else
    {
        qDebug() << "Eight Players is checked";
        playerNumber = 8;
    }

    /* --------------------------------------------------------------------------------------- */

    // CHECK GAME TIME SECTION
    // SET PUBLIC GAME TIME VARIABLE
    /* --------------------------------------------------------------------------------------- */

    gameTimer = ui->gameTime->text();
    gameTimeValue = gameTimer.toInt();

    emit gameTimeChanged(gameTimeValue);
    emit maxPlayersChanged(playerNumber);

    /* --------------------------------------------------------------------------------------- */
}

void HostConfigDialog::on_backButton_Host_clicked()
{
    emit homeClicked();
}
