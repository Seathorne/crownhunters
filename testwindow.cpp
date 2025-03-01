#include "testwindow.h"
#include "ui_testwindow.h"

TestWindow::TestWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::TestWindow),
    mapScene(new MapScene(this))
{
    ui->setupUi(this);

    ui->statusBar->addPermanentWidget(ui->usernameLabel);

    // Create database
    ch=QSqlDatabase::addDatabase("QSQLITE");
    ch.setDatabaseName("crownhunters.db");
    //ch.open();

    QSqlQuery q;

    // Check to make sure database opens successfully
    if (!ch.open())
    {
        qDebug() << ch.lastError();
        qDebug() << "Error 1: Unable to connect due to above error";
        return;
    }

        // Drops rankings table when game is closed
        if(q.exec("DROP TABLE IF EXISTS rankings") == false)
        {
            qDebug() << "Error dropping existing table.";
            qDebug() << q.lastError();
            qDebug() << q.lastQuery();
            return;
        }

        // Drops playersettings table when game is closed
        if(q.exec("DROP TABLE IF EXISTS playersettings") == false)
        {
            qDebug() << "Error dropping existing table.";
            qDebug() << q.lastError();
            qDebug() << q.lastQuery();
            return;
        }

    if(!q.exec("CREATE TABLE playersettings (PlayerName TEXT, Color TEXT)"))
    {
        qDebug() << ch.lastError();
        qDebug() << "Table already exists";
    }

    if(!q.exec("CREATE TABLE rankings (PlayerName TEXT, Wins INT, Loses INT)"))
    {
        qDebug() << ch.lastError();
        qDebug() << "Table already exists";
    }

    // Add config/host/help/ranks Dialog to stackedWidget
    ui->stackedWidget->insertWidget(2, &configDialog);
    ui->stackedWidget->insertWidget(3, &hostDialog);
    ui->stackedWidget->insertWidget(4, &helpDialog);
    ui->stackedWidget->insertWidget(5, &ranksDialog);
    ui->stackedWidget->insertWidget(6, &changeNameDialog);

    // Set up map
    ui->mapGraphicsView->setScene(mapScene);

    // Connect menu bar actions
    connect(ui->actionExit, &QAction::triggered, this, &TestWindow::cleanClose);
    connect(ui->actionRankings, &QAction::triggered, this, &TestWindow::createRanksDialog);
    connect(ui->actionPlayer_Options, &QAction::triggered, this, &TestWindow::createConfigDialog);
    connect(ui->actionGame_Options, &QAction::triggered, this, &TestWindow::createGameConfigDialog);
    connect(ui->actionInstructions, &QAction::triggered, this, &TestWindow::createHelpDialog);

    // Connect buttons
    connect(ui->exitButton, &QAbstractButton::clicked, this, &TestWindow::cleanClose);
    connect(ui->openRanksButton, &QAbstractButton::clicked, this, &TestWindow::createRanksDialog);
    connect(ui->openHelpButton, &QAbstractButton::clicked, this, &TestWindow::createHelpDialog);

    // Connect statements for stackedWidget
    connect(&configDialog, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    connect(&hostDialog, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    connect(&helpDialog, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    connect(&ranksDialog, SIGNAL(homeClicked()), this, SLOT(moveHome()));
    connect(&configDialog, SIGNAL(nextClicked_Config()), this, SLOT(moveNext()));

    // Connect statement to update username label
    connect(&configDialog, SIGNAL(updateHomeLabel1()), this, SLOT(labelUpdate1()));
    connect(&changeNameDialog, SIGNAL(updateHomeLabel2()), this, SLOT(labelUpdate2()));
    connect(&changeNameDialog, SIGNAL(resetClicked()), this, SLOT(usernameReset()));
    connect(&configDialog, SIGNAL(changeUsername()), this, SLOT(updateUsername()));

    // Connect network updates
    networkUpdateTimer = new QTimer();
    networkUpdateTimer->setSingleShot(false);
    connect(networkUpdateTimer, &QTimer::timeout, this, &TestWindow::sendPlayerUpdate);
    networkUpdateTimer->start(NETWORK_UPDATE_RATE);

    // Connect network<-->map/UI updates
    connect(ui->newNetworkWidget->client(), &NetworkClient::positionUpdated, mapScene, &MapScene::onPositionUpdated);
    connect(ui->newNetworkWidget->client(), &NetworkClient::bulletUpdated, mapScene, &MapScene::onBulletUpdated);
    connect(ui->newNetworkWidget->client(), &NetworkClient::healthUpdated, mapScene, &MapScene::onHealthUpdated);
    connect(ui->newNetworkWidget->client(), &NetworkClient::gameStarted, this, &TestWindow::onGameStarted);
    connect(ui->newNetworkWidget->client(), &NetworkClient::leftGame, this, &TestWindow::onLeftGame);

    connect(ui->newNetworkWidget->host(), &NetworkHost::positionUpdated, mapScene, &MapScene::onPositionUpdated);
    connect(ui->newNetworkWidget->host(), &NetworkHost::bulletUpdated, mapScene, &MapScene::onBulletUpdated);
    connect(ui->newNetworkWidget->host(), &NetworkHost::healthUpdated, mapScene, &MapScene::onHealthUpdated);
    connect(ui->newNetworkWidget->host(), &NetworkHost::gameStarted, this, &TestWindow::onGameStarted);
    connect(ui->newNetworkWidget->host(), &NetworkHost::stoppedHosting, this, &TestWindow::onLeftGame);

    // TODO currently says host always wins. Change once rankings are implemented.
    connect(this, &TestWindow::gameEnded, [=] { ui->newNetworkWidget->host()->endGame(ui->newNetworkWidget->host()->color(), ui->newNetworkWidget->host()->username()); });

    connect(ui->newNetworkWidget, &NetworkWidget::modeChanged, this, &TestWindow::onModeChanged);
    onModeChanged(NetworkWidget::None);

    // Connect updates from other pages to update the network settings
    connect(&configDialog, &ConfigDialog::usernameChanged, ui->newNetworkWidget, &NetworkWidget::setUsername);
    connect(&configDialog, &ConfigDialog::colorChanged, ui->newNetworkWidget, &NetworkWidget::setColor);
    connect(&configDialog, &ConfigDialog::colorChanged, [=](PlayerColor newValue) { changeNameDialog.setColor(playerColorToString(newValue)); });
    connect(&changeNameDialog, &changeUsername::usernameChanged, ui->newNetworkWidget, &NetworkWidget::setUsername);
    connect(&changeNameDialog, &changeUsername::colorChanged, ui->newNetworkWidget, &NetworkWidget::setColor);
    connect(&changeNameDialog, &changeUsername::resetClicked, &configDialog, &ConfigDialog::refreshTable);
    connect(&hostDialog, &HostConfigDialog::maxPlayersChanged, ui->newNetworkWidget, &NetworkWidget::setMaxPlayers);
    connect(&hostDialog, &HostConfigDialog::gameTimeChanged, ui->newNetworkWidget, &NetworkWidget::setGameTime);

    //Game timer
    // Create timer for game
    runClock = new QTimer(this);
    displayClock = new QTimer;
    // Timeout is set in the StartGame slot
    // When timeout is set -> go to check Time
    // Then leave game
    connect(runClock, SIGNAL(timeout()), this, SLOT(checkTime()));

    // Connect player config to map
    connect(&configDialog, &ConfigDialog::colorChanged, this, &TestWindow::updateMyPlayerColor);
    this->updateMyPlayerColor(PlayerColor::Red);

    // Set active tab to map screen, resize it to fit in view,
    // then set active tab back to network screen
    ui->tabWidget->setCurrentIndex(1);
    resizeMap();
    ui->tabWidget->setCurrentIndex(0);

    // Move to home screen
    ui->stackedWidget->setCurrentIndex(0);
}

void TestWindow::updateMyPlayerColor(PlayerColor newValue)
{
    // Disconnect old player who will no longer be "myPlayer"
    disconnect(mapScene->myPlayer(), &PlayerItem::shotBullet, this, &TestWindow::sendShotBulletUpdate);

    // Connect new "myPlayer"
    mapScene->setMyColor(newValue);
    connect(mapScene->myPlayer(), &PlayerItem::shotBullet, this, &TestWindow::sendShotBulletUpdate);
}

void TestWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    resizeMap();
}

void TestWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    resizeMap();
}

void TestWindow::closeEvent(QCloseEvent *bar)
{
    QSqlQuery q;

    // Drops rankings table when game is closed
    if(q.exec("DROP TABLE IF EXISTS rankings") == false)
    {
        qDebug() << "Error dropping existing table.";
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
        return;
    }

    // Drops playersettings table when game is closed
    if(q.exec("DROP TABLE IF EXISTS playersettings") == false)
    {
        qDebug() << "Error dropping existing table.";
        qDebug() << q.lastError();
        qDebug() << q.lastQuery();
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Close", "<font color=\"White\"><font size=\"5\">Player options data will be deleted. Do you want to close the program? ",
                          QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        // Check if playername is already used
        if(q.exec("SELECT DISTINCT PlayerName FROM playersettings"))
        {
            while(q.next())
            {
                QString name = q.value(0).toString();
                qDebug() << "This is the value(0): " << name;
                if(playerName == name)
                {
                    qDebug() << "Playername exists. Deleting entry...";
                    q.exec("DELETE FROM playersettings WHERE PlayerName = '"+playerName+"'");

                    if(!q.exec())
                    {
                        qDebug() << "Error deleting player settings.";
                        return;
                    }

                    qDebug() << "Entry deleted.";
                }
            }
        }

        bar->accept();
    }
    else
    {
        qDebug() << "No has been clicked.";
        bar->ignore();
    }
}

void TestWindow::updateDisplayClock()
{
    t = t.addSecs(-1);  // Decrement time by a second
    clockText = t.toString("mm:ss");    // Convert time to a string
    ui->displayTime->setText(clockText);    // Display time in label
    //qDebug() << clockText;
}

void TestWindow::resizeMap()
{
    ui->mapGraphicsView->ensureVisible(mapScene->sceneRect());
    ui->mapGraphicsView->fitInView(mapScene->sceneRect(), Qt::KeepAspectRatio);
    ui->mapGraphicsView->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
}

void TestWindow::cleanClose()
{
    exit(0);
}

void TestWindow::onGameStarted(int gameTime)
{
    // Move to map screen
    ui->tabWidget->setCurrentIndex(1);

    int gameLength = gameTime * 60 * 1000; // hostDialog.gameTimeValue * 60 * 1000;
    mapScene->runGameStartOverlay();

    QTime n(0,0,0); // Create initial QTime object
    t = n.addSecs(gameLength/1000); // Set new object to Game time in seconds
    clockText = t.toString("mm:ss");    // Convert time to text
    ui->displayTime->setText(clockText);    // Display time in label

    displayClock = new QTimer;  // new QTimer
    displayClock->setInterval(1000);    // timeout every second
    connect(displayClock, SIGNAL(timeout()), this, SLOT(updateDisplayClock())); // Connect timeout to update function for the label
    displayClock->start();  // Start the timer

    runClock->setInterval(gameLength);
    runClock->start(gameLength);

    resizeMap();
}

void TestWindow::onModeChanged(NetworkWidget::Mode newValue)
{
    bool hosting = newValue == NetworkWidget::Mode::Host;
    configDialog.setClearTableButtonEnabled(hosting);
}

void TestWindow::sendPlayerUpdate()
{
    auto hostStatus = ui->newNetworkWidget->mode();

    switch (hostStatus)
    {
    case NetworkWidget::Mode::Host:
    {
        if (!ui->newNetworkWidget->host()->hasGameStarted())
        {
            return;
        }

        for (PlayerItem* player : mapScene->players())
        {
            ui->newNetworkWidget->host()->sendHealthUpdate(player->color(), player->health(), player->hasCrown());
        }

        PlayerItem* myPlayer = mapScene->myPlayer();
        ui->newNetworkWidget->host()->sendPositionUpdate(myPlayer->pos());
        break;
    }
    case NetworkWidget::Mode::Client:
    {
        if (!ui->newNetworkWidget->client()->hasGameStarted())
        {
            return;
        }

        PlayerItem* player = mapScene->myPlayer();
        ui->newNetworkWidget->client()->sendPositionUpdate(player->pos());
        break;
    }
    case NetworkWidget::Mode::None:
        break;
    }
}

void TestWindow::sendShotBulletUpdate(PlayerColor color, QPointF source, qreal angle)
{
    if (color != mapScene->myPlayer()->color())
    {
        return;
    }

    auto hostStatus = ui->newNetworkWidget->mode();

    switch (hostStatus)
    {
    case NetworkWidget::Mode::Host:
    {
        ui->newNetworkWidget->host()->sendBulletUpdate(source, angle);
        break;
    }
    case NetworkWidget::Mode::Client:
    {
        ui->newNetworkWidget->client()->sendBulletUpdate(source, angle);
        break;
    }
    case NetworkWidget::Mode::None:
        break;
    }
}

void TestWindow::createHelpDialog()
{
//    HelpDialog helpDialog(this);
//    helpDialog.exec();
    ui->stackedWidget->setCurrentIndex(4);
}

void TestWindow::createConfigDialog()
{
//    ConfigDialog configDialog(this);
//    configDialog.exec();
    ui->stackedWidget->setCurrentIndex(2);
    configDialog.refreshTable();
}

void TestWindow::createRanksDialog()
{
//    RanksDialog *ranking = new RanksDialog(this);
//    ranking->exec();
    ui->stackedWidget->setCurrentIndex(5);
}

void TestWindow::createGameConfigDialog()
{
//    HostConfigDialog hostConfigs(this);
//    hostConfigs.exec();
    ui->stackedWidget->setCurrentIndex(3);
}

void TestWindow::on_devButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    this->resizeMap();
}

void TestWindow::on_backButton_clicked()
{
    //ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(2);
}

void TestWindow::moveHome()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void TestWindow::moveNext()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->tabWidget->setCurrentIndex(0);
}

void TestWindow::labelUpdate1()
{
    qDebug() << "label update 1 triggered";
    playerName = configDialog.username();
    ui->usernameLabel->setText("Username: " +playerName);
    changeNameDialog.setName(playerName);
}

void TestWindow::labelUpdate2()
{
    qDebug() << "label upate 2 triggered";
    playerName = changeNameDialog.newUsername;
    ui->usernameLabel->setText("Username: " +playerName);
    changeNameDialog.setName(playerName);
}

void TestWindow::updateUsername()
{
    ui->stackedWidget->setCurrentIndex(6);
}

void TestWindow::usernameReset()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void TestWindow::checkTime()
{
    // Should send winning player
    emit gameEnded();

    mapScene->mapSetup();

    // Stop timer and open ranks
    runClock->stop();
    displayClock->stop();
    ui->stackedWidget->setCurrentIndex(5);
}

void TestWindow::onLeftGame()
{
    // Stop timer and open ranks
    runClock->stop();
    displayClock->stop();
}

void TestWindow::on_startGameButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

