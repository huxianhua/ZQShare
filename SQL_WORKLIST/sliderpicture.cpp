#include "sliderpicture.h"

SliderPicture::SliderPicture(QWidget *parent)
    : QWidget(parent)
    , mouse_press(false)
    , mouse_move(false)
    , label_move(true)
    , current_index(0)
    , current_pos_x(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
//    resize(643, 592);
    resize(643, 630);

    background_label = new QLabel(this);
    background_label->setPixmap(QPixmap(":/images/background"));
    background_label->setGeometry(QRect(0, 0, this->width(), this->height()));

    QPixmap pixmap(QSize(this->width()*3, this->height()-38));
    QPainter painter(&pixmap);
    for(int i = 0; i < 3; i++){
        painter.drawImage(QRect(this->width()*i, 0, this->width(), this->height()-38),
                          QImage(QString(":/images/sql_%1").arg(i)));
    }
    total_label = new QLabel(this);
    total_label->resize(pixmap.size());
    total_label->setPixmap(pixmap);
    total_label->move(0, 0);

    for(int index = 0; index < 3; index++) {
        PixLabel *label = new PixLabel(this);
        label->resize(QSize(155, 45));
//        label->setPixmap(QPixmap(QString(":/btn_%1").arg(index)));
        if (index == 0) {
            label->setText(tr("oracle"));
        }
        else if (index == 1) {
            label->setText(tr("sqlServer"));
        }
        else{
            label->setText(tr("mySql"));
        }

        label->move(index*230, 590);
        connect(label, SIGNAL(signalLabelPress(PixLabel*)), this, SLOT(changeCurrentPage(PixLabel*)));
        label_array.append(label);
    }
    label_array.first()->setMousePressFlag(true);
    label_array.first()->setFocus();

    close_button = new PushButton(this);
    close_button->setPicName(":/images/close");
    close_button->move(this->width()-close_button->width()-10, 5);
    close_button->setToolTip(tr("close"));
    connect(close_button, SIGNAL(clicked()), this, SLOT(close()));
}

SliderPicture::~SliderPicture()
{

}

void SliderPicture::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_mouseSrcPos = event->pos();
        if(m_mouseSrcPos.y() <= 40) {
            mouse_move = true;
        }
        else {
            current_pos_x = total_label->x();
            mouse_press = true;
        }
    }
    else if(event->button() == Qt::RightButton) {
        if(label_move) {
            if(current_index > 0) {
                current_index--;
                moveCurrentPage(false);  //move right
            }
        }
    }
}

void SliderPicture::mouseReleaseEvent(QMouseEvent *event)
{
    int xpos = 0;
    if (mouse_press) {
        if (label_move) {
            m_mouseDstPos = event->pos();
            xpos = m_mouseDstPos.x() - m_mouseSrcPos.x();
            if (xpos > 0) {                     //the plan is:move right
                if(xpos >= 150) {               //mouse gap
                    if(current_index > 0) {     //move right
                        current_index--;
                        moveCurrentPage(false);
                    }
                    else {
                        moveCurrentPage(true);  //move left
                    }
                }
                else {
                     moveCurrentPage(true);     //move left
                }
            }
            else {             //the plan is:move right
                if(xpos <= -150)
                {
                    if(current_index < 3-1) {
                        current_index++;
                        moveCurrentPage(true);  //move left
                    }
                    else {
                        moveCurrentPage(false); //move right
                    }
                }
                else {
                    moveCurrentPage(false);     //move right
                }
            }
        }
        mouse_press = false;
    }
    else if(mouse_move){
        mouse_move = false;
    }
}

void SliderPicture::mouseMoveEvent(QMouseEvent *event)
{
    int xPos = 0;
    if(mouse_press) {
        if(label_move) {
            m_mouseDstPos = event->pos();
            xPos = m_mouseDstPos.x() - m_mouseSrcPos.x();

            setLabelMove(false);
            total_label->move(current_pos_x + xPos, 0);
            setLabelMove(true);
        }
    }
    else if(mouse_move) {
        m_mouseDstPos = event->pos();
        this->move(this->pos() + m_mouseDstPos - m_mouseSrcPos);
    }
}

void SliderPicture::keyPressEvent(QKeyEvent *event)
{
    if(label_move) {
        switch(event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Up:
            if(current_index > 0) {
                current_index--;
                moveCurrentPage(false);    //move right
            }
            break;
        case Qt::Key_Right:
        case Qt::Key_Down:
            if(current_index < 3-1) {
                current_index++;
                moveCurrentPage(true);     //move left
            }
            break;
        default:
            break;
        }
    }
}

void SliderPicture::moveCurrentPage(bool direction)
{
    //change all label
    for(int i=0; i<3; i++) {
        if(i != current_index) {
            label_array.at(i)->setMousePressFlag(false);
        }
        else{
            label_array.at(i)->setMousePressFlag(true);
        }
    }
    //split point of picture
    //0-680 680-1360 1360-2040 2040-2720
    //true:left     false:right

    //index=0, move -680*0
    //index=1, move -680*1
    //index=2, move-680*2
    //index=3, move-680*3
    setLabelMove(false);
    int current_pos_x = total_label->x();    //label position
    int dest_pos_x = -643 * current_index;
    if(direction) {
        while(current_pos_x > dest_pos_x) {
            total_label->move(current_pos_x-20, 0);
            current_pos_x = total_label->x();
            qApp->processEvents(QEventLoop::AllEvents);
        }
    }
    else {
        while(current_pos_x < dest_pos_x) {
            total_label->move(current_pos_x+20, 0);
            current_pos_x = total_label->x();
            qApp->processEvents(QEventLoop::AllEvents);
        }
    }
    total_label->move(dest_pos_x, 0);
    setLabelMove(true);
}

