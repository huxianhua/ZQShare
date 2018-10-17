#ifndef SLIDERPICTURE_H
#define SLIDERPICTURE_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMouseEvent>
#include <QPainter>
#include <QLabel>
#include <QHBoxLayout>
#include <QVector>
#include <QDebug>
/************SliderPicture**************/
class PushButton;
class PixLabel;
class SliderPicture : public QWidget
{
    Q_OBJECT

public:
    explicit SliderPicture(QWidget *parent = 0);
    ~SliderPicture();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    void moveCurrentPage(bool);
    void setLabelMove(bool);

private:
    PushButton *close_button;
    QLabel *background_label;
    QPoint m_mouseSrcPos;
    QPoint m_mouseDstPos;
    bool mouse_press;
    bool mouse_move;
    bool label_move;
    int current_index;
    QLabel *total_label;
    QVector<PixLabel*>label_array;

    int current_pos_x;

private slots:
    void changeCurrentPage(PixLabel *label);
};

/************PushButton**************/
class PushButton : public QPushButton
{
    Q_OBJECT

public:

    explicit PushButton(QWidget *parent = 0);
    ~PushButton();
    void setPicName(QString pic_name);

protected:

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

private:
    enum ButtonStatus{NORMAL, ENTER, PRESS, NOSTATUS};
    ButtonStatus status;
    QString pic_name;

    int btn_width;
    int btn_height;
    bool mouse_press;
};
/************CLabel**************/
class PixLabel : public QWidget
{
    Q_OBJECT
public:
    explicit PixLabel(QWidget *parent = 0);
    ~PixLabel();

    void setPixmap(const QPixmap &);
    void setText(const QString &);
    void setMouseEnterFlag(bool);
    void setMousePressFlag(bool);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

signals:
    void signalLabelPress(PixLabel *);

private:
    void initVariable();
    void initSetupUi();
    void createFrame();
    void createWidget();
    void createLayout();
    void paintWidget(int, QPainter *);
    bool getMouseEnterFlag();
    bool getMousePressFlag();

    bool m_mouseEnterFlag;
    bool m_mousePressFlag;

    QHBoxLayout *m_pHLayout;
    QLabel *m_pLabelIcon;
    QLabel *m_pLabelText;
};
#endif // SLIDERPICTURE_H
