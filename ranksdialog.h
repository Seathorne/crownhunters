#ifndef RANKSDIALOG_H
#define RANKSDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QStackedWidget>
#include <QMessageBox>

namespace Ui {
class RanksDialog;
}

class RanksDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RanksDialog(QWidget *parent = nullptr);
    ~RanksDialog();

private slots:
    void on_backButton_Rankings_clicked();

    void on_refreshButton_Rankings_clicked();

private:
    Ui::RanksDialog *ui;

    /*!
     * \brief database declaration for ch (CrownHunters)
     */
    QSqlDatabase ch;

    /*!
     * \brief query model declatration
     */
    QSqlQueryModel *ranksModel;

signals:
    /*!
     * \brief Signal used to move home
     */
    void homeClicked();
};

#endif // RANKSDIALOG_H
