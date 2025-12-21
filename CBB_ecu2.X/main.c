#include <xc.h>
#include <stdio.h>
// i2c
void init_i2c(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(void);

// clcd
#define CLCD_PORT			PORTD
#define CLCD_EN				RC2
#define CLCD_RS				RC1
#define CLCD_RW				RC0
#define CLCD_BUSY			RD7
#define PORT_DIR			TRISD7
#define HI												1
#define LO												0
#define INPUT											0xFF
#define OUTPUT											0x00
#define DATA_COMMAND									1
#define INSTRUCTION_COMMAND								0
#define _XTAL_FREQ                  20000000
#define LINE1(x)									(0x80 + (x))
#define LINE2(x)										(0xC0 + (x))
#define TWO_LINE_5x8_MATRIX_8_BIT					clcd_write(0x38, INSTRUCTION_COMMAND)
#define CLEAR_DISP_SCREEN				                clcd_write(0x01, INSTRUCTION_COMMAND)
#define CURSOR_HOME							clcd_write(0x02, INSTRUCTION_COMMAND)
#define DISP_ON_AND_CURSOR_OFF						clcd_write(0x0C, INSTRUCTION_COMMAND)
#define EIGHT_BIT_MODE   0x33
void init_clcd(void);
void clcd_print(const unsigned char *data, unsigned char addr);
void clcd_putch(const unsigned char data, unsigned char addr);
void clcd_write(unsigned char bit_values, unsigned char control_bit);

//clcd
void clcd_write(unsigned char byte, unsigned char control_bit)
{
	CLCD_RS = control_bit;
	CLCD_PORT = byte;

	/* Should be atleast 200ns */
	CLCD_EN = HI;
	CLCD_EN = LO;

	PORT_DIR = INPUT;
	CLCD_RW = HI;
	CLCD_RS = INSTRUCTION_COMMAND;

	do
	{
		CLCD_EN = HI;
		CLCD_EN = LO;
	} while (CLCD_BUSY);

	CLCD_RW = LO;
	PORT_DIR = OUTPUT;
}

void init_clcd()
{
	/* Set PortD as output port for CLCD data */
	TRISD = 0x00;
	/* Set PortC as output port for CLCD control */
	TRISC = TRISC & 0xF8;

	CLCD_RW = LO;

	
     /* Startup Time for the CLCD controller */
    __delay_ms(30);
    
    /* The CLCD Startup Sequence */
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(1); 
    
    CURSOR_HOME;
    __delay_us(100);
    TWO_LINE_5x8_MATRIX_8_BIT;
    __delay_us(100);
    CLEAR_DISP_SCREEN;
    __delay_us(500);
    DISP_ON_AND_CURSOR_OFF;
    __delay_us(100);
}

void clcd_print(const unsigned char *data, unsigned char addr)
{
	clcd_write(addr, INSTRUCTION_COMMAND);
	while (*data != '\0')
	{
		clcd_write(*data++, DATA_COMMAND);
	}
}

void clcd_putch(const unsigned char data, unsigned char addr)
{
	clcd_write(addr, INSTRUCTION_COMMAND);
	clcd_write(data, DATA_COMMAND);
}

void init_adc(void)
{
    /* Selecting right justified ADRES Registers order */
    ADFM = 1;
    
    /* Acquisition time selection bits - Set for 4 Tad */
    ACQT2 = 0;
    ACQT1 = 1;
    ACQT0 = 0;
    
    /* Selecting the conversion clock of Fosc / 32 */
    ADCS0 = 0;
    ADCS1 = 1;
    ADCS2 = 0;
    
    /* Stop the conversion to start with */
    GODONE = 0;
    
    /* Voltage reference bit as VSS */
    VCFG1 = 0;
    /* Voltage reference bit as VDD */
    VCFG0 = 0;
    
    /* Clear ADRESH & ADRESL registers */
    ADRESH = 0;
    ADRESL = 0;
    
    /* Turn ON the ADC module */
    ADON = 1;
}

unsigned short read_adc(unsigned char channel)
{
    unsigned short reg_val;
    
    /* Select the channel */
    ADCON0 = (ADCON0 & 0xC3) | (channel << 2);
    
    /* Start the conversion */
    GO = 1;
    while (GO);
    
    reg_val = (ADRESH << 8) | ADRESL;
    return reg_val;
}

// Matrix Keypad
#define STATE_CHANGE        1
#define LEVEL_CHANGE        0
#define MATRIX_KEYPAD_PORT  PORTB
#define ROW3                PORTBbits.RB7
#define ROW2                PORTBbits.RB6
#define ROW1                PORTBbits.RB5
#define COL4                PORTBbits.RB4
#define COL3                PORTBbits.RB3
#define COL2                PORTBbits.RB2
#define COL1                PORTBbits.RB1
#define MK_SW1              1
#define MK_SW2              2
#define MK_SW3              3
#define MK_SW10             10
#define MK_SW11             11
#define MK_SW12             12
#define MKP_ALL_RELEASED    0xFF
#define HI                  1
#define LO                  0

void init_matrix_keypad(void)
{
    /* Config PORTB as digital */
    ADCON1 = 0x0F;
    
    /* Set Rows (RB7 - RB5) as Outputs and Columns (RB4 - RB1) as Inputs */
    TRISB = 0x1E;
    
    /* Set PORTB input as pull up for columns */
    RBPU = 0;
    
    MATRIX_KEYPAD_PORT = MATRIX_KEYPAD_PORT | 0xE0;
}

unsigned char scan_key(void)
{
    ROW1 = LO;
    ROW2 = HI;
    ROW3 = HI;
    
    if (COL1 == LO)
        return 1;
    else if (COL2 == LO)
        return 4;
    else if (COL3 == LO)
        return 7;
    else if (COL4 == LO)
        return 10;
    
    ROW1 = HI;
    ROW2 = LO;
    ROW3 = HI;
    
    if (COL1 == LO)
        return 2;
    else if (COL2 == LO)
        return 5;
    else if (COL3 == LO)
        return 8;
    else if (COL4 == LO)
        return 11;
    
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = LO;
    ROW3 = LO;
    
    if (COL1 == LO)
        return 3;
    else if (COL2 == LO)
        return 6;
    else if (COL3 == LO)
        return 9;
    else if (COL4 == LO)
        return 12;
    
    return 0xFF;
}

unsigned char read_switches(unsigned char detection_type)
{
    static unsigned char once = 1, key;
    
    if (detection_type == STATE_CHANGE)
    {
        key = scan_key();
        if(key != 0xFF && once)
        {
            once = 0;
            return key;
        }
        else if(key == 0xFF)
        {
            once = 1;
        }
    }
    else if (detection_type == LEVEL_CHANGE)
    {
        return scan_key();
    }
    
    return 0xFF;
}

// can

unsigned char can_payload[13];

// ENUMS
typedef enum _CanOpMode {
    e_can_op_mode_bits    = 0xE0,
    e_can_op_mode_normal  = 0x00,
    e_can_op_mode_sleep   = 0x20,
    e_can_op_mode_loop    = 0x40,
    e_can_op_mode_listen  = 0x60,
    e_can_op_mode_config  = 0x80
} CanOpMode;

// CAN Defines
#define CAN_SET_OPERATION_MODE_NO_WAIT(mode) \
{ \
    CANCON &= 0x1F; \
    CANCON |= mode; \
}

#define EIDH            0
#define EIDL            1
#define SIDH            2
#define SIDL            3
#define DLC             4
#define D0              5
#define D1              6
#define D2              7
#define D3              8
#define D4              9
#define D5              10
#define D6              11
#define D7              12

void init_can(void)
{
    /* CAN_TX = RB2, CAN_RX = RB3 */
    TRISB2 = 0;     /* CAN TX */
    TRISB3 = 1;     /* CAN RX */
    
    /* Enter CAN module into config mode */
    CAN_SET_OPERATION_MODE_NO_WAIT(e_can_op_mode_config);
    
    /* Wait until desired mode is set */
    while (CANSTAT != 0x80);
    
    /* Enter CAN module into Mode 0 */
    ECANCON = 0x00;
    
    /* Initialize CAN Timing 8MHz */
    BRGCON1 = 0xE1;     /* 1110 0001, SJW=4 TQ, BRP 4 */
    BRGCON2 = 0x1B;     /* 0001 1011, SEG2PHTS 1 sampled once PS1=4TQ PropagationT 4TQ */
    BRGCON3 = 0x03;     /* 0000 0011, PS2, 4TQ */
    RXFCON0 = 0x00;
    CAN_SET_OPERATION_MODE_NO_WAIT(e_can_op_mode_normal);
    
    while ((CANSTAT & 0xE0) != 0x00);
    RXB0CON = 0x00;
}

void init_config(){
    init_i2c();
    init_clcd();
    init_adc();
    init_matrix_keypad();
    init_can();
}

// status flags
unsigned char belt_sts = 0;
unsigned char col_sts = 0;
unsigned char brake_sts = 0;
unsigned char engine_sts = 0;
unsigned char door_sts = 0;
unsigned char ind = 0;

unsigned char get_gear(){
    unsigned char key;
    
    key = read_switches(STATE_CHANGE);
    if (key != 0xFF){
        if (key == MK_SW1){   
            ind++;
        }
    }
    if(ind > 6) ind = 0;
    
    if(ind == 0) return 'N';
    else if(ind > 5)return 'R';
    else return ind;
}
unsigned char read_seat_belt(){
    unsigned char key ;
    key = read_switches(STATE_CHANGE);
    if (key != 0xFF){
        if (key == MK_SW2){   
            belt_sts ^= 1;
        }
    }
    return belt_sts; 
}
unsigned char read_door_lock(){
    unsigned char key ;
    key = read_switches(STATE_CHANGE);
    if (key != 0xFF){
        if (key == MK_SW10){   
            door_sts ^= 1;
        }
    }
    return door_sts; 
}
unsigned char read_engine(){
    unsigned char key ;
    key = read_switches(STATE_CHANGE);
    if (key != 0xFF){
        if (key == MK_SW11){   
            engine_sts ^= 1;
        }
    }
    return engine_sts; 
}
unsigned char read_col(){
    unsigned char key ;
    key = read_switches(STATE_CHANGE);
    if (key != 0xFF){
        if (key == MK_SW12){   
            col_sts ^= 1;
        }
    }
    return col_sts; 
}
unsigned char read_brake(){
    unsigned char key ;
    key = read_switches(STATE_CHANGE);
    if (key != 0xFF){
        if (key == MK_SW3){   
            brake_sts ^= 1;
        }
    }
    return brake_sts; 
}

void delay(unsigned short factor)
{
    unsigned short i, j;
    for (i = 0; i < factor; i++)
    {
        for (j = 500; j--; );
    }
}

void can_transmit(unsigned char * buf, int size){
    TXB0EIDH = 0x00;
        TXB0EIDL = 0x00;
        /* ========== CHANGE 6: SAME AS CHANGE 3 ========== */
        // ORIGINAL: TXB0SIDH = 0x6c;
        // ORIGINAL: TXB0SIDL = 0xC1;
        TXB0SIDH = 0x6D;  // For 0x36E
        TXB0SIDL = 0xC0;

        TXB0DLC = size;

        TXB0D0 = buf[0];
        TXB0D1 = buf[1];
        TXB0D2 = buf[2];
        TXB0D3 = buf[3];
        TXB0D4 = buf[4];
        TXB0D5 = buf[5];
        TXB0D6 = buf[6];
        TXB0D7 = buf[7];

        TXB0REQ = 1;
        while(TXB0REQ);
}

void main(void)
{
    unsigned char door_lock = 0, seat_belt = 0, brake = 0, engine = 0, col = 0;
    unsigned char gear = 'N';
    unsigned short speed = 0;
    unsigned char buf[8];
    

    init_config();

    while (1)
    {
        speed = read_adc(0x04);

        gear = get_gear();
        seat_belt = read_seat_belt();
        brake = read_brake();
        door_lock = read_door_lock();
        engine = read_engine();
        col = read_col();

        if (col == 1)
        {
            engine_sts = 0;
            engine = 0;
        }

        if (engine == 0)
        {
            belt_sts = 0;
            col_sts = 0;
            brake_sts = 0;
            engine_sts = 0;
            door_sts = 0;
            ind = 0;
            gear = 'N';
        }

        

        buf[0] = (speed / 100) + '0';
        buf[1] = ((speed / 10) % 10) + '0';
        buf[2] = (speed % 10) + '0';
        can_transmit(buf, 3);
        delay(10);

        buf[0] = gear;
        buf[1] = seat_belt + '0';
        buf[2] = brake + '0';
        buf[3] = door_lock + '0';
        buf[4] = engine + '0';
        buf[5] = col + '0';
        can_transmit(buf, 6);
        delay(10);
    }
}