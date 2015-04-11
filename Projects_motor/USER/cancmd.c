#include "step.h"
#include "can.h"
#include "main.h"

#include "cancmd.h"
#include "anniuled.h"
#include "main.h"
#include "StepRefpwm.h"
#include "algorithm.h"





void CanCmd_ResetDumu(CAN_WP *wp) {
    if (wp->dlc % 2 == 0) {
        return;
    }
    unsigned int num = wp->dlc / 2;
    unsigned int imotor;
    DEFINE_CAN_WP_FRAME(twp);
    twp.dlc = wp->dlc;
    twp.funcode = CANCMD_RESETDUMU;
    twp.desid = wp->srcid;
    for (int i = 0; i < num; i++) {
        imotor = wp->data[i * 2];
        uint32 val = wp->data[i * 2 + 1];
        if (val == 0x80 || val == 0x82) {
            ResetDumu(imotor, val);
            twp.data[i * 2] = imotor;
            twp.data[i * 2 + 1] = MiduMotoCtrPra[imotor].MotoRunState;
        }
    }
    wpSend(&twp);
}


/*void CanCmd_SetDumuMotoHz(CAN_WP *wp) {
    if (wp->dlc!=4 || wp->dlc!=8) {
        return;
    }
    for (int i = 0; i < wp->dlc / 4; i++) {
    }

    uint32_t capture;

    unsigned int motor = wp->data[i * 4];
    if (motor >= MiDuMotoNum) {
        continue;
    }
    unsigned int Hzval = wp->data[i * 4 + 2] << 8 + wp->data[i * 4 + 1] << 8;
    if (Hzval < DUMUMOTOSTOPHZ) {
        if (MiduMotoCtrPra[moto].MotoRunState == MOTOWORKING) {
            switch (moto) {
            case MIDUMOTO1:
                { TIM_ITConfig(TIM1, TIM_IT_CC1, DISABLE); break;}
            case MIDUMOTO2:
                { TIM_ITConfig(TIM1, TIM_IT_CC2, DISABLE); break;}
            case MIDUMOTO3:
                { TIM_ITConfig(TIM1, TIM_IT_CC3, DISABLE); break;}
            case MIDUMOTO4:
                { TIM_ITConfig(TIM1, TIM_IT_CC4, DISABLE); break;}
            default:
                break;
            }
            MiduMotoCtrPra[moto].MotoRunState = MOTOSTOPPAUSE;
            MiduMotoRefCtr(moto, ENABLE);
        }
    } else if (MiduMotoCtrPra[moto].MotoRunState == MOTOSTOPPAUSE) {
        MiduMotoCtrPra[moto].MotoRunHzMax = Hzbuf[moto];
        GetMotoAddTab(moto, MiduMotoCtrPra[moto].MotoRunHzMax);
        //�趨��һ�δ���ֵ
        MiduMotoCtrPra[moto].MotoRunCmpValue = GetMotoCmpValue(MiduMotoCtrPra[moto].MotoRunHzDangqian);
        MiduMotoRefCtr(moto, DISABLE);
        MiduMotoFreeCtr(moto, DISABLE);
        MiduMotoCtrPra[moto].MotoRunState = MOTOWORKING;
        switch (moto) {
        case MIDUMOTO1:
            capture = TIM_GetCounter(TIM1) + MiduMotoCtrPra[MIDUMOTO1].MotoRunCmpValue;
            TIM_SetCompare1(TIM1, capture);
            TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
            TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
            break;
        case MIDUMOTO2:
            capture = TIM_GetCounter(TIM1) + MiduMotoCtrPra[MIDUMOTO2].MotoRunCmpValue;
            TIM_SetCompare2(TIM1, capture);
            TIM_ClearFlag(TIM1, TIM_FLAG_CC2);
            TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);
            break;
        case MIDUMOTO3:
            capture = TIM_GetCounter(TIM1) + MiduMotoCtrPra[MIDUMOTO3].MotoRunCmpValue;
            TIM_SetCompare3(TIM1, capture);
            TIM_ClearFlag(TIM1, TIM_FLAG_CC3);
            TIM_ITConfig(TIM1, TIM_IT_CC3, ENABLE);
            break;
        case MIDUMOTO4:
            capture = TIM_GetCounter(TIM1) + MiduMotoCtrPra[MIDUMOTO4].MotoRunCmpValue;
            TIM_SetCompare4(TIM1, capture);
            TIM_ClearFlag(TIM1, TIM_FLAG_CC4);
            TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
            break;
        default:
            break;
        }
    } else if (MiduMotoCtrPra[moto].MotoRunState == MOTOWORKING) {
        MiduMotoCtrPra[moto].MotoRunHzMax = Hzbuf[moto];
        GetMotoAddTab(moto, MiduMotoCtrPra[moto].MotoRunHzMax);
    }
}*/



