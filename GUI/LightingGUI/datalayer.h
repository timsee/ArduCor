/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef DATALAYER_H
#define DATALAYER_H

#include <QApplication>

/*!
 * \brief The DataLayer class contains all the saved data
 * about the states of the LEDs system. This is including but
 * not limited to its mode and its brightness.
 *
 * TODO: make this class save its data between sessions
 */
class DataLayer
{

public:
    DataLayer();
    ~DataLayer();

    enum ELightingMode {
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

    //TODO: remove this in favor of QColor
    struct Color {
      uint8_t r;
      uint8_t g;
      uint8_t b;
    };

    /*!
     * \brief colors all saved colors sent to the arduino.
     */
    Color* colors;

    /*!
     * \brief the main saved color, used for single color routines.
     */
    Color mainColor;

    /*!
     * \brief isUsingSerial true if using serial, false if using UDP
     */
    bool isUsingSerial;

    /*!
     * serial port maintence
     */
    bool setupSerial(QString serial);
    QString getSerialPort();

    // NYI: UDP
    bool setupUDP(QString ip, int port);
    QString getIP();
    int getUDPPort();

    /*!
     * mode of LEDs
     */
    bool setCurrentMode(ELightingMode mode);
    ELightingMode getCurrentMode();

    /*!
     * value between 0-100 that represents how bright the LEDs shine
     */
    bool setBrightness(int brightness);
    int getBrightness();

    /*!
     * time it takes the LEDs to turn off in minutes
     */
    bool setTimeOut(int timeOut);
    int getTimeOut();

    /*!
     *  Time between LED updates as FPS * 100. For example,
     *  a FPS of 5 is 500.
     */
    bool setSpeed(int speed);
    int getSpeed();

    /*!
     * number of colors in the color array
     */
    bool setColorCount(int count);
    int getColorCount();

    /*!
     * \brief resetToDefaults resets the GUI and the arduino to the default values, as defined at compile time.
     */
    void resetToDefaults();

private:
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
