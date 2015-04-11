//#include "step.h"
#include "can.h"
#include "main.h"
#include "swqf_output.h"

#include "cancmd.h"
#include "anniuled.h"
#include "main.h"
#include "pwm.h"
#include "can_wp.h"
#include "algorithm.h"
#include "can_dmp.h"

#include "Check_Qifa_Res_ADC.h"	       //adc���QIFA res


//const uint8_t Ver[36]@(APP_ADDR)={"SoftVer-A.SQFA.A1.00.01-" __DATE__} ;
//const uint8_t Ver[]@(APP_ADDR)=     {"SoftVer-A.SQFA.PB-A.01.00.00&"__DATE__};





void CanCmd_SetQfCs(CAN_WP *wp) {
    int32 val = !!wp->data[0];
    QifaEnCtr(val);
}

void CanCmd_QfRst(CAN_WP *wp) {
    int32 val = !!wp->data[0];
    QifaRstCtr(val);
}

void CanCmd_SetPwmValue(CAN_WP *wp) {
    if (wp->dlc != 1) {
        return;
    }
    uint32 val = wp->data[0];
    if (val >= 60 && val <= 95) {
        TIMSetPwmValue(TIM16, val);
    }
}



void CanCmd_ReadAlarm(CAN_WP *wp) {
    uint8 val = Scan_Input_state();
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = wp->funcode;
    twp.desid = wp->srcid;
    twp.dlc = 1;
    twp.data[0] = val;
    wpSend(&twp);
}



void ScanAlarm(void) {
    if (DeviceCanAddr==0) {
        return;
    }
    uint32 val = Scan_Input_state();
    if (val != (1UL << Max_Input_Single)-1) {
        DEFINE_CAN_WP_FRAME(twp);
        twp.funcode = CANCMD_SENDALARM;
        twp.desid = CAN_WP_ID_GROUP_BROADCAST(CAN_WP_DEV_TYPE_MAIN);
        twp.dlc = 1;
        twp.data[0] = val;
        wpSend(&twp);
    }
}




//����������� ���Ե���
void CanCmd_TestQifaOhm(CAN_WP *wp) {
    if(wp->desid)
    if (wp->dlc != 1) {
        return;
    }
    if (!(wp->data[0] & 0x80)) {
        return;
    }
    uint32 num = wp->data[0] & 0X7F;
    if (num >= 16) {
        return;
    }

    //����ͨ��
    QifaDrive(1, num);

    ADC_Value.Num = num;
    //Qifa_Io_Out_Single(1, ADC_Value.Num);
    //���������Ա�־ ��������������ʱ��־
    ADC_Value.Start_enable = 1;
    ADC_Value.end = 0;
    ADC_Value.delaytime = Max_ADC_DelayTIME;
    //ADC_Value.Count = Max_ADC_Value ;
}




void doCmdWp(CAN_WP *wp) {
    if (CAN_WP_GET_ID(wp->desid) == 0) {
        return;
    }
    if (DeviceCanAddr == 0) {
        return;
    }
    switch (wp->funcode) {
    case CANCMD_SETPWMEN:
        CanCmd_SetPwmEn(wp);
        break;
        //8803 Ƭѡʹ��
    case CANCMD_SETQIFAEN:
        CanCmd_SetQfCs(wp);
        break;
        //8803 RST ��λʹ�ܺ͹ر�(8803 RST ΪH  ��λ״̬)
    case CANCMD_QIFARST:
        CanCmd_QfRst(wp);
        break;
        //�趨PWM �ߵ�ƽʱ��
    case CANCMD_SETPWMVALUE:
        CanCmd_SetPwmValue(wp);
        break;
        //����IO�˿���� (��������)
    case CANCMD_QIFADRV:
        CanCmd_QifaDrv(wp);
        break;
        //��������������Ե���
    case CANCMD_SINGAL_QIFA:
        CanCmd_TestQifaOhm(wp);
        break;
        //��ȡ��������״̬
    case CANCMD_READALARM:
        CanCmd_ReadAlarm(wp);
        break;
        //��ȡ������ǰ״̬
    case CANCMD_READQIFA1TO16:
        CanCmd_ReadQifa1to16(wp);
        break;
    default:
        break;
    }
}




//�������
void CanCmd_QifaDrv(CAN_WP *wp) {

    uint16_t value;
    int32 qifa;
    if(IS_CAN_WP_ID_GROUP_BROADCAST(wp->desid)){
      return;
    }
    for (uint32 i = 0; i < wp->dlc; i++) {
        value = (wp->data[i] & 0x80) ? 1 : 0;
        qifa = wp->data[i] & 0x7f;
        if (qifa < 16) {
            QifaDrive(value, qifa);
        }
    }
}



//�趨PWMʹ����Ч �� PWM Ƶ��
void CanCmd_SetPwmEn(CAN_WP *wp) {
    uint8_t value = wp->data[0];
    QifaPwmCtr(value);
    if (value) Qifa_state.Out_PWM_Enable = ENABLE;
    else Qifa_state.Out_PWM_Enable = DISABLE;
}




// ��ȡ����1~16�� ��ǰ״̬
void CanCmd_ReadQifa1to16(CAN_WP *wp) {

    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = wp->funcode;
    twp.desid = wp->srcid;
    twp.dlc = 2;
    twp.data[0] = Qifa_state.fg_qifa_out & 0xff;
    twp.data[1] = (Qifa_state.fg_qifa_out / 256) & 0xff;
    wpSend(&twp);
}



//�����巢����������ֵ
void CanCmd_ReturnQIfa_Res_value(uint8_t num, uint16_t res_value) {
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = CANCMD_SINGAL_QIFA;
    twp.desid = 0; //todo
    twp.dlc = 3;
    twp.data[0] = num;
    twp.data[1] = res_value & 0xff;
    twp.data[2] = (res_value / 256) & 0xff;
    wpSend(&twp);
}