void SliderPicture::setLabelMove(bool enable)
{
    label_move = enable;
}

void SliderPicture::changeCurrentPage(PixLabel *label)
{
    int index = 0;
    for(int iter = 0; iter < 3; iter++) {
        if(label != label_array.at(iter)) {
            label_array.at(iter)->setMousePressFlag(false);
        }
        else {
            index = iter;                //get the clicked label of index
        }
    }
    if(index < current_index) {          //move left
        while(index != current_index) {
            current_index--;
            moveCurrentPage(false);
        }
    }
    else if(index > current_index) {     //move right
        while(index != current_index) {
            current_index++;
            moveCurrentPage(true);
        }
    }

}

/************PushButton**************/
PushButton::PushButton(QWidget *parent)
    : QPushButton(parent)
{
    status = NORMAL;
    mouse_press = false;
}

PushButton::~PushButton()
{

}

void PushButton::setPicName(QString pic_name)
{
    this->pic_name = pic_name;
    setFixedSize(QPixmap(pic_name).size());
}

void PushButton::enterEvent(QEvent *)
{
    status = ENTER;
    update();
}

void PushButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        mouse_press = true;
        status = PRESS;
        update();
    }
}

void PushButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(mouse_press  && this->rect().contains(event->pos())) {
        mouse_press = false;
        status = ENTER;
        update();
        emit clicked();
    }
}

void PushButton::leaveEvent(QEvent *)
{
    status = NORMAL;
    update();
}

void PushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap;
    switch(status) {
        case NORMAL: {
            pixmap.load(pic_name);
            break;
        }
        case ENTER: {
            pixmap.load(pic_name + QString("_hover"));
            break;
        }
        case PRESS: {
            pixmap.load(pic_name + QString("_pressed"));
            break;
        }
        case NOSTATUS: {
            pixmap.load(pic_name);
            break;
        }
        default:
            pixmap.load(pic_name);
    }
    painter.drawPixmap(rect(), pixmap);
}
/************PixLabel**************/
PixLabel::PixLabel(QWidget *parent)
    : QWidget(parent)
{
    initVariable();
    initSetupUi();
}

PixLabel::~PixLabel()
{
    delete m_pLabelIcon;
    delete m_pLabelText;
    delete m_pHLayout;
}

void PixLabel::setPixmap(const QPixmap &pixmap)
{
    m_pLabelIcon->setPixmap(pixmap.scaled(QSize(30, 30), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PixLabel::setText(const QString &text)
{
    m_pLabelText->setText(text);
}

void PixLabel::setMouseEnterFlag(bool flag)
{
    m_mouseEnterFlag = flag;
    this->update();
}

void PixLabel::setMousePressFlag(bool flag)
{
    m_mousePressFlag = flag;
    this->update();
}

void PixLabel::enterEvent(QEvent *)
{
    if(!getMousePressFlag()) {
        setMouseEnterFlag(true);
    }
    this->setCursor(Qt::PointingHandCursor);
}

void PixLabel::leaveEvent(QEvent *)
{
    setMouseEnterFlag(false);
}

void PixLabel::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        setMousePressFlag(true);
        emit signalLabelPress(this);
    }
}

void PixLabel::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    if(getMouseEnterFlag()) {
        paintWidget(50, &painter);
    }
    else if(getMousePressFlag()) {
        paintWidget(80, &painter);
    }
    QWidget::paintEvent(e);
}

void PixLabel::initVariable()
{
    setMouseEnterFlag(false);
    setMousePressFlag(false);
}

void PixLabel::initSetupUi()
{
    createFrame();
    createWidget();
    createLayout();
}

void PixLabel::createFrame()
{
    this->setStyleSheet("QWidget{background:transparent;border:0px;color:red;font-weight:bold;font-size:16px;}");
}

void PixLabel::createWidget()
{
    m_pLabelIcon = new QLabel(this);
    m_pLabelText = new QLabel(this);
}

void PixLabel::createLayout()
{
    m_pHLayout = new QHBoxLayout;
    m_pHLayout->setSpacing(10);
    m_pHLayout->setContentsMargins(QMargins(5, 0, 5, 0));
    m_pHLayout->addWidget(m_pLabelIcon);
    m_pHLayout->addWidget(m_pLabelText);
    m_pHLayout->addStretch();

    this->setLayout(m_pHLayout);
}

void PixLabel::paintWidget(int transparency, QPainter *device)
{
    QPen pen(Qt::NoBrush, 1);
    device->setPen(pen);
    QLinearGradient linear(this->rect().topLeft(), this->rect().bottomLeft());
    linear.setColorAt(0, QColor(255, 0, 0, transparency));

    QBrush brush(linear);
    device->setBrush(brush);
    device->drawRoundedRect(this->rect(), 2, 2);
}

inline bool PixLabel::getMouseEnterFlag()
{
    return m_mouseEnterFlag;
}

inline bool PixLabel::getMousePressFlag()
{
    return m_mousePressFlag;
}
