int nEvents;
int idx;
char strTemp[30];
float out;

enum OutputPorts
{
    tilt,            // AQ1
    plato,           // AQ2
	temperature,	 // AQ3
	battery          // AQ4
};

while(TRUE)
{
	nEvents = getinputevent();
    if (nEvents & 0x01)
        {
		char* Text = getinputtext(0);
		idx = strfind(Text, " ", 0);
		if (idx < 0) {free(Text); return;}
		//name
		strncpy(strTemp,Text,idx);
		setoutputtext(0, strTemp);
		Text = strstrskip(Text, " ");
		idx = strfind(Text, " ", 0);
		if (idx < 0) {free(Text); return;}
		//tilt
		strncpy(strTemp,Text,idx);
		out = batof(strTemp);
		setoutput(tilt, out);
		//plato
		out = 0.0045*out*out-0.123*out+1.0689;  //change formula according to your calibration!
		setoutput(plato, out);
		Text = strstrskip(Text, " ");
		idx = strfind(Text, " ", 0);
		if (idx < 0) {free(Text); return;}
		//temperature
		strncpy(strTemp,Text,idx);
		out = batof(strTemp);
		setoutput(temperature, out);
		Text = strstrskip(Text, " ");
		//battery
		out = batof(Text);
		setoutput(battery, out);
		free(Text);
        }
    sleep(5000);
}