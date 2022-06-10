#define buzzer 7
#define led 8
#define rec_button 10
#define listen_button 3
const byte buttons[] = { 4, 5, 6 };
const byte potents[] = { A0, A1, A2 };
int freq[] = { 100, 200, 300 };
int notes[25], durations[25], silence[25];
int counter = 0, cocounter = 0;
long timer, endtime, endsc;
bool rec = false, end_listening = false;

void setup()
{
    Serial.begin(9600);
    pinMode(buzzer, OUTPUT);
    pinMode(led, OUTPUT);
    pinMode(listen_button, INPUT_PULLUP);
    for (int i = 0; i < 3; i++)
    {
        pinMode(buttons[i], INPUT_PULLUP);
    }
}

void loop()
{
    Listening();
    Record();
    Is_Button_Pressed();
}

void Is_Recorded(int fr, int endtime)
{
    if (rec)
    {
        durations[counter] = endtime;
        notes[counter] = fr;
        Serial.println("Нота записана! Израсходовано " + (String)(counter + 1) + " нот(ы) из 25-ти доступных");
        counter++;
        cocounter++;
        if (counter > 25)
        {
            counter = 0;
            Serial.println("Вы преодолели количество допустимых записей. Очищаем буфер.");
            Clear();
        }
    }
}

void Listening()
{
    if (!digitalRead(listen_button) && notes[0] != 0)
    {
        for (int i = 0; i < sizeof(notes) / 2; i++)
        {
            tone(buzzer, notes[i]);
            delay(durations[i]);
            noTone(buzzer);
            delay(silence[i]);
            if (notes[i] == 0)
            {
                break;
            }
        }
        end_listening = false;
        timer = 0; endtime = 0; endsc = 0;
    }
}

void Record()
{
    if (digitalRead(rec_button))
    {
        rec = true;
        digitalWrite(led, HIGH);
    }
    else
    {
        rec = false;
        digitalWrite(led, LOW);
        Clear();
    }
}

void Is_Button_Pressed()
{
    for (int i = 0; i < 3; i++)
    {
        if (!digitalRead(buttons[i]))
        {
            end_listening = true;
            timer = millis();
            while (!digitalRead(buttons[i]))
            {
                tone(buzzer, freq[i] + analogRead(potents[i]));
            }
            endtime = millis() - timer;
            noTone(buzzer);
            delay(10);
            timer = millis();
            if (rec)
            {
                Is_Recorded(freq[i] + analogRead(potents[i]), endtime);
            }
        }
        else
        {
            if (notes[0] != 0 && end_listening)
            {
                endsc = millis() - timer;
                silence[cocounter] = endsc;
            }
        }
    }
}

void Clear()
{
    for (int i = 0; i < 25; i++)
    {
        notes[i] = 0;
        durations[i] = 0;
        silence[i] = 0;
    }
    counter = 0; cocounter = 0;
    timer = 0; endtime = 0; endsc = 0;
}