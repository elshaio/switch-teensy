#include "Joystick.h"

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	X,
	Y,
	A,
	B,
	L,
	R,
	HOME,
	NOTHING,
	TRIGGERS
} Buttons_t;

typedef struct {
	Buttons_t button;
	uint16_t duration;
} command;

typedef struct {
  long year;
  long month;
  long day;
} sdate;

sdate diaactual;
sdate diafinal;

static const int YEAR = 9;
static const int MONTH = 5;
static const int DAY = 1;

static const command movedate[] = {
	{ UP,         5 },		{ NOTHING,  5 },
	{ RIGHT,      5 },		{ NOTHING,  5 },
	{ UP,         5 },		{ NOTHING,  5 },
	{ RIGHT,      5 },		{ NOTHING,  5 },
	{ UP,         5 },		{ NOTHING,  5 },
};

static const command step[] = {
	// Setup controller
						{ NOTHING,  100 },
	{ TRIGGERS,   5 },	{ NOTHING,  50 },
	{ TRIGGERS,   5 },	{ NOTHING,  50 },
	{ A,          5 },	{ NOTHING,  50 },
	// Open game 
	{ HOME,       5 },	{ NOTHING,  50 },
	{ A,          5 },	{ NOTHING,  50 },
	// Set Day
	{ HOME,       5 },		{ NOTHING,  80 },
	{ DOWN,       5 },		{ NOTHING,  2 },
	{ RIGHT,      5 },		{ NOTHING,  1 },
	{ RIGHT,      5 },		{ NOTHING,  1 },
	{ RIGHT,      5 },		{ NOTHING,  1 },
	{ RIGHT,      5 },		{ NOTHING,  1 },
	{ A,          5 },		{ NOTHING,  40 },
	{ DOWN,       80 },		{ NOTHING,  10 },
	{ A,          5 },		{ NOTHING,  20 },
	{ DOWN,       5 },		{ NOTHING,  5 },
	{ DOWN,       5 },		{ NOTHING,  5 },
	{ DOWN,       5 },		{ NOTHING,  5 },
	{ DOWN,       5 },		{ NOTHING,  5 },
	{ A,          5 },		{ NOTHING,  20 },
	{ DOWN,       5 },		{ NOTHING,  5 },
	{ DOWN,       5 },		{ NOTHING,  5 },
	{ A,          5 },		{ NOTHING,  20 },
	// HERE IS THE DAY CHANGE, COMMENT IS FOR ONE DAY
	// { UP,         5 },		{ NOTHING,  5 },
	{ RIGHT,      25 },		{ NOTHING,  5 },
	{ A,          5 },		{ NOTHING,  5 },
	{ HOME,       5 },		{ NOTHING,  80 },
	{ A,          5 },		{ NOTHING,  100},
};

int main(void) {
	diaactual.year = 2020;
	diaactual.month = 2;
	diaactual.day = 3;

	diafinal.year = 2020;
	diafinal.month = 3;
	diafinal.year = 1;
	SetupHardware();
	GlobalInterruptEnable();
 	for (;;) {
		HID_Task();
		USB_USBTask();
	}
}

void SetupHardware(void) {
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	clock_prescale_set(clock_div_1);
	#ifdef ALERT_WHEN_DONE
	#warning LED and Buzzer functionality enabled. All pins on both PORTB and \
PORTD will toggle when printing is done.
	DDRD  = 0xFF; //Teensy uses PORTD
	PORTD =  0x0;
	DDRB  = 0xFF; //uses PORTB. Micro can use either or, but both give us 2 LEDs
	PORTB =  0x0; //The ATmega328P on the UNO will be resetting, so unplug it?
	#endif
	USB_Init();
}

void EVENT_USB_Device_Connect(void) {
}

void EVENT_USB_Device_Disconnect(void) {
}

void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
}

void EVENT_USB_Device_ControlRequest(void) {
}

