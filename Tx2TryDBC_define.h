#define MASTER_ESA_CheckSum   BO_List[0]->SG_List[0]->value
#define MASTER_REQ_CheckSum   BO_List[0]->SG_List[1]->value
#define New_Signal_263   BO_List[0]->SG_List[2]->value
#define New_Signal_262   BO_List[0]->SG_List[3]->value
#define VehV_a   BO_List[0]->SG_List[4]->value

#define MASTER_ESA_TorqueLimitation   BO_List[1]->SG_List[0]->value
#define MASTER_ESA_Speedgradient   BO_List[1]->SG_List[1]->value
#define MASTER_ESA_EndSpeed   BO_List[1]->SG_List[2]->value
#define MASTER_ESA_DurationMax   BO_List[1]->SG_List[3]->value
#define MASTER_DIS_ExMaxCurrent   BO_List[1]->SG_List[4]->value
#define MASTER_ESA_RollingCounter   BO_List[1]->SG_List[5]->value
#define MASTER_Generator_RegVoltage   BO_List[1]->SG_List[6]->value
#define MASTER_Generator_TrqLimitation   BO_List[1]->SG_List[7]->value

#define SCU_Negative_Trq_Prediction_10s   BO_List[2]->SG_List[0]->value
#define SCU_PREDICTION_CheckSum   BO_List[2]->SG_List[1]->value
#define SCU_PREDICTION_RollingCounter   BO_List[2]->SG_List[2]->value
#define SCU_Positive_Trq_Prediction_10s   BO_List[2]->SG_List[3]->value

#define SCU_12V_VOLTAGE_LOW   BO_List[3]->SG_List[0]->value
#define SCU_REVERSE_SPEED_FAULT   BO_List[3]->SG_List[1]->value
#define SCU_12V_VOLTAGE_HIGH   BO_List[3]->SG_List[2]->value
#define SCU_BUSOFF_FAULT   BO_List[3]->SG_List[3]->value
#define SCU_UNLOCK_PTL_FAULT   BO_List[3]->SG_List[4]->value
#define SCU_DIAG_RollingCounter   BO_List[3]->SG_List[5]->value
#define SCU_DIAG_CheckSum   BO_List[3]->SG_List[6]->value
#define SCU_BRUSH_WEAR_FAULT   BO_List[3]->SG_List[7]->value
#define SCU_EXC_OPENCIRCUIT_FAULT   BO_List[3]->SG_List[8]->value
#define SCU_EXC_LATENT_FAULT   BO_List[3]->SG_List[9]->value
#define SCU_EXC_HSM_FAULT   BO_List[3]->SG_List[10]->value
#define SCU_CAN_COM_ERROR   BO_List[3]->SG_List[11]->value
#define SCU_DC_VOLTAGEMEAS_SENSOR_FAULT   BO_List[3]->SG_List[12]->value
#define SCU_PHX_CURRENT_SENSOR_FAULT   BO_List[3]->SG_List[13]->value
#define SCU_MACHINE_TEMP_SENSOR_FAULT   BO_List[3]->SG_List[14]->value
#define SCU_INVERTER_TEMP_SENSOR_FAULT   BO_List[3]->SG_List[15]->value
#define SCU_POSITION_SENSORS_FAULT   BO_List[3]->SG_List[16]->value
#define SCU_STATOR_FAULT   BO_List[3]->SG_List[17]->value
#define SCU_INVERTER_FAULT   BO_List[3]->SG_List[18]->value
#define SCU_TORQUE_CTRL_TIMEOUT   BO_List[3]->SG_List[19]->value
#define SCU_TORQUE_SIGN_FAULT   BO_List[3]->SG_List[20]->value
#define SCU_TORQUE_LEVEL_FAULT   BO_List[3]->SG_List[21]->value
#define SCU_ROTOR_OVERCURRENT   BO_List[3]->SG_List[22]->value
#define SCU_STATOR_OVERCURRENT   BO_List[3]->SG_List[23]->value
#define SCU_LOADDUMP   BO_List[3]->SG_List[24]->value
#define SCU_OVERVOLTAGE   BO_List[3]->SG_List[25]->value
#define SCU_MOSFET_JUNCTION_PROTECTION   BO_List[3]->SG_List[26]->value
#define SCU_OVERHEATING   BO_List[3]->SG_List[27]->value
#define SCU_STARTER_WARNING   BO_List[3]->SG_List[28]->value
#define SCU_THERMAL_DERATING   BO_List[3]->SG_List[29]->value
#define SCU_STALLED_ROTOR_PROTECTION   BO_List[3]->SG_List[30]->value
#define SCU_TORQUE_SUSPENSION   BO_List[3]->SG_List[31]->value

#define SCU_FUNCTIONAL_RollingCounter   BO_List[4]->SG_List[0]->value
#define SCU_FUNCTIONAL_CheckSum   BO_List[4]->SG_List[1]->value
#define SCU_Positive_Trq_Prediction_1s   BO_List[4]->SG_List[2]->value
#define SCU_Negative_Trq_Prediction_1s   BO_List[4]->SG_List[3]->value
#define SCU_IDC_Current   BO_List[4]->SG_List[4]->value
#define SCU_Estimated_Torque   BO_List[4]->SG_List[5]->value

#define MASTER_REQ_RollingCounter   BO_List[5]->SG_List[0]->value
#define MASTER_TRQCTRL_POS_TRQ_LIM   BO_List[5]->SG_List[1]->value
#define MASTER_TRQCTRL_NEG_TRQ_LIM   BO_List[5]->SG_List[2]->value
#define MASTER_NEGATIVE_TORQUE_GRADIENT   BO_List[5]->SG_List[3]->value
#define MASTER_POSITIVE_TORQUE_GRADIENT   BO_List[5]->SG_List[4]->value
#define MASTER_TorqueCtrl_RegTorque   BO_List[5]->SG_List[5]->value
#define MASTER_MM_REQUEST   BO_List[5]->SG_List[6]->value

#define SCU_STATUS_RollingCounter   BO_List[6]->SG_List[0]->value
#define SCU_STATUS_CheckSum   BO_List[6]->SG_List[1]->value
#define CRANK_POSSIBLE_IF_STOPPED   BO_List[6]->SG_List[2]->value
#define SCU_ENA_STATUS   BO_List[6]->SG_List[3]->value
#define iBSG_Temperature_Rate   BO_List[6]->SG_List[4]->value
#define SCU_Rotor_Speed   BO_List[6]->SG_List[5]->value
#define SCU_BUS_VOLTAGE   BO_List[6]->SG_List[6]->value
#define SCU_MM_STATUS   BO_List[6]->SG_List[7]->value

