/*******************************************************************************
IO_INPUT.C
********************************************************************************/
#include "swqf_output.h"
#include "pwm.h"
#include "timer.h"



//////////////////////////////////////////////////////////////////////////
const GPIOGROUP Output_Single[Max_Output_Single] = {
    { Output0_PORT, Output0_PIN },
    { Output1_PORT, Output1_PIN},
    { Output2_PORT, Output2_PIN},
    { Output3_PORT, Output3_PIN},
    { Output4_PORT, Output4_PIN},
    { Output5_PORT, Output5_PIN},
    { Output6_PORT, Output6_PIN},
    { Output7_PORT, Output7_PIN},
    { Output8_PORT, Output8_PIN},
    { Output9_PORT, Output9_PIN},
    { Output10_PORT, Output10_PIN},
    { Output11_PORT, Output11_PIN},
    { Output12_PORT, Output12_PIN},
    { Output13_PORT, Output13_PIN},
    { Output14_PORT, Output14_PIN},
    { Output15_PORT, Output15_PIN}
};
const GPIOGROUP_IoInput Input_Single[Max_Input_Single] = {
    { Input_QFERR1_PORT, Input_QF_ERR1_PIN },
    { Input_QFERR2_PORT, Input_QF_ERR2_PIN},
    { Input_QFERR3_PORT, Input_QF_ERR3_PIN},
    { Input_QFERR4_PORT, Input_QF_ERR4_PIN}
};

S_qifa_state Qifa_state;
///////////////////////////////////////////////////////////////////////////////
//IO inint

void swqfout(uint8_t num, uint8_t on1off0) {
    if (on1off0) {
        GPIO_SetBits(Output_Single[num].port, Output_Single[num].bit);
    } else {
        GPIO_ResetBits(Output_Single[num].port, Output_Single[num].bit);
    }
}


void Swqf_Io_Init(void) {
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
    for (i = 0; i < Max_Output_Single; i++) {
        GPIO_InitStructure.GPIO_Pin = Output_Single[i].bit;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(Output_Single[i].port, &GPIO_InitStructure);
        swqfout(i, 0); //set 8803 IN PIN 0
    }
    //气阀8803复位状态  无效 复位状态
    GPIO_InitStructure.GPIO_Pin = Output_QF_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Output_QF_RST_PORT, &GPIO_InitStructure);
    GPIO_SetBits(Output_QF_RST_PORT, Output_QF_RST_PIN); //set 8803 reset

    //气阀8803 片选无效状态
    GPIO_InitStructure.GPIO_Pin = Output_QF_EN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(Output_QF_EN_PORT, &GPIO_InitStructure);
    GPIO_SetBits(Output_QF_EN_PORT, Output_QF_EN_PIN); //set 8803 EN PIN DISABLE

    //气阀8803报警  信号  INput
    for (i = 0; i < Max_Input_Single; i++) {
        GPIO_InitStructure.GPIO_Pin = Input_Single[i].bit;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(Input_Single[i].port, &GPIO_InitStructure);
    }
}



uint8_t Get_Io_Input(uint8_t num) {
    return(GPIO_ReadInputDataBit(Input_Single[num].port, Input_Single[num].bit));
}


uint8_t Scan_Input_state(void) {
    uint8_t i;
    uint8_t databuf = 0;
    for (i = 0; i < Max_Input_Single; i++) {
        databuf |= (Get_Io_Input(i) << i);
    }
    return(databuf & ~(1 << Max_Input_Single));
}


void QifaEnCtr(uint8_t value) {
    if (value) {
        GPIO_ResetBits(Output_QF_EN_PORT, Output_QF_EN_PIN);
    } else {
        GPIO_SetBits(Output_QF_EN_PORT, Output_QF_EN_PIN);
    }
}



static int32 qifa_reset = 0;
static long long qifa_reset_back_time05;
void QifaRstCtr(uint8_t value) {
    if (value) {
        //HIGH
        GPIO_SetBits(Output_QF_RST_PORT, Output_QF_RST_PIN);
		qifa_reset = 1;
		qifa_reset_back_time05 = timerTick05ms+2;
    } else {
        GPIO_ResetBits(Output_QF_RST_PORT, Output_QF_RST_PIN);
		qifa_reset = 0;
    }
}

void QifaRstProcess(){
    if ((qifa_reset==1) &&(qifa_reset_back_time05 <= timerTick05ms)) {
		GPIO_ResetBits(Output_QF_RST_PORT, Output_QF_RST_PIN);
		qifa_reset = 0;
    }
}

/*
// 气阀PWM 开启控制
*/
void QifaPwmCtr(uint8_t value) {
    if (value) {
        PWM_Plus_On();
    } else {
        PWM_Plus_OFF();
    }
}




void Qifa_Io_Out_Single(uint8_t onoff, uint8_t num) {
    swqfout(num, onoff) ;
    if (onoff) {
        GPIO_SetBits(Output_Single[ num ].port, Output_Single[ num ].bit);
        Qifa_state.fg_qifa_out |= (0X01 << num);
    } else {
        GPIO_ResetBits(Output_Single[ num ].port, Output_Single[ num ].bit);
        Qifa_state.fg_qifa_out &= ~(0X01 << num);
    }
}



void QifaDrive(uint8 on1off0, uint8 iqifa) {
    Qifa_Io_Out_Single(on1off0, iqifa);
    if (on1off0) {
        Qifa_state.PWM_Close_Time_Count = MAX_PWM_Close_Time;
        if (PwmPra.PWM_work_state == 1) {
            PWM_Plus_OFF();
        }
    }
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Qifa_state_Prainit(void) {
    Qifa_state.fg_qifa_out = 0;           //气阀输出 时间状态  BIT 为 1表示气阀输出 为0  表示气阀没有输出
    Qifa_state.Out_PWM_Enable = ENABLE;
    Qifa_state.PWM_Close_Time_Count = MAX_PWM_Close_Time;
}
