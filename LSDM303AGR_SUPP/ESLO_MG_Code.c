if (USE_MG == ESLO_MODULE_ON) {
	whoamI = 0;
	lsm303agr_mag_device_id_get(&dev_ctx_mg, &whoamI);

	if (whoamI != LSM303AGR_ID_MG) {
		while (1) {
		}
	}

	/* Restore default configuration for magnetometer */
	lsm303agr_mag_reset_set(&dev_ctx_mg, PROPERTY_ENABLE);
	do {
		lsm303agr_mag_reset_get(&dev_ctx_mg, &rst);
	} while (rst);

	lsm303agr_mag_block_data_update_set(&dev_ctx_mg, PROPERTY_ENABLE);
	lsm303agr_mag_data_rate_set(&dev_ctx_mg, LSM303AGR_MG_ODR_10Hz);
	lsm303agr_mag_set_rst_mode_set(&dev_ctx_mg,
			LSM303AGR_SENS_OFF_CANC_EVERY_ODR);
	lsm303agr_mag_offset_temp_comp_set(&dev_ctx_mg, PROPERTY_ENABLE);
	lsm303agr_mag_drdy_on_pin_set(&dev_ctx_mg, PROPERTY_ENABLE);
}

if (USE_MG == ESLO_MODULE_ON) {
	GPIO_enableInt(AXY_MAG);
	lsm303agr_mag_operating_mode_set(&dev_ctx_mg,
			LSM303AGR_CONTINUOUS_MODE);
} else {
	lsm303agr_mag_operating_mode_set(&dev_ctx_mg, LSM303AGR_POWER_DOWN);
}