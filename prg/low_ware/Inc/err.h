/*
 * err.h
 *
 *  Created on: 26 aug. 2018
 *      Author: Dorus
 */

#ifndef ERR_H_
#define ERR_H_

#ifdef __cplusplus
 extern "C" {
#endif

/**
* @brief Uncomment the line below to expanse the "assert_param" macro in the
* HAL drivers code
*/
/* #define USE_FULL_ASSERT    1U */

void ErrorHandler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* ERR_H_ */
