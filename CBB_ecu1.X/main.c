#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000

typedef enum{
    ST_DEFAULT,
    ST_LOGIN,
    ST_MENU,
    ST_VIEW_LOG,
    ST_SET_TIME
}state_t;


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


typedef struct
{
    unsigned char event;
    unsigned char gear;
    unsigned char seat;
    unsigned char brake;
    unsigned char door;
    unsigned char engine;
    unsigned char collision;
    unsigned char hh, mm, ss;
    unsigned char spd[3];
} Event;


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
#define UP                  MK_SW1
#define DOWN                MK_SW2
#define OK                  MK_SW3
#define BACK                MK_SW10
#define K1                  MK_SW11
#define K2                  MK_SW12


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

// rtc
#define SLAVE_READ		0xD1
#define SLAVE_WRITE		0xD0

#define SEC_ADDR		0x00
#define MIN_ADDR		0x01
#define HOUR_ADDR		0x02
#define DAY_ADDR		0x03
#define DATE_ADDR		0x04
#define MONTH_ADDR		0x05
#define YEAR_ADDR		0x06
#define CNTL_ADDR		0x07

void init_ds1307(void);
void write_ds1307(unsigned char address1,  unsigned char data);
unsigned char read_ds1307(unsigned char address1);

#define RTC_ADDR 0x68
unsigned char can_speed[3];
unsigned char can_status[6];
unsigned char key;
unsigned char is_collision = 0,menu = 0,event_idx = 0,field = 0;
state_t state = ST_DEFAULT;
unsigned char sec, min, hour;
unsigned long lcd_update_ms = 0,ms = 0,last_activity_ms = 0;
unsigned char logging_enabled = 1;
unsigned char speed_rx = 0;
unsigned char status_rx = 0;
static unsigned char event_no = 0;
//eeprom
#define EEPROM_W  0xA0
#define EEPROM_R  0xA1
#define EVENT_SIZE     13
#define MAX_EVENTS     10

#define PASSWORD_LEN 4
const unsigned char password[PASSWORD_LEN] = {K1, K2, K1, K2};

unsigned char pwd_buf[PASSWORD_LEN];
unsigned char pwd_idx = 0;
unsigned char attempts_left = 3;

unsigned long last_key_ms = 0;
unsigned long lock_start_ms = 0;
unsigned char login_locked = 0;

unsigned char blink = 1;
unsigned long blink_ms = 0;

void init_i2c(void)
{
	/* Set SCL and SDA pins as inputs */
	TRISC3 = 1;
	TRISC4 = 1;
	/* Set I2C master mode */
	SSPCON1 = 0x28;

	SSPADD = 0x31;
	/* Use I2C levels, worked also with '0' */
	CKE = 0;
	/* Disable slew rate control  worked also with '0' */
	SMP = 1;
	/* Clear SSPIF interrupt flag */
	SSPIF = 0;
	/* Clear bus collision flag */
	BCLIF = 0;
}

void i2c_idle(void)
{
	while (!SSPIF);
	SSPIF = 0;
}

void i2c_ack(void)
{
	if (ACKSTAT)
	{
		/* Do debug print here if required */
	}
}

void i2c_start(void)
{
	SEN = 1;
	i2c_idle();
}

void i2c_stop(void)
{
	PEN = 1;
	i2c_idle();
}

void i2c_rep_start(void)
{
	RSEN = 1;
	i2c_idle();
}

void i2c_write(unsigned char data)
{
	SSPBUF = data;
	i2c_idle();
}

void i2c_rx_mode(void)
{
	RCEN = 1;
	i2c_idle();
}

void i2c_no_ack(void)
{
	ACKDT = 1;
	ACKEN = 1;
}

unsigned char i2c_read(void)
{
	i2c_rx_mode();
	i2c_no_ack();

	return SSPBUF;
}
void eeprom_write_byte(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_write(EEPROM_W);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
    __delay_ms(5);
}

unsigned char eeprom_read_byte(unsigned char addr)
{
    unsigned char data;
    i2c_start();
    i2c_write(EEPROM_W);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(EEPROM_R);
    data = i2c_read();
    i2c_stop();
    return data;
}
void eeprom_write_event(unsigned char index, Event *e)
{
    unsigned char addr = index * EVENT_SIZE;

    eeprom_write_byte(addr++, e->event + '0');
    eeprom_write_byte(addr++, e->gear);
    eeprom_write_byte(addr++, e->seat + '0');
    eeprom_write_byte(addr++, e->brake + '0');
    eeprom_write_byte(addr++, e->door + '0');
    eeprom_write_byte(addr++, e->engine + '0');
    eeprom_write_byte(addr++, e->collision + '0');

    eeprom_write_byte(addr++, e->hh);
    eeprom_write_byte(addr++, e->mm);
    eeprom_write_byte(addr++, e->ss);

    eeprom_write_byte(addr++, e->spd[0]);
    eeprom_write_byte(addr++, e->spd[1]);
    eeprom_write_byte(addr++, e->spd[2]);
}

