#pragma once

#include "vl53l5cx_api.h"

/**
 * @brief Inner internal number of targets.
 */

#if VL53L5CX_NB_TARGET_PER_ZONE == 1
#define VL53L5CX_FW_NBTAR_XTALK	2
#else
#define VL53L5CX_FW_NBTAR_XTALK	VL53L5CX_NB_TARGET_PER_ZONE
#endif

/**
 * @brief Inner Macro for plugin. Not for user, only for development.
 */

#define VL53L5CX_DCI_CAL_CFG				((uint16_t)0x5470U)
#define VL53L5CX_DCI_XTALK_CFG				((uint16_t)0xAD94U)


/**
 * This function starts the VL53L5CX sensor in order to calibrate Xtalk.
 * This calibration is recommended is user wants to use a coverglass.
 * @param (VL53L5CX_Configuration) *p_dev : VL53L5CX configuration structure.
 * @param (uint16_t) reflectance_percent : Target reflectance in percent. This
 * value is include between 1 and 99%. For a better efficiency, ST recommends a
 * 3% target reflectance.
 *
 * @param (uint8_t) nb_samples : Nb of samples used for calibration. A higher
 * number of samples means a higher accuracy, but it increases the calibration
 * time. Minimum is 1 and maximum is 16.
 *
 * @param (uint16_t) distance_mm : Target distance in mm. The minimum allowed
 * distance is 600mm, and maximum is 3000mm. The target must stay in Full FOV,
 * so short distance are easier for calibration.
 *
 * @return (uint8_t) status : 0 if calibration OK, 127 if an argument has an
 * incorrect value, or 255 is something failed.
 */

uint8_t vl53l5cx_calibrate_xtalk(
		VL53L5CX_Configuration		*p_dev,
		uint16_t			reflectance_percent,
		uint8_t				nb_samples,
		uint16_t			distance_mm);

/**
 * @brief This function gets the Xtalk buffer. The buffer is available after
 * using the function vl53l5cx_calibrate_xtalk().
 * @param (VL53L5CX_Configuration) *p_dev : VL53L5 configuration structure.
 * @param (uint8_t) *p_xtalk_data : Buffer with a size defined by
 * macro VL53L5CX_XTALK_SIZE.
 * @return (uint8_t) status : 0 if buffer reading OK
 */

uint8_t vl53l5cx_get_caldata_xtalk(
		VL53L5CX_Configuration		*p_dev,
		uint8_t				*p_xtalk_data);

/**
 * @brief This function sets the Xtalk buffer. This function can be used to
 * override default Xtalk buffer.
 * @param (VL53L5CX_Configuration) *p_dev : VL53L5 configuration structure.
 * @param (uint8_t) *p_xtalk_data : Buffer with a size defined by
 * macro VL53L5CX_XTALK_SIZE.
 * @return (uint8_t) status : 0 if buffer OK
 */

uint8_t vl53l5cx_set_caldata_xtalk(
		VL53L5CX_Configuration		*p_dev,
		uint8_t				*p_xtalk_data);

/**
 * @brief This function gets the Xtalk margin. This margin is used to increase
 * the Xtalk threshold. It can also be used to avoid false positives after the
 * Xtalk calibration. The default value is 50 kcps/spads.
 * @param (VL53L5CX_Configuration) *p_dev : VL53L5CX configuration structure.
 * @param (uint32_t) *p_xtalk_margin : Xtalk margin in kcps/spads.
 * @return (uint8_t) status : 0 if reading OK
 */

uint8_t vl53l5cx_get_xtalk_margin(
		VL53L5CX_Configuration		*p_dev,
		uint32_t			*p_xtalk_margin);

/**
 * @brief This function sets the Xtalk margin. This margin is used to increase
 * the Xtalk threshold. It can also be used to avoid false positives after the
 * Xtalk calibration. The default value is 50 kcps/spads.
 * @param (VL53L5CX_Configuration) *p_dev : VL53L5CX configuration structure.
 * @param (uint32_t) xtalk_margin : New Xtalk margin in kcps/spads. Min value is
 * 0 kcps/spads, and max is 10.000 kcps/spads
 * @return (uint8_t) status : 0 if set margin is OK, or 127 is the margin is
 * invalid.
 */

uint8_t vl53l5cx_set_xtalk_margin(
		VL53L5CX_Configuration		*p_dev,
		uint32_t			xtalk_margin);

/**
 * @brief Command used to get Xtalk calibration data
 */

