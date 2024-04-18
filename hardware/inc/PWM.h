#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void YLed_Set(_Bool status);
void WLed_Set(_Bool status);
typedef struct
{

	_Bool YLed_Status;

} YLED_INFO;
typedef struct
{

	_Bool WLed_Status;

} WLED_INFO;

#define YLED_ON		1

#define YLED_OFF	0

#define WLED_ON		1

#define WLED_OFF	0


extern YLED_INFO yled_info;
extern WLED_INFO wled_info;
#endif