void HID_Task(void) {
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	if (Endpoint_IsOUTReceived()) {
		if (Endpoint_IsReadWriteAllowed()) {
			USB_JoystickReport_Output_t JoystickOutputData;
			while(Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		}
		Endpoint_ClearOUT();
	}
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	if (Endpoint_IsINReady()) {
		USB_JoystickReport_Input_t JoystickInputData;
		GetNextReport(&JoystickInputData);
		while(Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		Endpoint_ClearIN();
	}
}

typedef enum {
	SYNC_CONTROLLER,
	SYNC_POSITION,
	BREATHE,
	PROCESS,
	CLEANUP,
	CHANGEDAY,
	VERIFYDAY,
	DONE
} State_t;
State_t state = SYNC_CONTROLLER;

#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;

int report_count = 0;
int xpos = 0;
int ypos = 0;
int bufindex = 0;
int dateindex = 0;
int duration_count = 0;
int portsval = 0;
int movetype = 0;

sdate tmpdate;

long date2number(sdate date) {
  long  y, m;
  m = (date.month + 9)%12;
  y = date.year - m/10;
  return y*365 + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (date.day - 1);
}

sdate number2date(long d) {
  sdate pd;
  long y, ddd, mm, dd, mi;

  y = (10000*d + 14780)/3652425;
  ddd = d - (y*365 + y/4 - y/100 + y/400);
  if (ddd < 0) {
    y--;
    ddd = d - (y*365 + y/4 - y/100 + y/400);
  }
  mi = (52 + 100*ddd)/3060;
  pd.year = y + (mi + 2)/12;
  pd.month = (mi + 2)%12 + 1;
  pd.day = ddd - (mi*306 + 5)/10 + 1;
  return pd;
}

void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
	ReportData->LX = STICK_CENTER;
	ReportData->LY = STICK_CENTER;
	ReportData->RX = STICK_CENTER;
	ReportData->RY = STICK_CENTER;
	ReportData->HAT = HAT_CENTER;
 	if (echoes > 0) {
		memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
		echoes--;
		return;
	}
	switch (state) {
		case SYNC_CONTROLLER:
			state = BREATHE;
			break;
		case SYNC_POSITION:
			bufindex = 0;
			ReportData->Button = 0;
			ReportData->LX = STICK_CENTER;
			ReportData->LY = STICK_CENTER;
			ReportData->RX = STICK_CENTER;
			ReportData->RY = STICK_CENTER;
			ReportData->HAT = HAT_CENTER;
			state = BREATHE;
			break;
		case BREATHE:
			state = PROCESS;
			break;
		case PROCESS:
			switch (step[bufindex].button) {
				case UP:
					ReportData->LY = STICK_MIN;				
					break;
				case LEFT:
					ReportData->LX = STICK_MIN;				
					break;
				case DOWN:
					ReportData->LY = STICK_MAX;				
					break;
				case RIGHT:
					ReportData->LX = STICK_MAX;				
					break;
				case A:
					ReportData->Button |= SWITCH_A;
					break;
				case B:
					ReportData->Button |= SWITCH_B;
					break;
				case R:
					ReportData->Button |= SWITCH_R;
					break;
				case HOME:
					ReportData->Button |= SWITCH_HOME;
					break;
				case TRIGGERS:
					ReportData->Button |= SWITCH_L | SWITCH_R;
					break;
				default:
					ReportData->LX = STICK_CENTER;
					ReportData->LY = STICK_CENTER;
					ReportData->RX = STICK_CENTER;
					ReportData->RY = STICK_CENTER;
					ReportData->HAT = HAT_CENTER;
					break;
			}
			duration_count++;
			if (duration_count > step[bufindex].duration) {
				bufindex++;
				duration_count = 0;
				
				if (bufindex == 45) {
					state = VERIFYDAY;
				}
			}
			if (bufindex > (int)( sizeof(step) / sizeof(step[0])) - 1) {
				if (diaactual.year == diafinal.year && diaactual.month == diafinal.month && diaactual.day == diafinal.day) {
					state = CLEANUP;
				} else {
					bufindex = 11;
					duration_count = 0;
					state = BREATHE;
				}
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
			}
			break;
		case VERIFYDAY:
		    tmpdate = number2date(date2number(diaactual) + 1);
		    if (tmpdate.year != diaactual.year) {
          		movetype = YEAR;
        	} else if (tmpdate.month != diaactual.month){
          		movetype = MONTH;
        	} else {
          		movetype = DAY;
        	}
		    diaactual.year = tmpdate.year;
		    diaactual.month = tmpdate.month;
		    diaactual.day = tmpdate.day;
		    dateindex = 0;
		    state = CHANGEDAY;
		    
		    break;
		case CHANGEDAY:
			switch(movedate[dateindex].button) {
				case UP:
					ReportData->LY = STICK_MIN;				
					break;
				case LEFT:
					ReportData->LX = STICK_MIN;				
					break;
				case DOWN:
					ReportData->LY = STICK_MAX;				
					break;
				case RIGHT:
					ReportData->LX = STICK_MAX;				
					break;
				default:
					ReportData->LX = STICK_CENTER;
					ReportData->LY = STICK_CENTER;
					ReportData->RX = STICK_CENTER;
					ReportData->RY = STICK_CENTER;
					ReportData->HAT = HAT_CENTER;
					break;
			}
			duration_count++;
			if (duration_count > movedate[dateindex].duration) {
				dateindex++;
				duration_count = 0;				
			}
			if (dateindex > movetype) {
				duration_count = 0;
				state = BREATHE;
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
			}
			break;
		case CLEANUP:
			state = DONE;
			break;
		case DONE:
			#ifdef ALERT_WHEN_DONE
			portsval = ~portsval;
			PORTD = portsval; //flash LED(s) and sound buzzer if attached
			PORTB = portsval;
			_delay_ms(250);
			#endif
			return;
	}
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;
}
