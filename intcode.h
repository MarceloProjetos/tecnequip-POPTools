#ifndef __INTCODE_H
#define __INTCODE_H

#define INT_SET_BIT                              1
#define INT_CLEAR_BIT                            2
#define INT_COPY_BIT_TO_BIT                      3
#define INT_SET_VARIABLE_TO_LITERAL              4
#define INT_SET_VARIABLE_TO_VARIABLE             5   
#define INT_INCREMENT_VARIABLE                   6
#define INT_SET_VARIABLE_ADD                     7
#define INT_SET_VARIABLE_SUBTRACT                8
#define INT_SET_VARIABLE_MULTIPLY                9
#define INT_SET_VARIABLE_DIVIDE                 10
#define INT_SET_VARIABLE_MODULO                 39

#define INT_READ_ADC                            11
#define INT_SET_PWM                             12
#define INT_UART_SEND                           13
#define INT_UART_RECV                           14
#define INT_EEPROM_BUSY_CHECK                   15
#define INT_EEPROM_READ                         16
#define INT_EEPROM_WRITE                        17
#define INT_READ_ENC                            18
#define INT_RESET_ENC                           19
#define INT_READ_USS							20
#define INT_WRITE_USS							21
#define INT_SET_DA								22
#define INT_READ_MODBUS							23
#define INT_WRITE_MODBUS						24
#define INT_SET_SINGLE_BIT						25
#define INT_CLEAR_SINGLE_BIT					26
#define INT_CHECK_BIT							29
#define INT_READ_FORMATTED_STRING				30
#define INT_WRITE_FORMATTED_STRING				31
#define INT_READ_SERVO_YASKAWA					32
#define INT_WRITE_SERVO_YASKAWA					33
#define INT_CHECK_RTC							34
#define INT_MULTISET_DA							35
#define INT_SQRT								36
#define INT_SET_RTC								37
#define INT_RAND								38

#define INT_IF_GROUP(x) (((x) >= 50) && ((x) < 60))
#define INT_IF_BIT_SET                          50
#define INT_IF_BIT_CLEAR                        51
#define INT_IF_VARIABLE_LES_LITERAL             52
#define INT_IF_VARIABLE_EQUALS_VARIABLE         53
#define INT_IF_VARIABLE_GRT_VARIABLE            54
#define INT_IF_BIT_CHECK_SET					55
#define INT_IF_BIT_CHECK_CLEAR					56

#define INT_ELSE_GROUP(x) (((x) >= 60) && ((x) < 70))
#define INT_ELSE                                60
#define INT_ELSE_IF                             61

#define INT_END_IF                              70

#define INT_SIMULATE_NODE_STATE                 80

#define INT_COMMENT                            100

// Only used for the interpretable code.
#define INT_END_OF_PROGRAM                     255

#if !defined(INTCODE_H_CONSTANTS_ONLY)
    typedef struct IntOpTag {
        int         op;
		char		desc[MAX_NAME_LEN];
        char        name1[MAX_NAME_LEN];
        char        name2[MAX_NAME_LEN];
        char        name3[MAX_NAME_LEN];
        char        name4[MAX_NAME_LEN];
        char        name5[MAX_NAME_LEN];
        char        name6[MAX_NAME_LEN];
		char        name7[MAX_NAME_LEN];
        SWORD       literal;
        BOOL       *poweredAfter;
		unsigned char bit;
    } IntOp;

    #define MAX_INT_OPS     (1024*16)
    extern IntOp IntCode[MAX_INT_OPS];
    extern int IntCodeLen;
#endif


#endif
