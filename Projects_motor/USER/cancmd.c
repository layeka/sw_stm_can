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
        //设定第一次触发值
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



//读取度目马达状态
void  CanCmd_GetDumuMotoState(CAN_WP *wp) {
    uint32 moto = wp->data[0];
    if (moto>=MiDuMotoNum) {
        return;
    }
    //MiduMotoCtrPra[moto].shiji_zero_zhuangtai = GetMiduMotoZero(moto);
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

    uint8_t stat =  MiduMotoCtrPra[moto].MotoRunDir<<6
                    | MiduMotoCtrPra[moto].MotoRunState<<4
                    | GetMiduMotoZero(moto)<<3
                    | MiduMotoCtrPra[moto].MotoZeroState[2]<<2
                    | MiduMotoCtrPra[moto].MotoZeroState[1]<<1
                    | MiduMotoCtrPra[moto].MotoZeroState[0];
    twp.data[7] = stat;
    wpSend(&twp);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define Max_Current_Num 25
void Can_Cmd_Workcurrent_Set(CAN_WP *wp) {

    uint8_t val;
    for (int i = 0; i < MIN(4, wp->dlc); i++) {
        val = wp->data[i];
        //电机号 1~4  电流档位1~25 才为有效数据
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
            //若当前是半流锁定强制改变锁定电流
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




//度目电机执行CAN命令
void CanCmd_DumuMotoGo(CAN_WP *wp) {

    MOTOR_CMD cmd;
    uint32 moto = wp->data[0];
    if (wp->dlc!=8) {
        return;
    }
    if (moto >= 4) {
        return;
    }
    cmd.puls = wp->data[2]<<8 | wp->data[1];
    cmd.speedHz = wp->data[4] << 8 | wp->data[3];
    cmd.stopflag = wp->data[5] & 0x3f;
    cmd.stat = MOTOR_CMD_STAT_WAITE;
    cmd.cmdcode = wp->data[7];
    cmd.dir = (wp->data[5] & 0x40)?OUT_DIR_ZHENG:OUT_DIR_FAN;
    if ((cmd.speedHz >= DUMUMOTOSTOPHZ) && (cmd.puls <= 15000)) {
        ringBufPush(&motorcmdbuf[moto],&cmd);
    }
}


static void motorcmdreturn(int motor, int pulse, int time, int stopflag, uint8_t dir,uint8_t cmdcode) {
    DEFINE_CAN_WP_FRAME(wpt);
    wpt.funcode = CANCMD_DUMUMOTOGO_LOW;
    wpt.dlc = 8;
    wpt.data[0] = motor;
    wpt.data[1] = (uint8_t)pulse;
    wpt.data[2] = (uint8_t)(pulse >> 8);
    wpt.data[3] = (uint8_t)time;
    wpt.data[4] = (uint8_t)(time >> 8);
    wpt.data[5] = (!!dir <<6) | (uint8_t)stopflag;
    wpt.data[6] = 0;
    wpt.data[7] = cmdcode;
    wpSend(&wpt);
}


void scanMotorCmd(void) {
    MOTOR_CMD *cmd;
    for (int i = 0; i < MiDuMotoNum; i++) {
        if (ringBufRead(&motorcmdbuf[i], (void **)&cmd)) {
            switch (cmd->stat) {
            case MOTOR_CMD_STAT_WAITE:
                if ((cmd->speedHz >= DUMUMOTOSTOPHZ) && (cmd->puls <= 15000)) {
                    cmd->stat = MOTOR_CMD_STAT_BUSY;
                    MotoDumuRun(i, cmd->dir, cmd->puls,
                                cmd->stopflag, cmd->speedHz);
                    cmd->timertick = timerTick1ms;
                } else {
                    ringBufPop_noread(&motorcmdbuf[i]);
                }
                break;
            case MOTOR_CMD_STAT_BUSY:
                if (MiduMotoCtrPra[i].MotoRunState == MOTOSTOPHALF
                    || MiduMotoCtrPra[i].MotoRunState == MOTOSTOPFERR) {
                    unsigned char zero = MiduMotoCtrPra[i].MotoZeroState[2]<<2 | MiduMotoCtrPra[i].MotoZeroState[1]<<1 | MiduMotoCtrPra[i].MotoZeroState[0];
                    motorcmdreturn(i, cmd->puls, timerTick1ms - cmd->timertick,
                                   zero,cmd->dir,cmd->cmdcode);
                    ringBufPop_noread(&motorcmdbuf[i]);
                }
                break;
            default:
                break;
            }
        }
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




