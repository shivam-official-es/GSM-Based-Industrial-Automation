#include <LPC21XX.H>
 #include <string.h>
 #define LCD 0x0f<<20
 #define RS 1<<17
 #define RW 1<<18
 #define E 1<<19
 #define LED0 (1 << 17)
 #define LED1 (1 << 18)
 #define LED2 (1 << 19)
 #define mot1 (1<<5)
 #define mot2 (1<<6)
 void UART_CONFIG(void);
 void UART_TX(unsigned char);
 unsigned char UART_RX(void);
 void UART_STR(unsigned char*);
 void delay_ms(int);
 void GSM_INIT(void);
 void RECEIVE_SMS(void);
 void LCD_INIT(void);
 void LCD_COMMAND(unsigned char);
 void LCD_DATA(unsigned char);
 void LCD_STRING(unsigned char*);
 char response[100]; // Buffer for received message
 int main()
 {
    char motor_on[] = "MOTOR ON";
    char motor_off[] = "MOTOR OFF";
    char pump_on[] = "PUMP ON";
    char pump_off[] = "PUMP OFF";
    char alarm_on[] = "ALARM ON";
    char alarm_off[] = "ALARM OFF";
 char actuator_on[]="ACTUATOR ON";
 char actuator_off[]="ACTUATOR OFF";
 IODIR0 |= (LED0 | LED1 | LED2| mot1| mot2);  // Configure LEDs as output
    IOSET0 = LED0 | LED1 | LED2| mot1| mot2;  // Turn off all LEDs initially
    UART_CONFIG();
    LCD_INIT();
 LCD_COMMAND(0x80);
 LCD_STRING("GSM PROJECT...");
 LCD_COMMAND(0xc0);
 LCD_STRING("V24CE2");
 delay_ms(3000);
 LCD_COMMAND(0x01);
    LCD_COMMAND(0x80);
    LCD_STRING("Setting GSM");
 GSM_INIT();  // Initialize GSM module for receiving SMS
 delay_ms(2000);
    LCD_COMMAND(0x80);
    LCD_STRING("Ready to receive");  while (1)
    {
        RECEIVE_SMS();  // Continuously check for incoming SMS
        delay_ms(1000);
        LCD_COMMAND(0x80);
        if (strcmp(response, actuator_on) == 0)
        {
            IOCLR0 = LED0;  // Turn ON LED0
            LCD_COMMAND(0x01);
            LCD_STRING("ACTUATOR ON");
            delay_ms(2000);
        }
        else if (strcmp(response, pump_on) == 0)
        {
            IOCLR0 = LED1;  // Turn ON LED1
            LCD_COMMAND(0x01);
            LCD_STRING("PUMP ON");
            delay_ms(2000);
        }
        else if (strcmp(response, alarm_on) == 0)
        {
            IOCLR0 = LED2;  // Turn ON LED2
            LCD_COMMAND(0x01);
            LCD_STRING("ALARM ON");
            delay_ms(2000);
        }
        else if (strcmp(response, actuator_off) == 0)
        {
            IOSET0 = LED0;  // Turn OFF LED0
            LCD_COMMAND(0x01);
            LCD_STRING("ACTUATOR OFF");
            delay_ms(2000);
        }
        else if (strcmp(response, pump_off) == 0)
        {
            IOSET0 = LED1;  // Turn OFF LED1
            LCD_COMMAND(0x01);
            LCD_STRING("PUMP OFF");
            delay_ms(2000);
        }
        else if (strcmp(response, alarm_off) == 0)
        {
            IOSET0 = LED2;  // Turn OFF LED2
            LCD_COMMAND(0x01);
            LCD_STRING("ALARM OFF");
            delay_ms(2000);
        }
        {
        }
 else if (strcmp(response, motor_on) == 0)
            IOCLR0 = mot1;
 IOSET0 =mot2;  // Turn ON MOTOR
            LCD_COMMAND(0x01);
            LCD_STRING("MOTOR ON");
            delay_ms(2000);
 else if (strcmp(response, motor_off) == 0) {
            IOSET0 = mot1|mot2;  // Turn OFF MOTOR
            LCD_COMMAND(0x01);
            LCD_STRING("MOTOR OFF");
            delay_ms(2000);
        }
 else
 {
 LCD_COMMAND(0x01);  // Clear LCD
 LCD_STRING("INVALID MSG");  // Display message
 delay_ms(2000);
 }
        delay_ms(1000);
    }
 }
 // UART Configuration
 void UART_CONFIG()
 {
    PINSEL0 |= 0x05;   // Configure P0.0 as TXD0, P0.1 as RXD0
    U0LCR = 0x83;      // Enable DLAB, 8-bit data
    U0DLL = 97;        // Baud rate 9600 with PCLK = 15MHz
    U0DLM = 0;
    U0LCR = 0x03;      // Disable DLAB
 }
 void UART_TX(unsigned char ch)
 {
    while (!(U0LSR & (1 << 5))); // Wait until THR is empty
    U0THR = ch;
 }
 unsigned char UART_RX(void)
 {
    while (!(U0LSR & 1)); // Wait for data to be received
    return U0RBR;
 }
 void UART_STR(unsigned char* s)
 {
    while (*s)
        UART_TX(*s++);
 }
 // GSM Initialization
 void GSM_INIT()
 {
    UART_STR("AT\r\n");  // Check module response
    delay_ms(1000);
    UART_STR("AT+CMGF=1\r\n");  // Set SMS mode to text
    delay_ms(1000);
    UART_STR("AT+CNMI=2,2,0,0,0\r\n");  // Enable new SMS notification
    delay_ms(1000);
 }
