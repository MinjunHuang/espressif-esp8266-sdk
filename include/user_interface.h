/*
 *  Copyright (C) 2013 -2014  Espressif System
 *
 */

#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#include "os_type.h"
#ifdef LWIP_OPEN_SRC
#include "lwip/ip_addr.h"
#else
#include "ip_addr.h"
#endif

#include "queue.h"
#include "user_config.h"
#include "spi_flash.h"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

enum rst_reason {
	DEFAULT_RST_FLAG	= 0,
	WDT_RST_FLAG	= 1,
	EXP_RST_FLAG    = 2
};

struct rst_info{
	uint32_t flag;
	uint32_t exccause;
	uint32_t epc1;
	uint32_t epc2;
	uint32_t epc3;
	uint32_t excvaddr;
	uint32_t depc;
};

#define UPGRADE_FW_BIN1         0x00
#define UPGRADE_FW_BIN2         0x01

typedef void (*upgrade_states_check_callback)(void * arg);

struct upgrade_server_info {
    uint8_t ip[4];
    uint16_t port;

    uint8_t upgrade_flag;

    uint8_t pre_version[8];
    uint8_t upgrade_version[8];

    uint32_t check_times;
    uint8_t *url;

    upgrade_states_check_callback check_cb;
    struct espconn *pespconn;
};

uint8_t system_upgrade_start(struct upgrade_server_info *server);
uint8_t system_upgrade_start_ssl(struct upgrade_server_info *server);
uint8_t system_upgrade_userbin_check(void);
void system_upgrade_reboot(void);

void system_restore(void);
void system_restart(void);
void system_deep_sleep(uint32_t time_in_us);

void system_timer_reinit(void);
uint32_t system_get_time(void);

/* user task's prio must be 0/1/2 !!!*/
enum {
    USER_TASK_PRIO_0 = 0,
    USER_TASK_PRIO_1,
    USER_TASK_PRIO_2,
    USER_TASK_PRIO_MAX
};

void system_os_task(os_task_t task, uint8_t prio, os_event_t *queue, uint8_t qlen);
void system_os_post(uint8_t prio, os_signal_t sig, os_param_t par);

void system_print_meminfo(void);
uint32_t system_get_free_heap_size(void);

void system_set_os_print(uint8_t onoff);

uint64_t system_mktime(uint32_t year, uint32_t mon, uint32_t day, uint32_t hour, uint32_t min, uint32_t sec);

uint32_t system_get_chip_id(void);

typedef void (* init_done_cb_t)(void);

void system_init_done_cb(init_done_cb_t cb);

uint32_t system_rtc_clock_cali_proc(void);
uint32_t system_get_rtc_time(void);

uint8_t system_rtc_mem_read(uint8_t src_addr, void *des_addr, uint16_t load_size);
uint8_t system_rtc_mem_write(uint8_t des_addr, const void *src_addr, uint16_t save_size);

void system_uart_swap(void);

uint16_t system_adc_read(void);

#define NULL_MODE       0x00
#define STATION_MODE    0x01
#define SOFTAP_MODE     0x02
#define STATIONAP_MODE  0x03

uint8_t wifi_get_opmode(void);
uint8_t wifi_set_opmode(uint8_t opmode);

struct bss_info {
    STAILQ_ENTRY(bss_info)     next;

    uint8_t bssid[6];
    uint8_t ssid[32];
    uint8_t channel;
    int8_t rssi;
    uint8_t authmode;
    uint8_t is_hidden;
};

typedef struct _scaninfo {
    STAILQ_HEAD(, bss_info) *pbss;
    struct espconn *pespconn;
    uint8_t totalpage;
    uint8_t pagenum;
    uint8_t page_sn;
    uint8_t data_cnt;
} scaninfo;

typedef void (* scan_done_cb_t)(void *arg, STATUS status);

struct station_config {
    uint8_t ssid[32];
    uint8_t password[64];
    uint8_t bssid_set;
    uint8_t bssid[6];
};

uint8_t wifi_station_get_config(struct station_config *config);
uint8_t wifi_station_set_config(struct station_config *config);

uint8_t wifi_station_connect(void);
uint8_t wifi_station_disconnect(void);

struct scan_config {
    uint8_t *ssid;
    uint8_t *bssid;
    uint8_t channel;
    uint8_t show_hidden;
};

uint8_t wifi_station_scan(struct scan_config *config, scan_done_cb_t cb);

uint8_t wifi_station_get_auto_connect(void);
uint8_t wifi_station_set_auto_connect(uint8_t set);

enum {
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP
};

uint8_t wifi_station_get_connect_status(void);

uint8_t wifi_station_get_current_ap_id(void);
uint8_t wifi_station_ap_change(uint8_t current_ap_id);
uint8_t wifi_station_ap_number_set(uint8_t ap_number);

uint8_t wifi_station_dhcpc_start(void);
uint8_t wifi_station_dhcpc_stop(void);

typedef enum _auth_mode {
    AUTH_OPEN           = 0,
    AUTH_WEP,
    AUTH_WPA_PSK,
    AUTH_WPA2_PSK,
    AUTH_WPA_WPA2_PSK
} AUTH_MODE;

struct softap_config {
    uint8_t ssid[32];
    uint8_t password[64];
    uint8_t ssid_len;
    uint8_t channel;
    uint8_t authmode;
    uint8_t ssid_hidden;
    uint8_t max_connection;
};

uint8_t wifi_softap_get_config(struct softap_config *config);
uint8_t wifi_softap_set_config(struct softap_config *config);

struct station_info {
	STAILQ_ENTRY(station_info)	next;

	uint8_t bssid[6];
	struct ip_addr ip;
};

struct dhcps_lease {
	uint32_t start_ip;
	uint32_t end_ip;
};

struct station_info * wifi_softap_get_station_info(void);
void wifi_softap_free_station_info(void);

uint8_t wifi_softap_dhcps_start(void);
uint8_t wifi_softap_dhcps_stop(void);
uint8_t wifi_softap_set_dhcps_lease(struct dhcps_lease *please);

#define STATION_IF      0x00
#define SOFTAP_IF       0x01

uint8_t wifi_get_ip_info(uint8_t if_index, struct ip_info *info);
uint8_t wifi_set_ip_info(uint8_t if_index, struct ip_info *info);
uint8_t wifi_get_macaddr(uint8_t if_index, uint8_t *macaddr);
uint8_t wifi_set_macaddr(uint8_t if_index, uint8_t *macaddr);

uint8_t wifi_get_channel(void);
uint8_t wifi_set_channel(uint8_t channel);

void wifi_status_led_install(uint8_t gpio_id, uint32_t gpio_name, uint8_t gpio_func);

/** Get the absolute difference between 2 uint32_t values (correcting overflows)
 * 'a' is expected to be 'higher' (without overflow) than 'b'. */
#define ESP_U32_DIFF(a, b) (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0xFFFFFFFF) + 1)))

void wifi_promiscuous_enable(uint8_t promiscuous);

typedef void (* wifi_promiscuous_cb_t)(uint8_t *buf, uint16_t len);

void wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

enum phy_mode {
	PHY_MODE_11B	= 1,
	PHY_MODE_11G	= 2,
	PHY_MODE_11N    = 3
};

enum phy_mode wifi_get_phy_mode(void);
uint8_t wifi_set_phy_mode(enum phy_mode mode);

#endif