void eeprom_read_event(unsigned char index, Event *e)
{
    unsigned char addr = index * EVENT_SIZE;

    e->event     = eeprom_read_byte(addr++) - '0';
    e->gear      = eeprom_read_byte(addr++);
    e->seat      = eeprom_read_byte(addr++) - '0';
    e->brake     = eeprom_read_byte(addr++) - '0';
    e->door      = eeprom_read_byte(addr++) - '0';
    e->engine    = eeprom_read_byte(addr++) - '0';
    e->collision = eeprom_read_byte(addr++) - '0';

    e->hh = eeprom_read_byte(addr++);
    e->mm = eeprom_read_byte(addr++);
    e->ss = eeprom_read_byte(addr++);

    e->spd[0] = eeprom_read_byte(addr++);
    e->spd[1] = eeprom_read_byte(addr++);
    e->spd[2] = eeprom_read_byte(addr++);
}

void eeprom_shift_events(void)
{
    Event temp;
    for (char i = MAX_EVENTS - 1; i > 0; i--)
    {
        eeprom_read_event(i - 1, &temp);
        eeprom_write_event(i, &temp);
    }
}

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
void init_ds1307(void)
{
//	unsigned char dummy;

	/* Setting the CH bit of the RTC to Stop the Clock */
//	dummy = read_ds1307(SEC_ADDR);
	write_ds1307(SEC_ADDR,0xC5);
    
    // for minutes
//    dummy = read_ds1307(0x01);
	write_ds1307(0x01,0x43);

	/* Seting 24 Hr Format */
//	dummy = read_ds1307(HOUR_ADDR);
	write_ds1307(HOUR_ADDR,0x17); 

	/* 
	 * Control Register of DS1307
	 * Bit 7 - OUT
	 * Bit 6 - 0
	 * Bit 5 - OSF
	 * Bit 4 - SQWE
	 * Bit 3 - 0
	 * Bit 2 - 0
	 * Bit 1 - RS1
	 * Bit 0 - RS0
	 * 
	 * Seting RS0 and RS1 as 11 to achive SQW out at 32.768 KHz
	 */ 
	write_ds1307(CNTL_ADDR, 0x93); 

	/* Clearing the CH bit of the RTC to Start the Clock */
//	dummy = read_ds1307(SEC_ADDR);
	write_ds1307(SEC_ADDR,0x45); 

}

void write_ds1307(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
}

unsigned char read_ds1307(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ);
	data = i2c_read();
	i2c_stop();

	return data;
}
unsigned char bcd_to_dec(unsigned char b)
{
    return ((b >> 4) * 10) + (b & 0x0F);
}
unsigned char dec2bcd(unsigned char d){
    return(((unsigned char)(d/10))<<4)|((unsigned char)(d%10));
}
void rtc_write_time(unsigned char hh, unsigned char mm, unsigned char ss){
    i2c_start();
    i2c_write((RTC_ADDR<<1)|0);
    i2c_write(0x00);
    i2c_write(dec2bcd(ss));
    i2c_write(dec2bcd(mm));
    i2c_write(dec2bcd(hh));
    i2c_stop();
}

void init_config(){
    init_i2c();
    init_clcd();
    init_matrix_keypad();
    init_can();
    init_ds1307();
}

