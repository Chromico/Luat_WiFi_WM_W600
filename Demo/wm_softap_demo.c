/***************************************************************************** 
* 
* File Name : wm_softap_demo.c 
* 
* Description: soft ap demo function 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : cuiyanchang
* 
* Date : 2014-6-2 
*****************************************************************************/ 
#include <string.h>
#include "wm_include.h"
#include "wm_demo.h"

#if DEMO_SOFT_AP
/*1)Add sta add callback function
    2)Add sta list monitor task*/
static tls_os_timer_t *sta_monitor_tim = NULL;
static void demo_monitor_stalist_tim(void *ptmr, void *parg)
{
	u8 *stabuf = NULL;
	u32 stanum = 0;
	int i = 0;
	stabuf = tls_mem_alloc(1024);
	tls_wifi_get_authed_sta_info(&stanum, stabuf, 1024);
	if (stanum){
		printf("sta mac:\n");
		for (i = 0; i < stanum ; i++)
		{
			printf("%x:%x:%x:%x:%x:%x\n", stabuf[6*i+0], stabuf[6*i+1], stabuf[6*i+2], stabuf[6*i+3], stabuf[6*i+4], stabuf[6*i+5] );
		}
	}

	tls_mem_free(stabuf);
}

int demo_create_softap(u8 *ssid, u8 *key, int chan, int encrypt, int format)
{
	struct tls_softap_info_t *apinfo;
	struct tls_ip_info_t *ipinfo;
	u8 ret=0;
	u8 ssid_set = 0;

	u8 ssid_len = 0;
	if (!ssid)
	{
		return WM_FAILED;
	}

	ipinfo = tls_mem_alloc(sizeof(struct tls_ip_info_t));
	if (!ipinfo)
	{
		return WM_FAILED;
	}
	apinfo = tls_mem_alloc(sizeof(struct tls_softap_info_t));
	if (!apinfo)
	{
		tls_mem_free(ipinfo);
		return WM_FAILED;
	}

	tls_wifi_set_oneshot_flag(0);          /*���һ�����ñ�־*/

	tls_param_get(TLS_PARAM_ID_BRDSSID, (void *)&ssid_set, (bool)0);
	if (0 == ssid_set)
	{
		ssid_set = 1;
		tls_param_set(TLS_PARAM_ID_BRDSSID, (void *)&ssid_set, (bool)1); /*����BSSID�㲥��־*/
	}


	tls_wifi_disconnect();
	
	ssid_len = strlen((const char *)ssid);
	MEMCPY(apinfo->ssid, ssid, ssid_len);
	apinfo->ssid[ssid_len]='\0';
	
	apinfo->encrypt = encrypt;  /*0:open, 1:wep64, 2:wep128*/
	apinfo->channel = chan; /*channel*/
	apinfo->keyinfo.format = format; /*�����ʽ:0��hex��ʽ��1��ascii��ʽ*/
	apinfo->keyinfo.index = 1;  /*wep����*/
	apinfo->keyinfo.key_len = strlen((const char *)key); /*���볤��*/
	MEMCPY(apinfo->keyinfo.key, key, apinfo->keyinfo.key_len);
	/*ip������Ϣ:ip��ַ�����룬dns����*/
	ipinfo->ip_addr[0] = 192;
	ipinfo->ip_addr[1] = 168;
	ipinfo->ip_addr[2] = 4;
	ipinfo->ip_addr[3] = 1;
	ipinfo->netmask[0] = 255;
	ipinfo->netmask[1] = 255;
	ipinfo->netmask[2] = 255;
	ipinfo->netmask[3] = 0;
	MEMCPY(ipinfo->dnsname, "openluat.com", sizeof("openluat.com"));

	ret = tls_wifi_softap_create(apinfo, ipinfo);
	printf("\n ap create %s ! \n", (ret == WM_SUCCESS)? "Successfully" : "Error");

	if (ret == WM_SUCCESS)
	{
		if (sta_monitor_tim)
		{
			tls_os_timer_delete(sta_monitor_tim);
			sta_monitor_tim = NULL;
		}
		tls_os_timer_create(&sta_monitor_tim, demo_monitor_stalist_tim, NULL, 500, TRUE, NULL);
		tls_os_timer_start(sta_monitor_tim);
	}

	tls_mem_free(ipinfo);
	tls_mem_free(apinfo);
	return ret;
}

#endif