//��ȡ��Ŀ���״̬
void  CanCmd_GetDumuMotoState(CAN_WP *wp) {
    uint16_t i, j;
    i = 0X03;
    uint32 moto = wp->data[0] & 0x03;
    MiduMotoCtrPra[moto].shiji_zero_zhuangtai = GetMiduMotoZero(moto);
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = CANCMD_GETDUMUMOTOSTATE;
    twp.desid = wp->srcid;
    twp.dlc = 8;
    twp.data[0] = moto;
    twp.data[1] = MiduMotoCtrPra[moto].MotoDumuDangqian & 0xff;
    twp.data[2] = MiduMotoCtrPra[moto].MotoDumuDangqian >> 8;
    twp.data[3] = MiduMotoCtrPra[moto].MotoRunHz & 0xff;
    twp.data[4] = (MiduMotoCtrPra[moto].MotoRunHz) >> 8;
    twp.data[5] = MiduMotoCtrPra[moto].MotoRunSetHz & 0xff;
    twp.data[6] = (MiduMotoCtrPra[moto].MotoRunSetHz) >> 8;
    i = 0;
    j = MiduMotoCtrPra[moto].MotoRunDir;
    i = j << 6;
    j = MiduMotoCtrPra[moto].MotoRunState;
    i = i | (j << 4);
    j = MiduMotoCtrPra[moto].shiji_zero_zhuangtai;
    i = i | (j << 3);
    j = MiduMotoCtrPra[moto].MotoZeroState[2];
    i = i | (j << 2);
    j = MiduMotoCtrPra[moto].MotoZeroState[1];
    j = j << 1;
    i = i | j | MiduMotoCtrPra[moto].MotoZeroState[0];
    twp.data[7] = (uint8_t)i;
    wpSend(&twp);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define Max_Current_Num 25
void Can_Cmd_Workcurrent_Set(CAN_WP *wp) {

    uint8_t val;
    for (int i = 0; i < MIN(4, wp->dlc); i++) {
        val = wp->data[i];
        //����� 1~4  ������λ1~25 ��Ϊ��Ч����
        if ((val > 0) && (val < (Max_Current_Num + 1))) {
            ChannelNum[i] = val - 1;
        }
    }
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = CANCMD_WORKCURRENT_SET;
    twp.desid = wp->srcid;
    twp.dlc = 0;
    wpSend(&twp);
}



void Can_Cmd_Lockcurrent_Set(CAN_WP *wp) {
    if (wp->dlc%2!=0) {
        return;
    }
    unsigned int num = wp->dlc/2;
    uint8_t Current_value;
    for (int i = 0; i < num; i++) {
        uint32 imotor = wp->data[i*2];
        if (imotor>=MIDUMOTOBEGINHZ) {
            continue ;
        }
        Current_value =  MAX(wp->data[i * 2 + 1], Max_Current_Num);
        if (Current_value > 0) {
            HalfPulseNum[imotor] = Current_value - 1;
            //����ǰ�ǰ�������ǿ�Ƹı���������
            if (MiduMotoCtrPra[imotor].MotoRunState == MOTOSTOPHALF) {
                MiduMotoRefCtr(imotor, ENABLE);
            }
        }
    }
    DEFINE_CAN_WP_FRAME(twp);
    twp.funcode = CANCMD_LOCKCURRENT_SET;
    twp.desid = wp->srcid;
    twp.dlc = 0;
    wpSend(&twp);
}




//��Ŀ���ִ��CAN����
void CanCmd_DumuMotoGo(CAN_WP *wp) {

    MOTOR_CMD cmd;   
    uint32 moto = wp->data[0];
    if (moto>=4) {
        return;
    }
    cmd.puls = wp->data[2]<<8 | wp->data[1];;
    cmd.speedHz = wp->data[4] << 8 | wp->data[3];;
    cmd.stopflag = wp->data[5] & 0x3f;;
    cmd.stat = MOTOR_CMD_STAT_WAITE;
    cmd.dir = (wp->data[5] & 0x40)?OUT_DIR_ZHENG:OUT_DIR_FAN;
    if ((cmd.speedHz >= DUMUMOTOSTOPHZ) && (cmd.puls <= 15000)) {
        ringBufPush(&motorcmdbuf[moto],&cmd);
    }
}




void doCmdWp(CAN_WP *wp) {
    if (CAN_WP_GET_ID(wp->desid) == 0) {
        return;
    }
    if (DeviceCanAddr == 0) {
        return;
    }
    switch (wp->funcode) {
    case CANCMD_GETDUMUMOTOSTATE:
        CanCmd_GetDumuMotoState(wp);
        break;
    case CANCMD_SETDUMUMOTOHZ:
        //CanCmd_SetDumuMotoHz(wp);
        break;
    case CANCMD_RESETDUMU:
        CanCmd_ResetDumu(wp);
        break;
    case CANCMD_WORKCURRENT_SET:
        Can_Cmd_Workcurrent_Set(wp);
        break;
    case CANCMD_LOCKCURRENT_SET:
        Can_Cmd_Lockcurrent_Set(wp);
        break;
    case CANCMD_DUMUMOTOGO_LOW:
        CanCmd_DumuMotoGo(wp);
        break;
    default:
        break;
    }
}




