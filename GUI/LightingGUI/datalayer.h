
#ifndef DATALAYER_H
#define DATALAYER_H

#include <QApplication>
#include <QColor>

/*!
 * \brief The ELightingMode enum Lighting modes in the arduino library.
 */
enum class ELightingMode {
    eLightingModeOff,
    eLightingModeSingleConstant,
    eLightingModeSingleBlink,
    eLightingModeSingleFade,
    eLightingModeSingleGlimmer,
    eLightingModeSingleRed,
    eLightingModeSingleOrange,
    eLightingModeSingleYellow,
    eLightingModeSingleGreen,
    eLightingModeSingleTeal,
    eLightingModeSingleBlue,
    eLightingModeSinglePurple,
    eLightingModeSingleLightBlue,
    eLightingModeSinglePink,
    eLightingModeSingleWhite,
    eLightingModeMultiRandomIndividual,
    eLightingModeMultiRandomSolid,
    eLightingModeMultiFade,
    eLightingModeSavedGlimmer,
    eLightingModeSavedRandomIndividual,
    eLightingModeSavedRandomSolid,
    eLightingModeSavedFade,
    eLightingModeSavedBarsSolid,
    eLightingModeSavedBarsMoving
};

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The DataLayer class contains all the saved data
 * about the state and settings of the LEDs array. It saves
 * things such as the current brightness and the current mode.
 *
 * \todo Make this class save its data between sessions.
 * \todo Remove the Color struct and use QColor instead.
 */
class DataLayer
{

public:
    DataLayer();
    ~DataLayer();

    /*!
     * \brief colors all saved colors sent to the arduino.
     */
    std::vector<QColor> colors;

    /*!
     * \brief the main saved color, used for single color routines.
     */
    bool color(QColor newColor);
    QColor color();

    /*!
     * \brief isUsingSerial true if using serial, false if using UDP
     */
    bool isUsingSerial;

    /*!
     * serial port maintence
     */
    bool setupSerial(QString serial);
    QString serialPort();

    // NYI: UDP
    bool setupUDP(QString ip, int port);
    QString IP();
    int UDPPort();

    /*!
     * true if lights are on, false otherwise
     */
    bool isOn(bool isOn);
    bool isOn();

    /*!
     * mode of LEDs
     */
    bool currentMode(ELightingMode mode);
    ELightingMode currentMode();

    /*!
     * value between 0-100 that represents how bright the LEDs shine
     */
    bool brightness(int brightness);
    int brightness();

    /*!
     * Time it takes the LEDs to turn off in minutes.
     */
    bool timeOut(int timeOut);
    int timeOut();

    /*!
     *  Time between LED updates as FPS * 100. For example,
     *  a FPS of 5 is 500.
     */
    bool speed(int speed);
    int speed();

    /*!
     * number of colors in the color array
     */
    bool colorCount(int count);
    int colorCount();

    /*!
     * \brief resetToDefaults resets the GUI and the arduino to the default values, as defined at compile time.
     */
    void resetToDefaults();

private:
    QColor mColor;

    /*!
     * \brief true if lights are on, false otherwise
     */
    bool mIsOn;
    /*!
     * \brief currentMode the mode of the LEDs
     */
    ELightingMode mCurrentMode;

    /*!
     * \brief timeOut the amount of minutes before the lights turn off. If 0, then the   lights never turn off.
     */
    int mTimeOut;

    /*!
     * \brief brightness 0-100, how bright the LEDs are
     */
    int mBrightness;

    /*!
     * \brief serialPort the name of the serial port, used only if isUsingSerial is true.
     */
    QString mSerialPort;

    /*!
     * \brief ipAddress the IP address, only used if isUsingSerial is false.
     */
    QString mIpAddress;

    /*!
     * \brief port the port for the UDP connection, only used if isUSingSerial is false.
     */
    int mUDPPort;

    /*!
     * \brief mColorCount the number of colors saved in the color array.
     */
    int mColorCount;

    /*!
     * \brief mSpeed the current speed value of the arduino.
     */
    int mSpeed;
};

#endif // DATALAYER_H
