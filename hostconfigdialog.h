#ifndef HOSTCONFIGDIALOG_H
#define HOSTCONFIGDIALOG_H

#include <QDialog>
#include <QtDebug>
#include <QByteArray>
#include <QSqlQuery>

namespace Ui {
class HostConfigDialog;
}

class HostConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostConfigDialog(QWidget *parent = nullptr);
    ~HostConfigDialog();

    /*!
     * \brief Time variable set for the game configuration
     */
    int gameTimeValue;


private:
    Ui::HostConfigDialog *ui;

private slots:
    /*!
      * \brief Clears all configuration inputs
      * */
    void on_clearButton_clicked();

    /*!
     * \brief Holder Slot for future dialog functionality
     */
    void on_saveButtonHost_clicked();
    void on_backButton_Host_clicked();

signals:
    /*!
     * \brief Signal used to move home
     */
    void homeClicked();

    void gameTimeChanged(int newValue);
    void maxPlayersChanged(int newValue);
};

#endif // HOSTCONFIGDIALOG_H
