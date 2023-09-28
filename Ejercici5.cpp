const byte LEDR_PIN = 3;
const byte LEDV_PIN = 4;
const unsigned long LEDR_PERIODO_CONTROL = 5000;
const unsigned long LEDV_PERIODO = 800;
const unsigned long LEDV_PERIODO2 = (LEDV_PERIODO / 2);
const unsigned long MONITOR_PERIODO = 1000;
const byte CICLO_TRABAJO_INC = 20;
const byte CICLO_TRABAJO_MAX = 100;
// −−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
struct LED_Verde
{
    unsigned long last_ms;
    byte pin;
    byte estado;
};
void setup_ledv(struct LED_Verde &led, byte pin)
{
    led.last_ms = -LEDV_PERIODO2;
    led.pin = pin;
    led.estado = LOW;
    pinMode(led.pin, OUTPUT);
}
void loop_ledv(struct LED_Verde &led, unsigned long curr_ms)
{
    if (curr_ms - led.last_ms >= LEDV_PERIODO2)
    {
        led.last_ms += LEDV_PERIODO2;
        led.estado = (led.estado == LOW ? HIGH : LOW);
        digitalWrite(led.pin, led.estado);
    }
}
// −−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
struct LED_Rojo
{
    unsigned long last_ms;
    unsigned long duty_cicle_ms;
    byte pin;
    byte estado;
    byte ciclo_trabajo;
};
void setup_ledr(struct LED_Rojo &led, byte pin)
{
    led.last_ms = -LEDR_PERIODO_CONTROL;
    led.duty_cicle_ms = LEDR_PERIODO_CONTROL;
    led.pin = pin;
    led.estado = LOW;
    pinMode(led.pin, OUTPUT);
    led.ciclo_trabajo = 0;
}
void loop_ledr(struct LED_Rojo &led, unsigned long curr_ms)
{
    if (curr_ms - led.last_ms >= LEDR_PERIODO_CONTROL)
    {
        led.last_ms += LEDR_PERIODO_CONTROL;
        led.duty_cicle_ms = led.ciclo_trabajo * LEDR_PERIODO_CONTROL / 100;
        led.estado = (led.duty_cicle_ms == 0 ? LOW : HIGH);
        digitalWrite(led.pin, led.estado);
        led.ciclo_trabajo += CICLO_TRABAJO_INC;
        if (led.ciclo_trabajo > CICLO_TRABAJO_MAX)
        {
            led.ciclo_trabajo = 0;
        }
    }
    if (curr_ms - led.last_ms >= led.duty_cicle_ms)
    {
        led.duty_cicle_ms = LEDR_PERIODO_CONTROL;
        led.estado = LOW;
        digitalWrite(led.pin, led.estado);
    }
}
// −−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
struct Monitor
{
    unsigned long last_ms;
};
void setup_monitor(struct Monitor &mtr)
{
    mtr.last_ms = -MONITOR_PERIODO;
    Serial.begin(9600);
    if (Serial)
    {
        Serial.println("#> Arduino Ciclo de Trabajo (5000 ms)");
        Serial.println("#$ -y -1:2 - tCiclo_Trabajo_LED_Rojo ");
    }
}
void loop_monitor(struct Monitor &mtr, unsigned long curr_ms,
                  const struct LED_Rojo &led)
{
    if (curr_ms - mtr.last_ms >= MONITOR_PERIODO)
    {
        mtr.last_ms += MONITOR_PERIODO;
        if (Serial)
        {
            Serial.println(led.estado == LOW ? 0 : 1);
        }
    }
}
// −−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
struct Monitor monitor;
struct LED_Verde ledv;
struct LED_Rojo ledr;
void setup()
{
    setup_ledv(ledv, LEDV_PIN);
    setup_ledr(ledr, LEDR_PIN);
    setup_monitor(monitor);
}
void loop()
{
    unsigned long curr_ms = millis();
    loop_ledv(ledv, curr_ms);
    loop_ledr(ledr, curr_ms);
    loop_monitor(monitor, curr_ms, ledr);
}