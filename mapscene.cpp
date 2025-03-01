#include "mapscene.h"


MapScene::MapScene(QObject* parent) :
    QGraphicsScene(parent)
{
    // health spawner timer
    healthTimer = new QTimer(this);
    connect(healthTimer, SIGNAL(timeout()), this, SLOT(healthSpawner()));
    healthTimer->start(15000);

    // create frame timer for advance()
    advanceTimer = new QTimer(this);
    connect(advanceTimer, SIGNAL(timeout()), this, SLOT(advance()));
    advanceTimer->start(10);

    // set scene rect and background
    this->setSceneRect(0,0,MAP_WIDTH,MAP_HEIGHT);
    QPixmap pm(":/images/floorTile.png");
    this->setBackgroundBrush(QBrush(pm));

    // set the pen and brush to fill in polygons
    QPen redPen(Qt::red, 4, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    QPen bluePen(Qt::blue, 4, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    QPen greenPen(Qt::green, 4, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    QBrush brush(Qt::black, Qt::SolidPattern);


    //*******************************
    //QLabel *showTime = new QLabel;
    this->addSimpleText("Something");

/******************************************************* Create Left Wall *******************************************************/
    // create left_wall and add to the scene
    QPointF left_wall[13] = {QPointF(0,0), QPointF(50,25), QPointF(50,282), QPointF(81,332), QPointF(81,434),
                             QPointF(50,484), QPointF(50,575), QPointF(0,600), QPointF(0,0)};
    WallItem *leftWall = new WallItem(left_wall, 13, redPen, brush);
    this->addItem(leftWall);
/******************************************************* Create Top Wall ********************************************************/
    // create top_wall and add to the scene
    QPointF top_wall[15] = {QPointF(0,0), QPointF(1200,0), QPointF(1150,25), QPointF(700,25), QPointF(700,125),
                            QPointF(800,125), QPointF(774,145), QPointF(680,145), QPointF(680,25), QPointF(566,25),
                            QPointF(516,63), QPointF(354,63), QPointF(304,25), QPointF(50,25), QPointF(0,0)};
    WallItem *topWall = new WallItem(top_wall, 15, redPen, brush);
    this->addItem(topWall);
/******************************************************* Create Right Wall ******************************************************/
    // create right_wall and add to the scene
    //optional points for jutting wall-->   QPointF(1150,477), QPointF(1049,477), QPointF(1069,457), QPointF(1150,457),
    QPointF right_wall[5] = {QPointF(1200,0), QPointF(1200,600), QPointF(1150,575), QPointF(1150, 25), QPointF(1200,0)};
    WallItem *rightWall = new WallItem(right_wall, 5, redPen, brush);
    this->addItem(rightWall);
/******************************************************* Create Bottom Wall *****************************************************/
    // create bottom_wall and add to the scene
    QPointF bottom_wall[11] = {QPointF(0,600), QPointF(50,575), QPointF(580,575), QPointF(580,495), QPointF(480,495),
                               QPointF(500,475), QPointF(600,475), QPointF(600,575), QPointF(1150,575), QPointF(1200,600),
                               QPointF(0,600)};
    WallItem *bottomWall = new WallItem(bottom_wall, 11, redPen, brush);
    this->addItem(bottomWall);
/******************************************************* Create Inner Wall 1 ****************************************************/
    // create inner_wall_1 and add to the scene
    QPointF inner_wall_1[7] = {QPointF(160,230), QPointF(130,260), QPointF(287,260), QPointF(287,108), QPointF(257,138),
                               QPointF(257,230), QPointF(160,230)};
    WallItem *innerWall1 = new WallItem(inner_wall_1, 7, bluePen, brush);
    this->addItem(innerWall1);
/******************************************************* Create Inner Wall 2 ****************************************************/
    // create inner_wall_2 and add to the scene
    QPointF inner_wall_2[9] = {QPointF(182,404), QPointF(182,499), QPointF(360,499), QPointF(360,404), QPointF(330, 434),
                               QPointF(330,469), QPointF(212,469), QPointF(212,434), QPointF(182,404)};
    WallItem *innerWall2 = new WallItem(inner_wall_2, 9, bluePen, brush);
    this->addItem(innerWall2);
/******************************************************* Create Inner Wall 3 ****************************************************/
    // create inner_wall_3 and add to the scene
    QPointF inner_wall_3[9] = {QPointF(744,390), QPointF(744,485), QPointF(774,455), QPointF(774,420), QPointF(892,420),
                               QPointF(892,455), QPointF(922,485), QPointF(922,390), QPointF(744,390)};
    WallItem *innerWall3 = new WallItem(inner_wall_3, 9, bluePen, brush);
    this->addItem(innerWall3);
/******************************************************* Create Inner Wall 4 ****************************************************/
    // create inner_wall_4 and add to the scene
    QPointF inner_wall_4[5] = {QPointF(989,146), QPointF(938,197), QPointF(989,248), QPointF(1040,197), QPointF(989,146)};
    WallItem *innerWall4 = new WallItem(inner_wall_4, 5, bluePen, brush);
    this->addItem(innerWall4);
/********************************************************************************************************************************/

    // test having all 8 players on screen
    createPlayers(DEFAULT_MAX_PLAYERS);

    // create and add the crown to the scene
    crown = new CrownItem;
    addItem(crown);

    // add respawn overlay
    respawnOverlay = new RespawnOverlayItem;
    //addItem(respawnOverlay);
    //respawnOverlay->rateTimer->start();

    // add game start overlay
    gameStartOverlay = new GameStartOverlayItem;

    // respawn overlay timer
    respawnTimer = new QTimer;
    connect(respawnTimer, SIGNAL(timeout()), this, SLOT(checkIfPlayerDead()));
    respawnTimer->start(16);
}

MapScene::~MapScene()
{
    delete this;
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // if mouse press detected, set player's attack destination as where the
    // mouse was pressed and pass event to player
    if (event != nullptr
        && event->button() == Qt::LeftButton)
    {
        // Shoot bullet with attack destination as cursor location at time of
        //  mouse press event.
        myPlayer()->shoot(event->scenePos());
    }

    QGraphicsScene::mousePressEvent(event);
}

void MapScene::keyPressEvent(QKeyEvent* event)
{
    // passes key press event to player
    if (event != nullptr)
    {
        myPlayer()->keyPressEvent(event);
    }
    QGraphicsScene::keyPressEvent(event);
}

void MapScene::keyReleaseEvent(QKeyEvent* event)
{
    // passes key release event to player
    if (event != nullptr)
    {
        myPlayer()->keyReleaseEvent(event);
    }
    QGraphicsScene::keyReleaseEvent(event);
}

void MapScene::setMyColor(PlayerColor color)
{
    _myPlayerColor = color;
}

void MapScene::createPlayers(int count)
{
    _players.clear();

    for (int i = 0; i < count; i++)
    {
        auto player = new PlayerItem(static_cast<PlayerColor>(i));
        _players[static_cast<PlayerColor>(i)] = player;

        player->respawnPoint = spawns[i];
        player->setPos(player->respawnPoint);

        this->addItem(player);
        this->addItem(player->myHealthbar);
        this->addItem(player->myHealthbar->visibleHealth);
    }
}

void MapScene::onPositionUpdated(PlayerColor color, QPointF position)
{
    PlayerItem* player = _players[color];
    player->setPos(position);
}

void MapScene::onBulletUpdated(PlayerColor color, QPointF source, qreal angle)
{
    PlayerItem* player = _players[color];
    player->shoot(source, angle, true);
}

void MapScene::onHealthUpdated(PlayerColor color, int health, bool hasCrown)
{
    PlayerItem* player = _players[color];
    player->setHealth(health);
    player->setHasCrown(hasCrown);
}

void MapScene::checkIfPlayerDead()
{
    static int checkNum = 1;    // Static variable to ensure that the timer only starts once
    if (myPlayer()->isDead && checkNum == 1)
    {
        checkNum = 0;   // Set static int to a value other than the one to start the timer
        respawnOverlay->ResetIndex();
        addItem(respawnOverlay);    // Add respawn overlay to the scene
        respawnOverlay->rateTimer->start(); // Start the timer
    }
    else if (myPlayer()->isDead == false)
    {  // Check if player is no longer dead
        respawnOverlay->rateTimer->stop();  // Stop the timer
        //respawnOverlay->ResetIndex();

        if (respawnOverlay->scene() == this)
        {
            this->removeItem(respawnOverlay);     // Remove timer from the scene
        }
        checkNum = 1;   // Reset static int
    }
}

void MapScene::removeHealthKit(HealthItem* item)
{
    if (item == nullptr)
    {
        return;
    }

    if (item->scene() == this)
    {
        removeItem(item);
    }

    delete item;
}

void MapScene::mapSetup()
{
    // health spawner timer
    healthTimer->start(15000);

    // create frame timer for advance()
    advanceTimer->start(10);

    // set scene rect and background
    this->setSceneRect(0,0,MAP_WIDTH,MAP_HEIGHT);
    QPixmap pm(":/images/floorTile.png");
    this->setBackgroundBrush(QBrush(pm));

    // set the pen and brush to fill in polygons
    QPen redPen(Qt::red, 4, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    QPen bluePen(Qt::blue, 4, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    QPen greenPen(Qt::green, 4, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    QBrush brush(Qt::black, Qt::SolidPattern);


    //*******************************
    //QLabel *showTime = new QLabel;
    this->addSimpleText("Something");

/******************************************************* Create Left Wall *******************************************************/
    // create left_wall and add to the scene
    QPointF left_wall[13] = {QPointF(0,0), QPointF(50,25), QPointF(50,282), QPointF(81,332), QPointF(81,434),
                             QPointF(50,484), QPointF(50,575), QPointF(0,600), QPointF(0,0)};
    WallItem *leftWall = new WallItem(left_wall, 13, redPen, brush);
    this->addItem(leftWall);
/******************************************************* Create Top Wall ********************************************************/
    // create top_wall and add to the scene
    QPointF top_wall[15] = {QPointF(0,0), QPointF(1200,0), QPointF(1150,25), QPointF(700,25), QPointF(700,125),
                            QPointF(800,125), QPointF(774,145), QPointF(680,145), QPointF(680,25), QPointF(566,25),
                            QPointF(516,63), QPointF(354,63), QPointF(304,25), QPointF(50,25), QPointF(0,0)};
    WallItem *topWall = new WallItem(top_wall, 15, redPen, brush);
    this->addItem(topWall);
/******************************************************* Create Right Wall ******************************************************/
    // create right_wall and add to the scene
    //optional points for jutting wall-->   QPointF(1150,477), QPointF(1049,477), QPointF(1069,457), QPointF(1150,457),
    QPointF right_wall[5] = {QPointF(1200,0), QPointF(1200,600), QPointF(1150,575), QPointF(1150, 25), QPointF(1200,0)};
    WallItem *rightWall = new WallItem(right_wall, 5, redPen, brush);
    this->addItem(rightWall);
/******************************************************* Create Bottom Wall *****************************************************/
    // create bottom_wall and add to the scene
    QPointF bottom_wall[11] = {QPointF(0,600), QPointF(50,575), QPointF(580,575), QPointF(580,495), QPointF(480,495),
                               QPointF(500,475), QPointF(600,475), QPointF(600,575), QPointF(1150,575), QPointF(1200,600),
                               QPointF(0,600)};
    WallItem *bottomWall = new WallItem(bottom_wall, 11, redPen, brush);
    this->addItem(bottomWall);
/******************************************************* Create Inner Wall 1 ****************************************************/
    // create inner_wall_1 and add to the scene
    QPointF inner_wall_1[7] = {QPointF(160,230), QPointF(130,260), QPointF(287,260), QPointF(287,108), QPointF(257,138),
                               QPointF(257,230), QPointF(160,230)};
    WallItem *innerWall1 = new WallItem(inner_wall_1, 7, bluePen, brush);
    this->addItem(innerWall1);
/******************************************************* Create Inner Wall 2 ****************************************************/
    // create inner_wall_2 and add to the scene
    QPointF inner_wall_2[9] = {QPointF(182,404), QPointF(182,499), QPointF(360,499), QPointF(360,404), QPointF(330, 434),
                               QPointF(330,469), QPointF(212,469), QPointF(212,434), QPointF(182,404)};
    WallItem *innerWall2 = new WallItem(inner_wall_2, 9, bluePen, brush);
    this->addItem(innerWall2);
/******************************************************* Create Inner Wall 3 ****************************************************/
    // create inner_wall_3 and add to the scene
    QPointF inner_wall_3[9] = {QPointF(744,390), QPointF(744,485), QPointF(774,455), QPointF(774,420), QPointF(892,420),
                               QPointF(892,455), QPointF(922,485), QPointF(922,390), QPointF(744,390)};
    WallItem *innerWall3 = new WallItem(inner_wall_3, 9, bluePen, brush);
    this->addItem(innerWall3);
/******************************************************* Create Inner Wall 4 ****************************************************/
    // create inner_wall_4 and add to the scene
    QPointF inner_wall_4[5] = {QPointF(989,146), QPointF(938,197), QPointF(989,248), QPointF(1040,197), QPointF(989,146)};
    WallItem *innerWall4 = new WallItem(inner_wall_4, 5, bluePen, brush);
    this->addItem(innerWall4);
/********************************************************************************************************************************/

    // Reset player positions and health, and take away crown
    for (PlayerItem* player : _players)
    {
        player->reset();
    }

    // create and add the crown to the scene
    if (crown == nullptr)
    {
        crown->scene()->removeItem(crown);
        delete crown;
    }
    crown = new CrownItem;
    addItem(crown);

    // respawn overlay timer
    respawnTimer->start(16);
}

void MapScene::runGameStartOverlay()
{
    this->addItem(gameStartOverlay);
    this->gameStartOverlay->rateTimer->start();
}

void MapScene::healthSpawner()
{
    // static int variable to change spawn positions of health kits
    static int count = 0;

    // remove old health kits before adding new ones
    if (count > 0)
    {
        removeHealthKit(healthKit);
        removeHealthKit(healthKit2);
        removeHealthKit(healthKit3);
    }

    if (count % 2 == 0)
    {
        // create health items at position 1
        healthKit = new HealthItem(200,175);
        healthKit2 = new HealthItem(525,525);
        healthKit3 = new HealthItem(725,75);
        // add health kits to the scene after position has been set
        addItem(healthKit);
        addItem(healthKit2);
        addItem(healthKit3);
    }
    else
    {
        // create health items at position 2
        healthKit = new HealthItem(225,300);
        healthKit2 = new HealthItem(925,300);
        healthKit3 = new HealthItem(575,50);
        // add health kits to the scene after position has been set
        addItem(healthKit);
        addItem(healthKit2);
        addItem(healthKit3);
    }

    count++;
}
