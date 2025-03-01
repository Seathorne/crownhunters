#include "ranksdialog.h"
#include "ui_ranksdialog.h"

RanksDialog::RanksDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RanksDialog)
{
    ui->setupUi(this);

    // create sql query model
    ranksModel = new QSqlQueryModel;
    ranksModel->setQuery("SELECT * FROM rankings ORDER BY Wins DESC");
    ui->rankingsTableView->setModel(ranksModel);
}

RanksDialog::~RanksDialog()
{
    delete ui;
}

void RanksDialog::on_backButton_Rankings_clicked()
{
    emit homeClicked();
}

void RanksDialog::on_refreshButton_Rankings_clicked()
{
    QSqlQuery qry;
    int count = 0;
    qry.exec("SELECT COUNT(*) FROM rankings");

    if(qry.first())
    {
        count = qry.value(0).toInt();
        qDebug() << "Count is: " << count;
        if(count == 0)
        {
            QMessageBox msg(this);
            msg.critical(this,"Error","<font color=\"White\"><font size=\"5\">Rankings currently not available.");
        }
    }

    ranksModel->setQuery("SELECT * FROM rankings ORDER BY Wins DESC");
    ui->rankingsTableView->setModel(ranksModel);
}
