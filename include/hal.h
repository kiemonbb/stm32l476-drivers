#ifndef HAL_H
#define HAL_H

typedef enum {
	HAL_OK,
	HAL_ERROR,
	HAL_ERROR_TIMEOUT,
    HAL_ERROR_INVALID_PARAM,
	HAL_ERROR_BUSY
} hal_status_t;

#endif
