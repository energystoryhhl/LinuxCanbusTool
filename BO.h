#ifndef __BO_H__
#define __BO_H__

#define BOFLAG "BO_ "
#define SGFLAG " SG_ "
//#define DEBUG       //show BO something....
//#define DEBUGLINE
//#define DEBUGPARSE  //start size offset /..debug inf

typedef enum bit_order_t{motorola,intel} bit_order;
typedef enum val_type_t{unsigned_value,signed_value}VALUE_TYPE;

typedef struct SG_{
    char signal_name[50];
    char multiplexer_indicator;
    int start_bit;
    unsigned int signal_size;
    bit_order Bit_order;             //0 :motorola 1:intel
    VALUE_TYPE val_type;             //+:unsigned(0) -:signed(1)
    double facator;
    double offset;
    double minimum;
    double maximum;
    char unit[20];
    char receiver[20];
    struct SG_ *next;               //NOT USED
    double value;
    double value_send;
}SG_t;

//struct of every msg
typedef struct BO_Unit{
    unsigned int message_id;        //msg id 0x123....
    unsigned int message_size;      //DLC MAX 8byte
    char  transmitter[50];
    char  message_name[100];
    SG_t *First_SG;
    struct SG_ *SG_List[50];
}BO_Unit_t;
BO_Unit_t * BO_List[50];        //dbc list
#endif
