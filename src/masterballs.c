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
	PLUS,
	MINUS,
	HOME,
	NOTHING,
	TRIGGERS
} Buttons_t;

typedef struct {
	Buttons_t button;
	uint16_t duration;
} command; 

typedef struct {
  int year;
  int month;
  int day;
} sdate;

sdate diaactual = {2020, 3, 31};

static const int YEAR = 9;
static const int MONTH = 5;
static const int DAY = 1;

static const command movedate[] = {
	{ UP,         2 },		{ NOTHING,  2 },
	{ RIGHT,      2 },		{ NOTHING,  2 },
	{ UP,         2 },		{ NOTHING,  2 },
	{ RIGHT,      2 },		{ NOTHING,  2 },
	{ UP,         2 },		{ NOTHING,  2 },
};


static const command step[] = {
	// Setup controller
						{ NOTHING,  100 },
	// 1 2
	{ TRIGGERS,   2 },	{ NOTHING,  50 },
	// 2 4
	{ TRIGGERS,   2 },	{ NOTHING,  50 },
	// 5 6
	{ A,          2 },	{ NOTHING,  50 },
	// Open game 
	// 7-8
	{ HOME,       2 },	{ NOTHING,  50 },
	// 9-10
	{ A,          2 },	{ NOTHING,  50 },
	// one day forward
	// 11-12
	{ HOME,       2 },		{ NOTHING,  80 },
	// 13-14
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 15-16
	{ RIGHT,      2 },		{ NOTHING,  1 },
	// 17-18
	{ RIGHT,      2 },		{ NOTHING,  1 },
	// 19-20
	{ RIGHT,      2 },		{ NOTHING,  1 },
	// 21-22
	{ RIGHT,      2 },		{ NOTHING,  1 },
	// 23-24
	{ A,          2 },		{ NOTHING,  40 },
	// 25-26
	{ DOWN,       80 },		{ NOTHING,  10 },
	// 27-28
	{ A,          2 },		{ NOTHING,  20 },
	// 29-30
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 31-32
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 33-34
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 35-36
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 37-38
	{ A,          2 },		{ NOTHING,  20 },
	// 39-40
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 41-42
	{ DOWN,       2 },		{ NOTHING,  2 },
	// 43-44
	{ A,          2 },		{ NOTHING,  15 },
	// HERE IS THE DAY CHANGE, COMMENT IS FOR ONE DAY
	// { UP,         2 },		{ NOTHING,  2 },
	// 45-46
	{ RIGHT,      2 },		{ NOTHING,  2 },
	// 47-48
	{ RIGHT,      2 },		{ NOTHING,  2 },
	// 49-50
	{ RIGHT,      2 },		{ NOTHING,  2 },
	// 51-52
	{ RIGHT,      2 },		{ NOTHING,  2 },
	// 53-54
	{ RIGHT,      2 },		{ NOTHING,  2 },
	// 55-56
	{ A,          2 },		{ NOTHING,  6 },
	// Return to game
	// 57-58
	{ HOME,       2 },		{ NOTHING,  80 },
	// 59-60
	{ A,          2 },		{ NOTHING,  60 },
	// 61-62
	{ B,          2 },		{ NOTHING,  5 },
	// 63-64
	{ B,          2 },		{ NOTHING,  5 },
	// 65-66
	{ B,          2 },		{ NOTHING,  5 },
	// 67-68
	{ B,          2 },		{ NOTHING,  5 },
	// 69-70
	{ B,          2 },		{ NOTHING,  5 },
	// 71-72
	{ B,          2 },		{ NOTHING,  5 },
	// 73-74
	{ B,          2 },		{ NOTHING,  5 },
	// 75-76
	{ B,          2 },		{ NOTHING,  5 },
	// 77-78
	{ B,          2 },		{ NOTHING,  5 },
	// 79-80
	{ B,          2 },		{ NOTHING,  5 },
	// Rotom Lottery
	// 81-82
	{ A,          2 },		{ NOTHING,  40 },
	// 83-84
	{ A,          2 },		{ NOTHING,  20 },
	// 85-86
	{ DOWN,       2 },		{ NOTHING,  10 },
	// 87-88
	{ A,          2 },		{ NOTHING,  40 },
	// 89-90
	{ A,          2 },		{ NOTHING,  40 },
	// 91-92
	{ A,          2 },		{ NOTHING,  40 },
	// 93-94
	{ A,          2 },		{ NOTHING,  40 },
	// 95-96
	{ A,          2 },		{ NOTHING,  100 },
	// 97-98
	{ A,          2 },		{ NOTHING,  40 },
	// 99-100
	{ A,          2 },		{ NOTHING,  40 },
	// 101-102
	{ A,          2 },		{ NOTHING,  40 },
	// 103-104
	{ A,          2 },		{ NOTHING,  40 },
	// 105-106
	{ A,          2 },		{ NOTHING,  100 },
	// 107-108
	{ A,          2 },		{ NOTHING,  40 },
	// 109-110
	{ A,          2 },		{ NOTHING,  40 },
	// 111-112
	{ A,          2 },		{ NOTHING,  40 },
	// 113-114
	{ A,          2 },		{ NOTHING,  40 },
	// 115-116
	{ A,          2 },		{ NOTHING,  40 },
	// 117-118
	{ A,          2 },		{ NOTHING,  100 },
	// 119-120
	{ A,          2 },		{ NOTHING,  40 },
	// 121-122
	{ A,          2 },		{ NOTHING,  40 },
	// 123-124
	{ A,          2 },		{ NOTHING,  40 },
};

int main(void) {
	SetupHardware();
	GlobalInterruptEnable();
	for (;;)
	{
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
	DDRD  = 0xFF;
	PORTD =  0x0;
	DDRB  = 0xFF;
	PORTB =  0x0;
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
int duration_count = 0;
int portsval = 0;
int dateindex = 0;
int movetype = 0;
sdate tmpdate;

long date2number(sdate date) {
  long long  y, m;
  m = ((long)date.month + 9)%12;
  y = (long)date.year - m/10;
  return y*365 + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ((long)date.day - 1);;
}

sdate number2date(long d) {
  sdate pd;
  long long y, ddd, mi;

  y = (10000LL*(long long)d + 14780LL)/3652425LL;
  ddd = (long long)d - (y*365LL + y/4LL - y/100LL + y/400LL);
  if (ddd < 0) {
    y--;
    ddd = (long long)d - (y*365LL + y/4LL - y/100LL + y/400LL);
  }
  mi = (52LL + 100LL*ddd)/3060LL;
  pd.year =(int) (y + (mi + 2LL)/12LL);
  pd.month =(int) ((mi + 2LL)%12LL + 1LL);
  pd.day =(int) (ddd - (mi*306LL + 5LL)/10LL + 1LL);
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
				case X:
					ReportData->Button |= SWITCH_X;
					break;
				case Y:
					ReportData->Button |= SWITCH_Y;
					break;
				case PLUS:
					ReportData->Button |= SWITCH_PLUS;
					break;
				case MINUS:
					ReportData->Button |= SWITCH_MINUS;
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
				bufindex = 11;
				duration_count = 0;
				state = BREATHE;
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
			PORTD = portsval;
			PORTB = portsval;
			_delay_ms(250);
			#endif
			return;
	}
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;
}
