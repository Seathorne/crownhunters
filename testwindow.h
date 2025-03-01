#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include "mapscene.h"
#include "playeritem.h"
#include "configdialog.h"
#include "ranksdialog.h"
#include "helpdialog.h"
#include "crownitem.h"
#include "healthbaritem.h"
#include "bulletitem.h"
#include "hostconfigdialog.h"
#include "changeusername.h"
#include "networkwidget.h"
#include "gamestartoverlayitem.h"

#include <QDebug>
#include <QMainWindow>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QStackedWidget>
#include <QTimer>
#include <QTime>
#include <QMessageBox>

namespace Ui {
class TestWindow;
}

/*!
 * \brief The TestWindow class allows various aspects of the game to be tested individually.
 */
class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Constructs a TestWindow which is a child of \a parent.
     * \param parent the parent of this widget
     */
    explicit TestWindow(QWidget* parent = nullptr);
    QString playerName;
    QTimer *runClock;
    QTimer *displayClock;
    //QLabel *displayTime;
    QTime t;
    QString clockText;

protected:
    /*!
     * \brief Resizes the widget and scales the map to fit.
     * \param event contains information about the resize event
     */
    void resizeEvent(QResizeEvent* event) override;

    /*!
     * \brief Shows the widget and scales the map to fit.
     * \param event contains information about the show event
     */
    void showEvent(QShowEvent* event) override;

private:
    //using QMainWindow::mousePressEvent;
    /*!
     * \brief The Qt designer form for this class.
     */
    Ui::TestWindow* ui;

    /*!
     * \brief The graphics scene that contains geometry information for the map.
     */
    MapScene* mapScene;

    /*!
     * \brief database declaration for ch (CrownHunters)
     */
    QSqlDatabase ch;

    /*!
     * \brief query model declatration
     */
    QSqlQueryModel *chModel;

    /*!
     * \brief Objects used for connect statements associated with homeClicked() and moveHome()
     */
    ConfigDialog configDialog;
    HostConfigDialog hostDialog;
    HelpDialog helpDialog;
    RanksDialog ranksDialog;
    changeUsername changeNameDialog;

    QTimer* networkUpdateTimer;

    void closeEvent(QCloseEvent *bar)override;


public slots:
    void sendPlayerUpdate();
    void sendShotBulletUpdate(PlayerColor color, QPointF source, qreal angle);
    void updateMyPlayerColor(PlayerColor newValue);

private slots:
    /*!
     * \brief Prepares and closes the application.
     */
    void cleanClose();

    /*!
     * \brief Creates a modal dialog containing information about the game.
     */
    void createHelpDialog();

    /*!
     * \brief Creates a modal dialog with configuration settings for the game.
     */
    void createConfigDialog();

    /*!
     * \brief Creates a modal dialog containing infomration about player rankings.
     */
    void createRanksDialog();

    /*!
     * \brief Creates a modal dialog for game settings that host can set
     */
    void createGameConfigDialog();

    /*!
     * \brief Function to move to dev page on stackedWidget
     */
    void on_devButton_clicked();

    /*!
     * \brief Function to move to stackedWidget index(0)
     */
    void on_backButton_clicked();

    /*!
     * \brief Function to move to stackedWidget index(0)
     */
    void moveHome();
    void moveNext();
    void labelUpdate1();
    void labelUpdate2();
    void updateUsername();
    void usernameReset();
    void onModeChanged(NetworkWidget::Mode newValue);

    /*!
     * \brief Scales the map to fit on-screen.
     */
    void resizeMap();

    void onGameStarted(int gameTime);

    void onLeftGame();

    void checkTime();

    void updateDisplayClock();

    void on_startGameButton_2_clicked();

signals:
    /*!
     * \brief gameEnded is a custom signal emmitted when the game ends to signal
     * shutdown process
     * \param winner
     */

    // FIgure out to get winning player from mapScene
    void gameEnded();
};

#endif // TESTWINDOW_H
