
// CLASS : ToggleButton ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Operates each of the buttons by toggling the corresponding output
// when the button is pressed.
// Each button runs as a seperate thread

class ToggleButton : public SwitchInput
{
public:
    ToggleButton(int pin, Type type, int ch);
    void on_close();
private:
    int channel;
};

ToggleButton::ToggleButton(int pin, Type type, int ch) :
    SwitchInput(pin, DEFAULT_DEBOUNCE, type)
{
    this->channel = ch;
}

void ToggleButton::on_close()
{
    Serial.print("Toggle channel ");
    Serial.println(this->channel + 1);
    digitalWrite(output_pin[this->channel], !digitalRead(output_pin[this->channel]));
}

// End CLASS : ToggleButton -----------------------------------------------------

