// TQ1 > Name der Spindel
// AQ1 > Tilt
// AQ2 > Plato
// AQ3 > Temperatur
// AQ4 > Battery

int nEvents;
int idx;
char* Text;
char strTemp[30];
float out;

while(TRUE)
    {
    nEvents = 1;
    if (nEvents & 0x01)
        {
        Text = getinputtext(0);
		idx = strfind(Text, " ", 0);
		if (idx < 0) {return;}
		strncpy(strTemp,Text,idx);
		//name
		setoutputtext(0, strTemp);
		Text = strstrskip(Text, " ");
		idx = strfind(Text, " ", 0);
		if (idx < 0) {return;}
		//tilt
		strncpy(strTemp,Text,idx);
		out = batoi(strTemp);
		setoutput(0, out);
		out = 0.0045*out*out-0.123*out+1.0689;
		setoutput(1, out);
		Text = strstrskip(Text, " ");
		idx = strfind(Text, " ", 0);
		if (idx < 0) {return;}
		//temperature
		strncpy(strTemp,Text,idx);
		out = batoi(strTemp);
		setoutput(2, out);
		Text = strstrskip(Text, " ");
		//battery
		out = batoi(Text);
		setoutput(3, out);
        }
    sleep(100);
    }