// Function to receive SMS message only
 void RECEIVE_SMS()
 {
    char buffer[100];  // Temporary buffer to avoid overwriting response[]
    int i = 0, msgStart = 0;
    while (1)
    {
        buffer[i] = UART_RX();  // Read character from UART
        if (buffer[i] == '\n')  // New line detected
        {
            buffer[i + 1] = '\0';  // Null terminate string
            if (strstr(buffer, "+CMT:"))  // SMS header detected
            {
                msgStart = 1;  // Next line will contain actual message
                i = 0;  // Reset buffer index for next read
            }
            else if (msgStart)  // Extract actual message content
            {
                strcpy(response, buffer);  // Copy message to response
                response[strcspn(response, "\r\n")] = '\0';  // Remove \r\n if 
present
    }
 }
                break;  // Exit after capturing message
            }
            i = 0;  // Reset buffer index for next read
        }
        else
        {
            i++;  // Keep reading
        }
 // LCD Initialization
 void LCD_INIT(void)
 {
    IODIR1 |= LCD | RS | RW | E;
    LCD_COMMAND(0x01);
    LCD_COMMAND(0x02);
    LCD_COMMAND(0x0c);
    LCD_COMMAND(0x28);
    LCD_COMMAND(0x80);
 }
 void LCD_STRING(unsigned char *s)
 {
    while (*s)
        LCD_DATA(*s++);
 }
 void LCD_COMMAND(unsigned char cmd)
 {
    IOCLR1 = LCD;
    IOSET1 = (cmd & 0xf0) << 16;
    IOCLR1 = RS;
    IOCLR1 = RW;
 IOSET1 = E;
    delay_ms(2);
    IOCLR1 = E;
    IOCLR1 = LCD;
    IOSET1 = (cmd & 0x0f) << 20;
    IOCLR1 = RS;
    IOCLR1 = RW;
    IOSET1 = E;
    delay_ms(2);
    IOCLR1 = E;
 }
 void LCD_DATA(unsigned char d)
 {
    IOCLR1 = LCD;
    IOSET1 = (d & 0xf0) << 16;
    IOSET1 = RS;
    IOCLR1 = RW;
    IOSET1 = E;
    delay_ms(2);
    IOCLR1 = E;
    IOCLR1 = LCD;
    IOSET1 = (d & 0x0f) << 20;
    IOSET1 = RS;
    IOCLR1 = RW;
    IOSET1 = E;
    delay_ms(2);
    IOCLR1 = E;
 }
 // Delay Function
 void delay_ms(int ms)
 {
    T0PR = 15000 - 1;
    T0TCR = 0x01;
    while (T0TC < ms);
    T0TCR = 0x03;
    T0TCR = 0x00;
 } 