void delay(unsigned short factor)
{
    unsigned short i, j;
    for (i = 0; i < factor; i++)
    {
        for (j = 500; j--; );
    }
}
void can_task(void)
{
    if (RXB0FUL)
    {
        unsigned char dlc = RXB0DLC;

        if (dlc == 3)
        {
            can_speed[0] = RXB0D0;
            can_speed[1] = RXB0D1;
            can_speed[2] = RXB0D2;
            speed_rx = 1;
        }
        else if (dlc == 6)
        {
            if (RXB0D0 == 'N' || RXB0D0 == 'R' || (RXB0D0 >= '1' && RXB0D0 <= '5'))
            {
                can_status[0] = RXB0D0;
                can_status[1] = RXB0D1;
                can_status[2] = RXB0D2;
                can_status[3] = RXB0D3;
                can_status[4] = RXB0D4;
                can_status[5] = RXB0D5;
                status_rx = 1;
            }
        }

        RXB0FUL = 0;
    }
}
void log_event_conditionally(Event *e)
{
    if (logging_enabled == 0)
        return;

    eeprom_shift_events();

    e->event = event_no;
    eeprom_write_event(0, e);

    event_no++;
    if (event_no > 9)
        event_no = 0;

    if (e->collision == 1)
    {
        logging_enabled = 0;
        is_collision = 1;
    }
}
void screen_default(){
        sec = bcd_to_dec(read_ds1307(SEC_ADDR) & 0x7F);
        min = bcd_to_dec(read_ds1307(MIN_ADDR));
        hour = bcd_to_dec(read_ds1307(HOUR_ADDR) & 0x3F);
        Event e;
        eeprom_read_event(0, &e);
        CLEAR_DISP_SCREEN;
        unsigned char b[17]; 
        sprintf(b,"TIME      E    SP");
        clcd_print(b,LINE1(0));
        sprintf(b,"%02d:%02d:%02d %s %c%c%c",hour,min,sec,(can_status[4]=='1') ? "ENG ON " : "ENG OFF",
        can_speed[0], can_speed[1], can_speed[2]);
        clcd_print(b,LINE2(0));
}
void screen_login(unsigned char stars)
{
    unsigned char i;
    CLEAR_DISP_SCREEN;
    clcd_print("ENTER PASSWORD", LINE1(0));
    for(i = 0; i < stars; i++)
        clcd_putch('*', LINE2(i));
}
void screen_menu(){
    CLEAR_DISP_SCREEN;
    clcd_print(menu==0?">VIEW LOG":" VIEW LOG", LINE1(0));
    clcd_print(menu==1?">SET TIME":" SET TIME", LINE2(0));
}
void screen_set_time(){
    unsigned char buf[17];
    if (ms - blink_ms >= 500){
        blink ^= 1;
        blink_ms = ms;
    }
    CLEAR_DISP_SCREEN;
    clcd_print("SET TIME", LINE1(0));
    if (field == 0){
        if (blink) sprintf(buf, "  :%02d", min);
        else sprintf(buf, "%02d:%02d", hour, min);
    }
    else{
        if (blink) sprintf(buf, "%02d:  ", hour);
        else sprintf(buf, "%02d:%02d", hour, min);
    }
    clcd_print(buf, LINE2(4));
}
void screen_view_log(void){
    Event e;
    unsigned char buf[17];
    unsigned char display_idx = event_idx;
    eeprom_read_event(display_idx, &e);
    sprintf(buf, "# GSBDC %s %c%c%c",e.engine-'0' ? "ON" : "OFF",e.spd[0],e.spd[1],e.spd[2]);
    CLEAR_DISP_SCREEN;
    clcd_print(buf, LINE1(0));
    sprintf(buf, "%c %c%c%c%c%c %02d:%02d:%02d",e.event,e.gear,e.seat,e.brake,e.door,e.collision,e.hh,e.mm,e.ss);
    clcd_print(buf, LINE2(0));
}
void main(void){   
    init_config();
    while (1){
        key = read_switches(STATE_CHANGE);
        if (key != 0xFF)
        {
            last_activity_ms = ms;
        }
        can_task();
        sec = bcd_to_dec(read_ds1307(SEC_ADDR) & 0x7F);
        min = bcd_to_dec(read_ds1307(MIN_ADDR));
        hour = bcd_to_dec(read_ds1307(HOUR_ADDR) & 0x3F);
        if (speed_rx && status_rx){
            Event e;
            e.gear      = can_status[0];
            e.seat      = can_status[1] - '0';
            e.brake     = can_status[2] - '0';
            e.door      = can_status[3] - '0';
            e.engine    = can_status[4] - '0';
            e.collision = can_status[5] - '0';
            e.hh = hour;
            e.mm = min;
            e.ss = sec;
            e.spd[0] = can_speed[0];
            e.spd[1] = can_speed[1];
            e.spd[2] = can_speed[2];
            log_event_conditionally(&e);
            speed_rx = status_rx = 0; 
        }
        if (is_collision == 1)
        {
            logging_enabled = 0;
            CLEAR_DISP_SCREEN; 
            clcd_print("COLLISION HAPPENED", LINE1(0));
            clcd_print("ENGINE OFF", LINE2(0));
            continue;
        }
        else{
            if (state == ST_MENU || state == ST_VIEW_LOG || state == ST_SET_TIME)
            {
                if ((ms - last_activity_ms) >= 5000)
                {
                    CLEAR_DISP_SCREEN;
                    state = ST_DEFAULT;
                    menu = 0;
                    event_idx = 0;
                    pwd_idx = 0;
                }
            }
            switch(state){
                case ST_DEFAULT:
                    if(key == UP || key == DOWN){
                        CLEAR_DISP_SCREEN;
                        state = ST_LOGIN;
                        menu = 0;
                    }
                    break;
                case ST_LOGIN:
                    if (login_locked){
                        clcd_print("LOGIN LOCKED", LINE1(0));
                        clcd_print("WAIT 3 MIN", LINE2(0));
                        if (ms - lock_start_ms >= 180000){
                            login_locked = 0;
                            attempts_left = 3;
                            CLEAR_DISP_SCREEN;
                        }
                        break;
                    }
                    if (pwd_idx > 0 && (ms - last_key_ms) >= 3000){
                        pwd_idx = 0;
                        CLEAR_DISP_SCREEN;
                        state = ST_DEFAULT;
                        break;
                    }
                    if (key == K1 || key == K2){
                        pwd_buf[pwd_idx++] = key;
                        last_key_ms = ms;
                        screen_login(pwd_idx);
                        if (pwd_idx == PASSWORD_LEN){
                            unsigned char match = 1;
                            for (unsigned char i = 0; i < PASSWORD_LEN; i++){
                                if (pwd_buf[i] != password[i]){
                                    match = 0;
                                    break;
                                }
                            }
                            pwd_idx = 0;
                            if (match){
                                last_activity_ms = ms;
                                CLEAR_DISP_SCREEN;
                                state = ST_MENU;
                            }
                            else{
                                attempts_left--;
                                CLEAR_DISP_SCREEN;
                                clcd_print("WRONG PASSWORD", LINE1(0));
                                if (attempts_left == 0){
                                    login_locked = 1;
                                    lock_start_ms = ms;
                                }
                            }
                        }
                    }
                    /* Cancel login */
                    if (key == BACK){
                        pwd_idx = 0;
                        CLEAR_DISP_SCREEN;
                        state = ST_DEFAULT;
                        menu = 0;
                        event_idx = 0;
                        pwd_idx = 0;
                    }
                    break;
                case ST_MENU:
                    if(key == UP && menu > 0) menu--;
                    else if(key == DOWN && menu < 1) menu++;
                    else if(key == OK){
                        CLEAR_DISP_SCREEN;
                        state = (menu == 0) ? ST_VIEW_LOG : ST_SET_TIME;
                        menu = 0;
                    }
                    else if(key == BACK){
                        CLEAR_DISP_SCREEN;
                        state = ST_DEFAULT;
                        menu = 0;
                        event_idx = 0;
                        pwd_idx = 0;
                    }
                    break;
                case ST_VIEW_LOG:
                    if(key == UP){
                        event_idx++;
                        if(event_idx >= MAX_EVENTS) event_idx = 0;
                    }
                    else if(key == DOWN){
                        if(event_idx == 0) event_idx = MAX_EVENTS - 1;
                        else event_idx--;
                    }
                    else if(key == OK){
                            CLEAR_DISP_SCREEN;
                            state = ST_MENU;
                        }
                    else if(key == BACK){
                        CLEAR_DISP_SCREEN;
                        state = ST_DEFAULT;
                        menu = 0;
                        event_idx = 0;
                        pwd_idx = 0;
                    }
                    break;
                case ST_SET_TIME:
                    if(key == DOWN){
                        field = (field == 0) ? 1 : 0;
                    }
                    else if(key == UP){
                        if(field == 0){
                            hour++;
                            if(hour > 23) hour = 0;
                        }
                        else{
                            min++;
                            if(min > 59) min = 0;
                        }
                    }
                    else if(key == OK){
                        rtc_write_time(hour, min, 0);
                        CLEAR_DISP_SCREEN;
                        state = ST_DEFAULT;
                        menu = 0;
                    }
                    else if(key == BACK){
                        CLEAR_DISP_SCREEN;
                        state = ST_DEFAULT;
                        menu = 0;
                        event_idx = 0;
                        pwd_idx = 0;
                    }
                    break;
                }
            }
        if(ms - lcd_update_ms >= 100){
            lcd_update_ms = ms;
            
            switch(state){
                case ST_DEFAULT: screen_default(); break;
                case ST_SET_TIME: screen_set_time(); break;
                case ST_MENU: screen_menu(); break;
                case ST_VIEW_LOG: screen_view_log(); break;
            }
        }
        ms++;
        __delay_ms(1);
    }
}