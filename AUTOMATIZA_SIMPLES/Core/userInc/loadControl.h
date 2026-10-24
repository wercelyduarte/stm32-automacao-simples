/*
 * loadControl.h
 *
 *  Created on: Dec 9, 2023
 *      Author: Wercely
 */

#ifndef USERINC_LOADCONTROL_H_
#define USERINC_LOADCONTROL_H_

typedef enum {
    LOAD_ON = 0,
    LOAD_OFF,
    LOAD_TOGGLE,
    LOAD_ERROR
} load_value_en;

void loadControlInitialize(void);
void loadSetValue(load_value_en value);

#endif /* USERINC_LOADCONTROL_H_ */
