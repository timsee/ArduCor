
#ifndef COMMLAYER_H
#define COMMLAYER_H

#include "datalayer.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <memory>

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The CommLayer class provides communication protocols
 *  that allow the user to connect and send packets to an LED
 *  array. Currently it supports serial communication. In a future
 *  update it will also support UDP.
 *
 * \todo Add UDP support.
 */
class CommLayer
{
public:
    CommLayer();
    ~CommLayer();

    /*!
     *  start up a serial port
     */
    bool connectSerialPort(QString serialPortName);

    /*!
     * \brief closeSerialPort cleans up after the serial port.
     */
    void closeSerialPort();
    /*!
     * \brief serialList list of possible serial ports
     * for connection
     */
    QList<QSerialPortInfo> serialList;
    /*!
     * \brief serial the serial port currently in use
     */
    std::shared_ptr<QSerialPort> serial;

    /*!
     * \brief sendMainColorChange change the main color of the lighting settings
     *        in the GUI, this is the color displayed in the leftmost menu.
     * \param r a value between 0-255
     * \param g a value between 0-255
     * \param b a value between 0-255
     */
    void sendMainColorChange(int r, int g, int b);

    /*!
     * \brief sendColorChange change an array color in the lighting system
     * \param color the index of the array color
     * \param r a value between 0-255
     * \param g a value between 0-255
     * \param b a value between 0-255
     */
    void sendArrayColorChange(int color, int r, int g, int b);

    /*!
     * \brief sendModeChange change the mode of the lights. The mode changes
     *        how the lights look. some modes are a single color, some modes are random colors
     *        and some use a saved array.
     * \param mode the mode being sent to the LED system
     */
    void sendModeChange(ELightingMode mode);

    /*!
     * \brief sendArrayModeChange similar to sendModeChange, but this command works specifically
     *        with array modes. It has an added argument that controls how many colors are used when
     *        displaying the array mode.
     * \param mode the array mode being sent to the LED system
     * \param count the number of colors from the array it will use.
     */
    void sendArrayModeChange(ELightingMode mode, int count);

    /*!
     * \brief sendBrightness sends a brightness value between 0 and 100, with 100 being full brightness.
     * \param brightness a value between 0 and 100
     */
    void sendBrightness(int brightness);

    /*!
     * \brief sendSpeed sends a desired FPS for light updates. This value is the FPS * 100,
     * for example if you want a FPS of 5, send the value 500.
     * \param speed the FPS multiplied by 100.
     */
    void sendSpeed(int speed);

    /*!
     * \brief sendTimeOut the amount of minutes that it takes for the LEDs to turn themselves off from
     *        inactivity. Perfect for bedtime!
     * \param timeOut a number greater than 0
     */
    void sendTimeOut(int timeOut);
};

#endif // COMMLAYER_H