static const uint8_t VL53L5CX_GET_XTALK_CMD[] = {
	0x54, 0x00, 0x00, 0x40,
	0x9F, 0xD8, 0x00, 0xC0,
	0x9F, 0xE4, 0x01, 0x40,
	0x9F, 0xF8, 0x00, 0x40,
	0x9F, 0xFC, 0x04, 0x04,
	0xA0, 0xFC, 0x01, 0x00,
	0xA1, 0x0C, 0x01, 0x00,
	0xA1, 0x1C, 0x00, 0xC0,
	0xA1, 0x28, 0x09, 0x02,
	0xA2, 0x48, 0x00, 0x40,
	0xA2, 0x4C, 0x00, 0x81,
	0xA2, 0x54, 0x00, 0x81,
	0xA2, 0x5C, 0x00, 0x81,
	0xA2, 0x64, 0x00, 0x81,
	0xA2, 0x6C, 0x00, 0x84,
	0xA2, 0x8C, 0x00, 0x82,
	0x00, 0x00, 0x00, 0x0F,
	0x07, 0x02, 0x00, 0x44
};

/**
 * @brief Command used to get run Xtalk calibration
 */

static const uint8_t VL53L5CX_CALIBRATE_XTALK[] = {
	0x54, 0x50, 0x00, 0x80,
	0x00, 0x04, 0x08, 0x08,
	0x00, 0x00, 0x04, 0x04,
	0xAD, 0x30, 0x00, 0x80,
	0x03, 0x01, 0x06, 0x03,
	0x00, 0x00, 0x01, 0x00,
	0xAD, 0x38, 0x01, 0x00,
	0x01, 0xE0, 0x01, 0x40,
	0x00, 0x10, 0x00, 0x10,
	0x01, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01,
	0x54, 0x58, 0x00, 0x40,
	0x04, 0x1A, 0x02, 0x00,
	0x54, 0x5C, 0x01, 0x40,
	0x00, 0x01, 0x00, 0x51,
	0x00, 0x00, 0x0F, 0xA0,
	0x0F, 0xA0, 0x03, 0xE8,
	0x02, 0x80, 0x1F, 0x40,
	0x00, 0x00, 0x05, 0x00,
	0x54, 0x70, 0x00, 0x80,
	0x03, 0x20, 0x03, 0x20,
	0x00, 0x00, 0x00, 0x08,
	0x54, 0x78, 0x01, 0x00,
	0x01, 0x1B, 0x00, 0x21,
	0x00, 0x33, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x01,
	0x04, 0x01, 0x08, VL53L5CX_FW_NBTAR_XTALK,
	0x54, 0x88, 0x01, 0x40,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x00,
	0xAD, 0x48, 0x01, 0x00,
	0x01, 0xF4, 0x00, 0x00,
	0x03, 0x06, 0x00, 0x10,
	0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x00, 0x08,
	0xAD, 0x60, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00,
	0x20, 0x1F, 0x01, 0xF4,
	0x00, 0x00, 0x1D, 0x0A,
	0xAD, 0x70, 0x00, 0x80,
	0x08, 0x00, 0x1F, 0x40,
	0x00, 0x00, 0x00, 0x01,
	0xAD, 0x78, 0x00, 0x80,
	0x00, 0xA0, 0x03, 0x20,
	0x00, 0x01, 0x01, 0x90,
	0xAD, 0x80, 0x00, 0x40,
	0x00, 0x00, 0x28, 0x00,
	0xAD, 0x84, 0x00, 0x80,
	0x00, 0x00, 0x32, 0x00,
	0x03, 0x20, 0x00, 0x00,
	0xAD, 0x8C, 0x00, 0x80,
	0x02, 0x58, 0xFF, 0x38,
	0x00, 0x00, 0x00, 0x0C,
	0xAD, 0x94, 0x01, 0x00,
	0x00, 0x01, 0x90, 0x00,
	0xFF, 0xFF, 0xFC, 0x00,
	0x00, 0x00, 0x04, 0x00,
	0x00, 0x00, 0x01, 0x00,
	0xAD, 0xA4, 0x00, 0xC0,
	0x04, 0x80, 0x06, 0x1A,
	0x00, 0x80, 0x05, 0x80,
	0x00, 0x00, 0x01, 0x06,
	0xAD, 0xB0, 0x00, 0xC0,
	0x04, 0x80, 0x06, 0x1A,
	0x19, 0x00, 0x05, 0x80,
	0x00, 0x00, 0x01, 0x90,
	0xAD, 0xBC, 0x04, 0x40,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x12, 0x00, 0x25,
	0x00, 0x00, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x0F,
	0x00, 0x00, 0x00, 0x5A,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x09,
	0x0B, 0x0C, 0x0B, 0x0B,
	0x03, 0x03, 0x11, 0x05,
	0x01, 0x01, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x0D, 0x00, 0x00,
	0xAE, 0x00, 0x01, 0x04,
	0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x0A,
	0x00, 0x00, 0x00, 0x0C,
	0x00, 0x00, 0x00, 0x0D,
	0x00, 0x00, 0x00, 0x0E,
	0x00, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x08,
	0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x06,
	0x00, 0x00, 0x05, 0x0A,
	0x02, 0x00, 0x0C, 0x08,
	0x00, 0x00, 0x00, 0x00,
	0xAE, 0x40, 0x00, 0x40,
	0x00, 0x00, 0x00, 0xFF,
	0xAE, 0x44, 0x00, 0x40,
	0x00, 0x10, 0x04, 0x01,
	0xAE, 0x48, 0x00, 0x40,
	0x00, 0x00, 0x10, 0x00,
	0xAE, 0x4C, 0x00, 0x40,
	0x00, 0x00, 0x00, 0x01,
	0xAE, 0x50, 0x01, 0x40,
	0x00, 0x00, 0x00, 0x14,
	0x04, 0x00, 0x28, 0x00,
	0x03, 0x20, 0x6C, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xAE, 0x64, 0x00, 0x40,
	0x00, 0x00, 0x00, 0x01,
	0xAE, 0xD8, 0x01, 0x00,
	0x00, 0xC8, 0x05, 0xDC,
	0x00, 0x00, 0x0C, 0xCD,
	0x01, 0x04, 0x00, 0x00,
	0x00, 0x01, 0x26, 0x01,
	0xB5, 0x50, 0x02, 0x82,
	0xA3, 0xE8, 0xA3, 0xB8,
	0xA4, 0x38, 0xA4, 0x28,
	0xA6, 0x48, 0xA4, 0x48,
	0xA7, 0x88, 0xA7, 0x48,
	0xAC, 0x10, 0xA7, 0x90,
	0x99, 0xBC, 0x99, 0xB4,
	0x9A, 0xFC, 0x9A, 0xBC,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB5, 0xA0, 0x02, 0x82,
	0x00, 0x88, 0x03, 0x00,
	0x00, 0x82, 0x00, 0x82,
	0x04, 0x04, 0x04, 0x08,
	0x00, 0x80, 0x04, 0x01,
	0x09, 0x02, 0x09, 0x08,
	0x04, 0x04, 0x00, 0x80,
	0x04, 0x01, 0x04, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB5, 0xF0, 0x00, 0x40,
	0x00, 0x04, 0x00, 0x00,
	0xB3, 0x9C, 0x01, 0x00,
	0x40, 0x00, 0x05, 0x1E,
	0x02, 0x1B, 0x08, 0x7C,
	0x80, 0x01, 0x12, 0x01,
	0x00, 0x00, 0x08, 0x00,
	0xB6, 0xC0, 0x00, 0xC0,
	0x00, 0x00, 0x60, 0x00,
	0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xAE, 0xA8, 0x00, 0x40,
	0x00, 0x00, 0x04, 0x05,
	0xAE, 0xAC, 0x00, 0x80,
	0x01, 0x00, 0x01, 0x00,
	0x00, 0x02, 0x00, 0x00,
	0xAE, 0xB4, 0x00, 0x40,
	0x00, 0x00, 0x00, 0x00,
	0xAE, 0xB8, 0x00, 0x81,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xAE, 0xC0, 0x00, 0x81,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xAE, 0xC8, 0x00, 0x81,
	0x08, 0x01, 0x01, 0x08,
	0x00, 0x00, 0x00, 0x08,
	0xAE, 0xD0, 0x00, 0x81,
	0x01, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x00, 0x01,
	0xB5, 0xF4, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB5, 0xFC, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x04, 0x00, 0x40,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x08, 0x00, 0x44,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x18, 0x00, 0x44,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x28, 0x00, 0x44,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x38, 0x00, 0x44,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x48, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x58, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0xB6, 0x68, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x54, 0x70, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x0F,
	0x00, 0x01, 0x03, 0xD4
};
