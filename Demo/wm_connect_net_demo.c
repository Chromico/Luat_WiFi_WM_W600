#include <string.h>
#include "wm_include.h"
#include "wm_demo.h"

#if DEMO_CONNECT_NET
static void con_net_status_changed_event(u8 status )
{
	switch(status)
	{
		case NETIF_IP_NET_UP:
		{
			struct tls_ethif * tmpethif = tls_netif_get_ethif();
#if TLS_CONFIG_LWIP_VER2_0_3
			print_ipaddr(&tmpethif->ip_addr);
#if TLS_CONFIG_IPV6
			print_ipaddr(&tmpethif->ip6_addr[0]);
			print_ipaddr(&tmpethif->ip6_addr[1]);
			print_ipaddr(&tmpethif->ip6_addr[2]);
#endif
#else
			printf("net up ==> ip = %d.%d.%d.%d\n",ip4_addr1(&tmpethif->ip_addr.addr),ip4_addr2(&tmpethif->ip_addr.addr),
							 ip4_addr3(&tmpethif->ip_addr.addr),ip4_addr4(&tmpethif->ip_addr.addr));
#endif
		}
			break;
		default:
			break;
	}
}

int demo_oneshot(void)
{
	printf("waiting for oneshot \n");
	tls_netif_add_status_event(con_net_status_changed_event);
	tls_wifi_set_oneshot_flag(1);
	return 0;
}


//������������ʾ��:t-connet("ssid","pwd");
int demo_connect_net(char *ssid, char *pwd)
{
	struct tls_param_ip *ip_param = NULL;

	if (!ssid)
	{
		return WM_FAILED;
	}

	printf("\nssid:%s\n", ssid);
	printf("password=%s\n",pwd);
	tls_wifi_disconnect();

	tls_wifi_set_oneshot_flag(0);

	ip_param = tls_mem_alloc(sizeof(struct tls_param_ip));
	if (ip_param)
	{
		tls_param_get(TLS_PARAM_ID_IP, ip_param, FALSE);
		ip_param->dhcp_enable = TRUE;
		tls_param_set(TLS_PARAM_ID_IP, ip_param, FALSE);
		tls_mem_free(ip_param);
	}

	tls_netif_add_status_event(con_net_status_changed_event);
	tls_wifi_connect((u8 *)ssid, strlen(ssid), (u8 *)pwd, strlen(pwd));
	printf("\nplease wait connect net......\n");

	return WM_SUCCESS;
}
#endif

