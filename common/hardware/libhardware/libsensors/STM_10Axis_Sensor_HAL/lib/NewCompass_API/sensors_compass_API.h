/******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
* File Name		: sensors_compass_API.h
* 			: AMS - Motion Mems Div. - Application Team
*			: Carmine Iascone (carmine.iascone@st.com)
* Version		: Revision: 2.0.0
* Description		: Sensors Compass API
*******************************************************************************
* History:
* Date		| Modification				| Author
*
* 06/05/2010	| First release V 1.0: 			| MSH - Motion Mems BU -
* 		  					| Application Team:
* 							| Carmine Iascone
*
* 09/01/2012	| V 1.1:	 			| AMS - Motion Mems Div.
* 		  Added getCalibrationData() function	| Application Team:
* 							| Carmine Iascone
* 16/07/2014	| V 2.0: Added SI calibration		| Application Team
*******************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR REFERENCE OR
* EDUCATION. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
******************************************************************************/

#define CALIB_FACTOR_NUM	2

#define FILTER_WEIGHT_LOW	5
#define FILTER_WEIGHT_HIGH	160

#define DEFAULT_MAGNITUDE_MG	500
#define DEFAULT_NORMALIZATION_FACTOR	(float)1.0f
#define DEFAULT_CALIB_DATA_FILE		"/data/calib_data_file.dat"
#define DEFAULT_SI_MATRIX_FILEPATH	"/data/calib_SI_matrix_data_file.dat"

typedef struct {
	float magOffX;	/* X axis Offset		*/
	float magOffY;	/* Y axis Offset		*/
	float magOffZ;	/* Z axis Offset		*/
	float magGainX;		/* X axis Gain			*/
	float magGainY;		/* Y axis Gain			*/
	float magGainZ;		/* Z axis Gain			*/
	float expMagVect; 	/* expected magnetic field 	*/
	unsigned int avgLength;
	signed short deltaX;
	signed short deltaY;
	signed short deltaZ;
} CalibFactor;

typedef struct {
	float si_matrix[3][3];
	float norm_factor;
} SI_Factor;

typedef struct {
	float azimuth;
	float pitch;
	float roll;
} orientation_data;

/**
 * Initialize compass library,to use every time the magnetometer sensor is
 * enabled.
 *
 * @param MagFullScale the magnitude full scale
 * @param formFactorNumber
 * @param calib_file calibration file name, if is NULL will be used default
 * 						value (/data/calib_data_file.dat)
 */
int compass_API_Init(int MagFullScale, int formFactorNumber,
							const char *calib_file);

/**
 * Save Accelerometer data in ENU systems coordinate
 *
 * @param acc_x X component [m/s^2]
 * @param acc_y Y component [m/s^2]
 * @param acc_z Z component [m/s^2]
 */
int compass_API_SaveAcc(float acc_x, float acc_y, float acc_z);

/**
 * Save Magnetometer data in ENU systems coordinate
 *
 * @param acc_x X component [uT]
 * @param acc_y Y component [uT]
 * @param acc_z Z component [uT]
 */
int compass_API_SaveMag(float mag_x, float mag_y, float mag_z);

/**
 * Return new fullscale of Magnetometer
 */
int compass_API_GetNewFullScale(void);

/**
 * STCompass_API_Run: run calibration algorithm (run  at 25Hz)
 *
 * @return true if is running, false otherwise
 */
int compass_API_Run(void);

/**
 * Return Calibration Goodness value
 */
unsigned int compass_API_GetCalibrationGodness(void);

/**
 * Get calibration data accuracy
 *
 * @return 1 = LOW ACCURACY, 2 = MEDIUM ACCURACY, 3 = HIGH ACCURACY.
 */
unsigned short compass_API_Get_Calibration_Accuracy(void);

/**
 * Force new Recalibration
 */
void compass_API_ForceReCalibration(void);

/**
 * Set new FormFactor
 *
 * @param formFactorNumber
 */
void compass_API_ChangeFormFactor(int formFactorNumber);

/**
 * Get Calibration parameters
 *
 * @param CalibrationData the new calibration data
 */
void compass_API_getCalibrationData(CalibFactor* CalibrationData);

/**
 * Get Orientation Values
 *
 * @param data returned orientation data
 */
void compass_API_OrientationValues(orientation_data *data);

/**
 * Load Soft Iron calibration matrix from file
 *
 * @param filename the calibration file to read
 * @return 0 if succes, -1 if couldn't read the file, -2 if the file struct is
 * 		not well formatted
 */
int compass_API_loadSIMatrixFromFile(const char *filename);

/**
 * Load Soft Iron calibration parameters from an hard coded struct
 *
 * @param factor the soft iron calibration parameters
 */
void compass_API_loadSIMatrixFromVector(SI_Factor* factor);

/**
 * Return the soft iron calibrated data. The uncalibrated data is the latest one
 * passed to the lib using the compass_API_SaveMag function
 *
 * @param data the calibrated adimensional data vector [3]
 */
void compass_API_getSICalibratedData(float *data);
