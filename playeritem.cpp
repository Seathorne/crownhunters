#include "playeritem.h"


PlayerItem::PlayerItem(PlayerColor color)
    : QGraphicsEllipseItem ()
{
    // set player pen and brush
    this->setColor(color);

    // set player relative to scene
    qreal x = 0;
    qreal y = 0;
    setRect(x, y, width, height);
    this->mapToScene(0,0);

    // add healthbar
    myHealthbar = new HealthbarItem();
}

void PlayerItem::reset()
{
    this->setPos(respawnPoint);  // reset position of each player to their spawn point
    this->setHealth(maxHealth);
    this->setHasCrown(false);   // remove the crown from everyone

    leftKeyPressed = false;
    leftKeyReleased = false;

    rightKeyPressed = false;
    rightKeyReleased = false;

    upKeyPressed = false;
    upKeyReleased = false;

    downKeyPressed = false;
    downKeyReleased = false;

    allowedToMove = true;
    allowedToShoot = true;

    velocityX = 0.0;
    velocityY = 0.0;

    _bullets.clear();
}

void PlayerItem::setColor(PlayerColor value)
{
    _color = value;

    // This function sets the proper color graphic depending
    //  on whether the player has the crown or not
    setHasCrown(_hasCrown);
}

void PlayerItem::keyPressEvent(QKeyEvent *event)
{
    // Move Left pressed
    if ( event && (event->key() == Qt::Key_Left || event->key() == Qt::Key_A)) {
        leftKeyPressed = true;
        leftKeyReleased = false;
    }
    // Move Right pressed
    if (event && (event->key() == Qt::Key_Right || event->key() == Qt::Key_D)) {
        rightKeyPressed = true;
        rightKeyReleased = false;
    }
    // Move Up pressed
    if (event && (event->key() == Qt::Key_Up || event->key() == Qt::Key_W)) {
        upKeyPressed = true;
        upKeyReleased = false;
    }
    // Move Down pressed
    if (event && (event->key() == Qt::Key_Down || event->key() == Qt::Key_S)) {
        downKeyPressed = true;
        downKeyReleased = false;
    }
    QGraphicsItem::keyPressEvent(event);
}

void PlayerItem::keyReleaseEvent(QKeyEvent *event)
{
    // Move Left released
    if ( event && (event->key() == Qt::Key_Left || event->key() == Qt::Key_A)) {
        leftKeyReleased = true;
        leftKeyPressed = false;
    }
    // Move Right released
    if (event && (event->key() == Qt::Key_Right || event->key() == Qt::Key_D)) {
        rightKeyReleased = true;
        rightKeyPressed = false;
    }
    // Move Up released
    if (event && (event->key() == Qt::Key_Up || event->key() == Qt::Key_W)) {
        upKeyReleased = true;
        upKeyPressed = false;
    }
    // Move Down released
    if (event && (event->key() == Qt::Key_Down || event->key() == Qt::Key_S)) {
        downKeyReleased = true;
        downKeyPressed = false;
    }
    QGraphicsItem::keyReleaseEvent(event);
}

void PlayerItem::shoot(QPointF attackDestination)
{
    if (allowedToShoot)
    {
        // create a line between player and where mouse was clicked
        QLineF ln(QPointF(this->x() + width/qreal(2), this->y() + height/qreal(2)), attackDestination);
        // find angle of line, multiply by -1 because Qt does angles CW instead of CCW
        qreal angle = ln.angle() * qreal(-1);

        this->shoot(QPointF(scenePos().x() + width/qreal(2), scenePos().y() + height/qreal(2)), angle);
    }
}

void PlayerItem::shoot(QPointF source, qreal angle, bool fromNetwork)
{
    // create a bullet
    BulletItem* bullet = new BulletItem(this);
    // set bullet start position to middle of player
    bullet->setPos(source);
    bullet->setRotation(angle); // rotate the bullet to match angle that it's fired

    connect(bullet, &BulletItem::collided,
        [=]
        {
            _bullets.removeOne(bullet);
        });
    _bullets.append(bullet);
    scene()->addItem(bullet); // add bullet to the scene

    if (!fromNetwork)
    {
        emit shotBullet(_color, source, angle);
    }
}

