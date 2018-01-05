#include "msgbuild.h"
#define CIRCLE_COUNT 5
void valve_Init()
{
	GPIO_InitSettingType g;
	g.Signal = GPIO_Pin_Signal_Digital;
	g.Func = GPIO_Reuse_Func0;
	g.Dir = GPIO_Direction_Output;
	g.ODE = GPIO_O
	
}
void open_Water()
{
	
}
void close_Water()
{
	
}
uint8_t get_Valve_Status()
{
	uint8_t ret = 0;
	return ret;
}
void type_Time()
{
	/**************服务时间到**************/
	if(EPROM.RunTime >= EPROM.ServerTime)
	{
		close_Water();
	}	
}
void type_Flow()
{
	/**************服务流量到**************/
	if(EPROM.RunFlow >= EPROM.ServerFlow)
	{
		close_Water();
	}
}
void MSG_Build(uint8_t *sendStr)
{
	uint16_t len;
	char temp[20];
	volatile uint32_t tds1 = 0,tds2 = 0;
	getTDS(&tds1,&tds2);
	len = sprintf((char *)sendStr,"{\"SN\":\"%s\",",(char *)&EPROM.IMEI[0]);
    len+=sprintf((char *)temp,"\"CMD\":\"01\",");
	strcat((char *)sendStr,temp);
	len+=sprintf((char *)temp,"\"TYPE\":%c,",EPROM.ServerType);
	strcat((char *)sendStr,temp);
	len+=sprintf((char *)temp,"\"VS\":\"%c%c%c\",",(((EPROM.ValveStatus & 0x04)>> 2)+'0'),(((EPROM.ValveStatus & 0x02) >> 1)+'0'),((EPROM.ValveStatus & 0x01)+'0'));
	strcat((char *)sendStr,temp);
	len+=sprintf((char *)temp,"\"TDS1\":%03u,\"TDS2\":%03u,\"F\":%6u}",tds1,tds2,EPROM.RunFlow);
	strcat((char *)sendStr,temp);
}

ErrorStatus MSG_Deal(uint8_t *rcv)
{
	char *cp1,*cp2;
	uint8_t flag = 0,type = 0,vs = 0,t = 0,ft = 0;
	uint32_t sf = 0,rt = 0,et = 0;
	cp2 = (char *)rcv;
	if((cp1 = strstr(cp2,"TYPE")) != NULL)
	{
		type = *(cp1+6);
	}else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"VS")) != NULL)
	{
		vs = 0;
		cp1+=5;
		if(*(cp1+0) == '1')
		{
			vs += 4;
		}
		if(*(cp1+1) == '1')
		{
			vs += 2;
		}
		if(*(cp1+2) == '1')
		{
			vs += 1;
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"T")) != NULL)
	{
		cp1 +=3;
		t = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				t *=10;
				t +=*cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"FT")) != NULL)
	{
		cp1 +=4;
		ft = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				ft *=10;
				ft +=*cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"SF")) != NULL)
	{
		cp1 +=4;
		sf = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				sf *= 10;
				sf += *cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"RT")) != NULL)
	{
		cp1 +=4;
		rt = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				rt *= 10;
				rt += *cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	cp2 = cp1;
	if((cp1 = strstr(cp2,"ET")) != NULL)
	{
		cp1 +=4;
		et = 0;
		while(1)
		{
			if((*cp1) >= '0' && (*cp1) <='9')
			{
				et *= 10;
				et += *cp1 -'0';
				cp1++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		flag <<= 1;
        flag |= 1;		
	}
	if(!flag)
	{
		EPROM.ServerType = type;
		EPROM.ValveStatus = vs;
		EPROM.CircleTime = t;
		EPROM.ContinuTime = ft;
		EPROM.ServerFlow = sf;
		EPROM.RunTime = rt;
		EPROM.ServerTime  = et;
		Save_To_EPROM((uint32_t *)&EPROM.IMEI[0],(sizeof(EPROM)/sizeof(uint32_t)));
		delay_nms(10);
		Save_To_EPROM((uint32_t *)&EPROM.IMEI[0],(sizeof(EPROM)/sizeof(uint32_t)));
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}