#ifndef __ENCODER_H
#define __ENCODER_H

typedef enum Num_Encoder
{
	Encoder_left=0,
	Encoder_right
}Num_Encoder_t;

void Encoder_Init(void);
int16_t Encoder_Get(Num_Encoder_t Num_Encoder);

#endif