//used for when the player kills the crown holder or for the first touch
//on the crown sprite when the game starts;
// if player loses the crown, this sets the player image back to their plain
// color with no crown
void PlayerItem::setHasCrown(bool value)
{
    _hasCrown = value;

    //based on colornum we set the player to its new sprite.
    switch (_color)
    {
    case PlayerColor::Red:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-red.png" : ":/images/player-red.png"));
        break;
    case PlayerColor::Blue:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-blue.png" : ":/images/player-blue.png"));
        break;
    case PlayerColor::Green:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-green.png" : ":/images/player-green.png"));
        break;
    case PlayerColor::Magenta:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-magenta.png" : ":/images/player-magenta.png"));
        break;
    case PlayerColor::White:
        this->setPen(QPen(Qt::white, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-white.png" : ":/images/player-white.png"));
        break;
    case PlayerColor::Black:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-black.png" : ":/images/player-black.png"));
        break;
    case PlayerColor::Cyan:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-cyan.png" : ":/images/player-cyan.png"));
        break;
    case PlayerColor::Gray:
        this->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        playerBrush.setTextureImage(QImage(value ? ":/images/playerwithcrown-gray.png" : ":/images/player-gray.png"));
        break;
    }
    this->setBrush(playerBrush);

    emit hasCrownChanged(value);
}

void PlayerItem::advance(int phase)
{
    if (phase == 0)
        return;

/***Controls Left & Right movement***/
    // Move Right
    if (rightKeyPressed) {
        velocityX += acceleration;
        if (velocityX > max_speed) {
            velocityX = max_speed;
        }
    }
    // Move Left
    else if (leftKeyPressed) {
        velocityX -= acceleration;
        if (velocityX < -max_speed) {
            velocityX = -max_speed;
        }
    }
    // Slow down x-axis
    else if (rightKeyReleased || leftKeyReleased) {
        if (velocityX < 0) {
            velocityX += acceleration;
            if (velocityX >= 0) {
                velocityX = 0;
            }
        }
        else {
            velocityX -= acceleration;
            if (velocityX <= 0) {
                velocityX = 0;
            }
        }
    }

/***Controls Up & Down movement***/
    // Move Down
    if (downKeyPressed) {
        velocityY += acceleration;
        if (velocityY > max_speed) {
            velocityY = max_speed;
        }
    }
    // Move Up
    else if (upKeyPressed) {
        velocityY -= acceleration;
        if (velocityY < -max_speed) {
            velocityY = -max_speed;
        }
    }
    // Slow down y-axis
    else if (downKeyReleased || upKeyReleased) {
        if (velocityY < 0) {
            velocityY += acceleration;
            if (velocityY >= 0) {
                velocityY = 0;
            }
        }
        else {
            velocityY -= acceleration;
            if (velocityY <= 0) {
                velocityY = 0;
            }
        }
    }
    // set current position as "previousPos" to use in case of collision with wall or player
    previousPos = (this->scenePos());
    // set new position of player/healthbar/visible health
    myHealthbar->setPos(this->x()-12.5 + velocityX, this->y()-10 + velocityY);
    myHealthbar->visibleHealth->setPos(this->x()-12.5 + velocityX, this->y()-10 + velocityY);
    if (allowedToMove){
        setX(this->x() + velocityX);
        setY(this->y() + velocityY);
    }

    // if collision with wall, bounce off wall
    if (this->collidesWithWall()){
        // Need to check if we are moving at a diagonal
        if((velocityX != 0.0) && (velocityY != 0.0)) {
            // Check each viable direction for collision
            // Moving against a vertical surface
            setPos(this->previousPos);  // Reset pos to when we were not colliding
            setX(this->x());            // Leave X, change Y
            setY(this->y() + velocityY);

            if(!(this->collidesWithWall())) {   // Not colliding with a wall
                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                return;     // Player position has been updated accordingly
            }
            // Moving against a horizontal surface
            setPos(this->previousPos);   // Reset pos to when we were not colliding
            setX(this->x() + velocityX); // Change X, leave Y
            setY(this->y());

            if(!(this->collidesWithWall())) {   // Not colliding with a wall
                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                return;     // Player position has been updated accordingly
            }
            // If we're trying to go across a diagonal wall, reset position and try for diagonals
            setPos(this->previousPos);

            if (leftKeyPressed && upKeyPressed){
                // if moving across top wall to the left, deal with diagonal
                if (this->y() < MAP_HEIGHT/2){
                    setX(this->x() + velocityX);
                    setY(this->y() - velocityY);

                    if(!(this->collidesWithWall())) {   // Not colliding with a wall
                        myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                        myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                        return;     // Player position has been updated accordingly
                    }
                }
                else if (this->y() > MAP_HEIGHT/2){
                    // if still colliding, reset pos and deal with moving up the left wall
                    setPos(this->previousPos);

                    setX(this->x() - velocityX);
                    setY(this->y() + velocityY);

                    if(!(this->collidesWithWall())) {   // Not colliding with a wall
                        myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                        myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                        return;     // Player position has been updated accordingly
                    }
                }
            }

            if (leftKeyPressed && downKeyPressed){
                // If this case comes before the next one the wall across the top is a problem but the side wall is fine
                // If this case comes after the next one the side wall is a problem but the top wall is fine
                setX(this->previousPos.x() - velocityX);    // Set X to negated altered velocity
                setY(this->previousPos.y() + velocityY);    // Set Y to altered velocity

                if(!(this->collidesWithWall())) {   // Not colliding with a wall
                    myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    return;     // Player position has been updated accordingly
                }

                // The case below is the one refered to by the comments above
                setX(this->previousPos.x() + velocityX);    // Set X to altered velocity
                setY(this->previousPos.y() - velocityY);    // Set Y to negated altered velocity

                if(!(this->collidesWithWall())) {   // Not colliding with a wall
                    myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    return;     // Player position has been updated accordingly
                }
            }

            if ((rightKeyPressed && upKeyPressed) || (rightKeyPressed && downKeyPressed)){
                // If this case comes before the next one the wall across the top is a problem but the side wall is fine
                // If this case comes after the next one the side wall is a problem but the top wall is fine
                setX(this->previousPos.x() + velocityX);    // Set X to negated altered velocity
                setY(this->previousPos.y() - velocityY);    // Set Y to altered velocity

                if(!(this->collidesWithWall())) {   // Not colliding with a wall
                    myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    return;     // Player position has been updated accordingly
                }

                // The case below is the one refered to by the comments above
                setX(this->previousPos.x() - velocityX);    // Set X to altered velocity
                setY(this->previousPos.y() + velocityY);    // Set Y to negated altered velocity

                if(!(this->collidesWithWall())) {   // Not colliding with a wall
                    myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                    return;     // Player position has been updated accordingly
                }
            }

//            // This case doesn't really do anything other than fix a glitch where you can escape the map
//            setX(this->previousPos.x() - velocityX);    // Set X to negated altered velocity
//            setY(this->previousPos.y() - velocityY);     // Set Y to negated altered velocity

//            if(!(this->collidesWithWall())) {   // Not colliding with a wall
//                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
//                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
//                return;     // Player position has been updated accordingly
//            }

            // set new position of player/healthbar/visible health if none of the above positions work
            myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            setPos(this->previousPos);
            velocityX *= -0.75;
            velocityY *= -0.75;
        }
        else {  // not moving diagonally
            // set new position of player/healthbar/visible health
            myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            setPos(this->previousPos);
            velocityX *= -0.75;
            velocityY *= -0.75;
        }

    }
    // if collision with player, bounce off other player
        // Want to add imparting a backwards velocity on other player
    if (this->collidesWithPlayer()){
        // Need to check if we are moving at a diagonal
        if((velocityX != 0.0) && (velocityY != 0.0)) {
            // Check each viable direction for collision
            // Moving against a vertical surface
            setPos(this->previousPos);  // Reset pos to when we were not colliding
            setX(this->x());            // Leave X, change Y
            setY(this->y() + velocityY);

            if(!(this->collidesWithPlayer())) {   // Not colliding with a Player
                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                return;     // Player position has been updated accordingly
            }
            // Moving against a horizontal surface
            setPos(this->previousPos);   // Reset pos to when we were not colliding
            setX(this->x() + velocityX); // Change X, leave Y
            setY(this->y());

            if(!(this->collidesWithPlayer())) {   // Not colliding with a Player
                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                return;     // Player position has been updated accordingly
            }
            // If we're trying to go across a diagonal wall, reset position and try for diagonals
            setPos(this->previousPos);


            // If this case comes before the next one the wall across the top is a problem but the side wall is fine
            // If this case comes after the next one the side wall is a problem but the top wall is fine
            setX(this->previousPos.x() + velocityX);    // Set X to negated altered velocity
            setY(this->previousPos.y() - velocityY);    // Set Y to altered velocity

            if(!(this->collidesWithPlayer())) {   // Not colliding with a Player
                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                return;     // Player position has been updated accordingly
            }

            // The case below is the one refered to by the comments above
            setX(this->previousPos.x() - velocityX);    // Set X to altered velocity
            setY(this->previousPos.y() + velocityY);    // Set Y to negated altered velocity

            if(!(this->collidesWithPlayer())) {   // Not colliding with a Player
                myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
                return;     // Player position has been updated accordingly
            }

            // set new position of player/healthbar/visible health if none of the above positions work
            myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            setPos(this->previousPos);
            velocityX *= -0.75;
            velocityY *= -0.75;
        }
        else {  // not moving diagonally
            // set new position of player/healthbar/visible health
            myHealthbar->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            myHealthbar->visibleHealth->setPos(this->previousPos.x()-12.5, this->previousPos.y()-10);
            setPos(this->previousPos);
            velocityX *= -0.75;
            velocityY *= -0.75;
        }
    }
    // if collision with health, player regains health
    if (this->collidesWithHealth()){
        // picking up a health kit regains health to max
        for (int i = 1; i < maxHealth; i++){
            gainHealth();
        }
    }
}   // end of advance

bool PlayerItem::collidesWithWall()
{
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    foreach (QGraphicsItem * item, collidingItems){
        WallItem * wall = dynamic_cast<WallItem*>(item);
        if (wall){
            return true;
        }
    }
    return false;
}

bool PlayerItem::collidesWithPlayer()
{
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    foreach (QGraphicsItem * item, collidingItems)
    {
        // If two players collide
        if (PlayerItem* player = dynamic_cast<PlayerItem*>(item))
        {
            // If either player has the crown
            if (this->hasCrown())
            {
                // Swap the crown between the two colliding players
                this->setHasCrown(false);
                player->setHasCrown(true);
            }
            else if (player->hasCrown())
            {
                // Swap the crown between the two colliding players
                this->setHasCrown(true);
                player->setHasCrown(false);
            }
            return true;
        }
    }
    return false;
}

bool PlayerItem::collidesWithHealth()
{
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    foreach (QGraphicsItem * item, collidingItems)
    {
        HealthItem * healthKit = dynamic_cast<HealthItem*>(item);
        if (healthKit)
        {
            healthKit->scene()->removeItem(healthKit);
            return true;
        }
    }

    return false;
}

void PlayerItem::die()
{
    //qDebug() << "player has died";
    this->allowedToShoot = false; // don't allow player to shoot when dead
    this->allowedToMove = false; // don't allow player to move when dead
    this->hide(); // hide player when dead
    this->setPos(-100, -100); // set player off map when dead to avoid unwanted collisions
    this->myHealthbar->hide(); // hide healthbar when dead
    this->myHealthbar->visibleHealth->hide(); // hide visible health when dead

    this->isDead = true;
    // connect 5 second timer that will activate between player death and respawn
    deathTimer.singleShot(PLAYER_RESPAWN_TIME, this, SLOT(respawn()));
}

void PlayerItem::respawn()
{
    this->isDead = false;
    //qDebug() << "player has respawned";
    this->allowedToShoot = true; // player allowed to shoot when respawns
    this->allowedToMove = true; // player allowed to move when respawns
    this->_health = this->maxHealth; // set health to max upon respawn
    this->setPos(respawnPoint); // set player at their respawn point
    this->show(); // show the player
    this->myHealthbar->show(); // show the healthbar
    this->myHealthbar->vhWidth = this->myHealthbar->vhMaxWidth; // reset visible health to max
    this->myHealthbar->visibleHealth->show(); // show visible health

    // if on respawn, player is colliding with another player, move player out of collision
    // to prevent player freezing
    if (collidesWithPlayer()){
        this->setY(this->y() - height*2);
    }
    return;
}

void PlayerItem::reduceHealth()
{
    // set amount of pixels the healthbar goes down when taking damage
    double healthInPixels = (myHealthbar->vhMaxWidth)/(this->maxHealth);
    // if player takes damage, reduce health
    _health--;
    myHealthbar->vhWidth -= healthInPixels; // set visible health according to damage
    this->myHealthbar->visibleHealth->setRect(0,0,myHealthbar->vhWidth, myHealthbar->vhHeight);
    // if player's health reaches zero, they die
    if (_health <= 0)
    {
        die();
    }
}

void PlayerItem::gainHealth()
{
    // set amount of pixels the healthbar goes up when gaining health
    double healthInPixels = (myHealthbar->vhMaxWidth)/(this->maxHealth);
    // player gains health
    _health++;
    myHealthbar->vhWidth += healthInPixels; // set visible health according to health gain
    this->myHealthbar->visibleHealth->setRect(0,0,myHealthbar->vhWidth, myHealthbar->vhHeight);
    if (_health >= maxHealth)
    {
        _health = maxHealth;
        this->myHealthbar->vhWidth = myHealthbar->vhMaxWidth;
    }
}

void PlayerItem::setHealth(int value)
{
    while (_health < value)
    {
        gainHealth();
    }

    while (_health > value)
    {
        reduceHealth();
    }
}
