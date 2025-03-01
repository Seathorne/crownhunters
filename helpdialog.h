#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private slots:
    void on_backButton_Help_clicked();

private:
    Ui::HelpDialog *ui;

signals:
    /*!
     * \brief Signal used to move home
     */
    void homeClicked();
};

#endif // HELPDIALOG_H
