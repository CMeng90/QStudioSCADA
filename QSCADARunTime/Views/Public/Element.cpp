﻿#include "Element.h"
#include <QDebug>

Element::Element(QObject *parent)
    : QObject(parent),
      bShow_(true),
      bEnable_(true) {

}


Element::~Element() {

}

void Element::setElementId(const QString &id) {
    elementId = id;
}

QString Element::getElementId() const {
    return elementId;
}

QString Element::getInternalElementType() const {

    return internalElementType;
}

void Element::setElementWidth(int width) {
    elementWidth = width;
}

int Element::getElementWidth() const {
    return elementWidth;
}

void Element::setElementHeight(int height) {
    elementHeight = height;
}

int Element::getElementHeight() const {
    return elementHeight;
}

void Element::setElementXPos(int xpos) {
    elementXPos = xpos;
}

int Element::getElementXPos() const {
    return elementXPos;
}

void Element::setElementYPos(int ypos) {
    elementYPos = ypos;
}

int Element::getElementYPos() const {
    return elementYPos;
}

void Element::setElementZValue(int zval) {
    elementZValue = zval;
    //setZValue(elementZValue);
}

int Element::getElementZValue() const {
    return elementZValue;
}

void Element::moveTo(int x, int y) {
    elementXPos = elementXPos + x;
    elementYPos = elementYPos + y;
}

void Element::setAngle(qreal angle_) {

    QTransform transform;
    transform.rotate(angle_);
    elemAngle = angle_;
    //setTransform(transform);
}

qreal Element::angle() const {
    return elemAngle;
}

void Element::init() {
/*
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);
*/

    elementWidth = 70;
    elementHeight = 70;
    elementZValue = 0;
    backgroundColor = QColor(Qt::blue);
    signBackgroundColor = QColor(Qt::black);
    borderWidth = 2;
    borderColor = QColor(Qt::black);
    elementText = tr("文本");
    fontSize = 10;
    elemAngle = 0;
}


/**
 * @brief Element::setProjectPath
 * @details 设置工程路径
 * @param path 工程路径
 */
void Element::setProjectPath(const QString &path) {
    if(strProjectPath_ != path)
        strProjectPath_ = path;
}


/**
 * @brief Element::getProjectPath
 * @details 获取工程路径
 * @return 工程路径
 */
QString Element::getProjectPath() const {
    return strProjectPath_;
}


/**
 * @brief Element::getHAlignString
 * @param szAlign 对齐方式
 * @return 水平方向对齐方式
 */
QString Element::getHAlignString(const QString& szAlign) const {
    if(szAlign == tr("左对齐")) {
        return QString("left");
    } else if(szAlign == tr("居中对齐")) {
        return QString("center");
    } else if(szAlign == tr("右对齐")) {
        return QString("right");
    }
    return QString("");
}


/**
 * @brief Element::setHAlignString
 * @details 设置水平方向对齐方式
 * @param szAlign 水平方向对齐方式
 * @param szAlignSet 待设置垂直方向对齐方式
 */
void Element::setHAlignString(const QString& szAlign, QString& szAlignSet) {
    if(szAlign == QString("left")) {
        szAlignSet = tr("左对齐");
    } else if(szAlign == QString("center")) {
        szAlignSet = tr("居中对齐");
    } else if(szAlign == QString("right")) {
        szAlignSet = tr("右对齐");
    }
}


/**
 * @brief Element::getVAlignString
 * @param szAlign 对齐方式
 * @return 垂直方向对齐方式
 */
QString Element::getVAlignString(const QString& szAlign) const {
    if(szAlign == tr("上对齐")) {
        return QString("top");
    } else if(szAlign == tr("居中对齐")) {
        return QString("center");
    } else if(szAlign == tr("下对齐")) {
        return QString("bottom");
    }
    return QString("");
}

/**
 * @brief Element::setVAlignString
 * @details 设置垂直方向对齐方式
 * @param szAlign 垂直方向对齐方式
 * @param szAlignSet 待设置垂直方向对齐方式
 */
void Element::setVAlignString(const QString& szAlign, QString& szAlignSet) {
    if(szAlign == QString("top")) {
        szAlignSet = tr("上对齐");
    } else if(szAlign == QString("center")) {
        szAlignSet = tr("居中对齐");
    } else if(szAlign == QString("bottom")) {
        szAlignSet = tr("下对齐");
    }
}


/**
 * @brief Element::setOwnerWidget
 * @details 设置元素所属窗口
 * @param owner 所属窗口
 */
void Element::setOwnerWidget(QWidget *owner) {
    this->ownerWidget_ = owner;
}


/**
 * @brief Element::getOwnerWidget
 * @details 获取元素所属窗口
 * @return 所属窗口
 */
QWidget *Element::getOwnerWidget() {
    return this->ownerWidget_;
}


/**
 * @brief Element::hideElement
 * @details 隐藏控件
 */
void Element::hideElement() {
    bShow_ = false;
}


/**
 * @brief Element::showElement
 * @details 显示控件
 */
void Element::showElement() {
    bShow_ = true;
}

/**
 * @brief Element::enableElement
 * @details 生效控件
 */
void Element::enableElement() {
    bEnable_ = true;
}


/**
 * @brief Element::disableElement
 * @details 失效控件
 */
void Element::disableElement() {
    bEnable_ = false;
}


/**
 * @brief Element::startBlinkElement
 * @details 闪烁控件
 */
void Element::startBlinkElement() {
    connect(&blinkTimer_, SIGNAL(timeout()), this, SLOT(blinkTimeOut()));
    blinkTimer_.start(500);
}


/**
 * @brief Element::stopBlinkElement
 * @details 停止闪烁控件
 */
void Element::stopBlinkElement() {
    disconnect(&blinkTimer_, SIGNAL(timeout()), this, SLOT(blinkTimeOut()));
    blinkTimer_.stop();
}

void Element::blinkTimeOut() {
    bShow_ = !bShow_;
}
