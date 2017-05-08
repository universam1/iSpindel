int nEvents;
int idx1;
int idx2;
float out;
char strTemp[35];
char* str;

enum OutputPorts
{
    tilt,            // AQ1
    gravity,         // AQ2
	temperature,	 // AQ3
	battery          // AQ4
};

while(TRUE)
{
	nEvents = getinputevent();
	//AI1 means active
    if (nEvents & 0x01 && getinput(0) > 0)
        {
		str = getinputtext(0);
		idx1 = strfind(str, " ", 0);
		//name
		strncpy(strTemp,str,idx1);
		setoutputtext(0, strTemp);
		idx2 = strfind(str, " ", idx1+1);
		//tilt
		strncpy(strTemp,str+idx1+1,idx2);
		out = batof(strTemp);
		setoutput(tilt, out);
		idx1 = strfind(str, " ", idx2+1);
		//temperature
		strncpy(strTemp,str+idx2+1,idx1);
		out = batof(strTemp);
		setoutput(temperature, out);
		idx2 = strfind(str, " ", idx1+1);
		//battery
		strncpy(strTemp,str+idx1+1,idx2);
		out = batof(strTemp);
		setoutput(battery, out);
		//gravity
		strncpy(strTemp,str+idx2+1,strlen(str));
		out = batof(strTemp);
		setoutput(gravity, out);

		free(str);
    }
    sleep(5000);
}
