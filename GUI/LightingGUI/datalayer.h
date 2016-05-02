
#ifndef DATALAYER_H
#define DATALAYER_H

#include <QApplication>
#include <QColor>

#include "LightingProtocols.h"
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 */

/*!
 *
 * \brief The DataLayer class stores and maintains the data
 * about the state and settings of the application. It also saves
 * the settings sent to the LED hardware, such as the current brightness
 * and current lighting routine.
 *
 * \todo Make this class save its data between sessions.
 */
class DataLayer
{

public:
    /*!
     * \brief Constructor
     */
    DataLayer();
    /*!
     * \brief Deconstructor
     */
    ~DataLayer();

    /*!
     * \brief creates a color based off of the average of all colors currently being used
     * \return a QColor that represents the average of all colors up to `mColorsUsed` in `colors*`.
     */
    QColor colorsAverage();

    /*!
     * \brief the main saved color, used for single color routines.
     */
    bool mainColor(QColor newColor);
    /*!
     * \brief mainColor getter for mainColor, used for single color routines.
     * \return the mainColor, used for single color routines.
     */
    QColor mainColor();

    /*!
     * \brief arraySize size of the color array at the given index.
     * \return the size of the color array at the given index.
     */
    uint8_t arraySize(int);

    /*!
     * \brief colorArray the color array at the given index. Can be used
     *        to access the custom color array or any of the presets.
     * \return the color array at the given index.
     */
    QColor* colorArray(int);

    /*!
     * \brief customArray convenience function to access the custom color array
     * \return the custom color array.
     */
    QColor* customArray();

    /*!
     * \brief isUsingSerial true if using serial, false if using UDP
     */
    bool isUsingSerial;

    /*!
     * serial port maintence
     */
    bool setupSerial(QString serial);
    /*!
     * \brief serialPort getter for the string of the current serial port.
     * \return string representation of the current serial port.
     */
    QString serialPort();

    /*!
     * NYI
     * \brief setupUDP sets up the UDP client to talk to the LED array.
     * \param ip ip address of hte LED array
     * \param port port for connection to the LED array
     * \return true if client is set up properly, false otherwise.
     */
    bool setupUDP(QString ip, int port);
    /*!
     * NYI
     * \brief IP getter for the IP Address of the current UDP connection.
     * \return IP Address for the UDP connection.
     */
    QString IP();
    /*!
     * NYI
     * \brief UDPPort getter for the port of the current UDP connection.
     * \return port for the UDP connection.
     */
    int UDPPort();

    /*!
     * true if lights are on, false otherwise
     */
    bool isOn(bool isOn);
    /*!
     * \brief isOn getter which checks if the lights are currently on.
     * \return true if the lights are on, false otherwise.
     */
    bool isOn();

    /*!
     * mode of LEDs
     */
    bool currentMode(ELightingMode mode);
    /*!
     * \brief currentMode getter for the current ELightingMode.
     * \return the current lighting mode getting displayed on the LED array.
     */
    ELightingMode currentMode();

    /*!
     * value between 0-100 that represents how bright the LEDs shine
     */
    bool brightness(int brightness);
    /*!
     * \brief brightness getter for the current brightness.
     * \return a value between 0 and 100 that represents the current brightness.
     */
    int brightness();

    /*!
     * Time it takes the LEDs to turn off in minutes.
     */
    bool timeOut(int timeOut);
    /*!
     * \brief timeOut getter for the amount of minutes it takes for the LEDs
     *        to "time out." When this happens, they turn off, saving you
     *        electricity!
     * \return the time it'll take for the LEDs to time out.
     */
    int timeOut();

    /*!
     * The EColorPreset currently in use by the LEDs. This is used
     * for multi color routines.
     */
    bool preset(EColorPreset preset);
    /*!
     * \brief preset getter for the current color preset.
     * \return the EColorPreset that represents the colors being displayed on
     *         the LED array.
     */
    EColorPreset preset();

    /*!
     *  Time between LED updates as FPS * 100. For example,
     *  a FPS of 5 is 500.
     */
    bool speed(int speed);
    /*!
     * \brief speed getter for the speed the LED's update.
     * \return the speed the LEDs update.
     */
    int speed();

    /*!
     * number of colors in the color array
     */
    bool customColorCount(int count);
    /*!
     * \brief colorCount getter for the number of colors usd by
     *        the by the custom color routines. Will always be less
     *        than the total number of colors in the custom color array.
     * \return the number of colors used for a custom color routine.
     */
    int customColorCount();

    /*!
     * number of colors to use in the color array routines.
     * Must be less than the color count.
     */
    bool colorsUsed(int colorsUsed);
    /*!
     * \brief colorsUsed getted for the number of colors used
     * \return the number of colors used by the current color preset.
     */
    int colorsUsed();

    /*!
     * \brief resetToDefaults resets the GUI and the arduino to the default values,
     * as defined at compile time.
     */
    void resetToDefaults();

private:

    /*!
     * \brief mArraySizes the array that holds the sizes color arrays.
     */
    uint8_t mArraySizes[(int)EColorPreset::eColorPreset_MAX];

    /*!
     * \brief mColors the color arrays used for routines. This contains
     *        the custom color array and all of the presets.
     */
    QColor *mColors[(int)EColorPreset::eColorPreset_MAX];

    /*!
     * The color used for single color routines.
     */
    QColor mMainColor;

    /*!
     * \brief true if lights are on, false otherwise
     */
    bool mIsOn;

    /*!
     * \brief currentMode the mode of the LEDs
     */
    ELightingMode mCurrentMode;

    /*!
     * \brief timeOut the amount of minutes before the lights turn off. If 0, then the
     *  lights never turn off.
     */
    int mTimeOut;

    /*!
     * \brief brightness 0-100, how bright the LEDs are
     */
    int mBrightness;

    /*!
     * \brief mPreset the current preset being used for multi color routines.
     */
    EColorPreset mPreset;

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
     * \brief mColorsUsed the number of colors used for array colors routines. Must be less
     *        than colorCount.
     */
    int mColorsUsed;

    /*!
     * \brief mSpeed the current speed value of the arduino.
     */
    int mSpeed;
};

#endif // DATALAYER_H
