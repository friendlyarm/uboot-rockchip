/*
 * (C) Copyright 2023 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef _ASM_ARCH_GRF_RK3506_H
#define _ASM_ARCH_GRF_RK3506_H

#include <common.h>

/* grf register structure define */
struct rk3506_grf_reg {
     uint32_t soc_con0;                           /* address offset: 0x0000 */
     uint32_t soc_con1;                           /* address offset: 0x0004 */
     uint32_t soc_con2;                           /* address offset: 0x0008 */
     uint32_t soc_con3;                           /* address offset: 0x000c */
     uint32_t soc_con4;                           /* address offset: 0x0010 */
     uint32_t soc_con5;                           /* address offset: 0x0014 */
     uint32_t soc_con6;                           /* address offset: 0x0018 */
     uint32_t soc_con7;                           /* address offset: 0x001c */
     uint32_t soc_con8;                           /* address offset: 0x0020 */
     uint32_t soc_con9;                           /* address offset: 0x0024 */
     uint32_t soc_con10;                          /* address offset: 0x0028 */
     uint32_t soc_con11;                          /* address offset: 0x002c */
     uint32_t reserved0030;                       /* address offset: 0x0030 */
     uint32_t soc_con13;                          /* address offset: 0x0034 */
     uint32_t soc_con14;                          /* address offset: 0x0038 */
     uint32_t soc_con15;                          /* address offset: 0x003c */
     uint32_t soc_con16;                          /* address offset: 0x0040 */
     uint32_t soc_con17;                          /* address offset: 0x0044 */
     uint32_t soc_con18;                          /* address offset: 0x0048 */
     uint32_t soc_con19;                          /* address offset: 0x004c */
     uint32_t soc_con20;                          /* address offset: 0x0050 */
     uint32_t soc_con21;                          /* address offset: 0x0054 */
     uint32_t soc_con22;                          /* address offset: 0x0058 */
     uint32_t soc_con23;                          /* address offset: 0x005c */
     uint32_t soc_con24;                          /* address offset: 0x0060 */
     uint32_t soc_con25;                          /* address offset: 0x0064 */
     uint32_t soc_con26;                          /* address offset: 0x0068 */
     uint32_t soc_con27;                          /* address offset: 0x006c */
     uint32_t soc_con28;                          /* address offset: 0x0070 */
     uint32_t soc_con29;                          /* address offset: 0x0074 */
     uint32_t soc_con30;                          /* address offset: 0x0078 */
     uint32_t soc_con31;                          /* address offset: 0x007c */
     uint32_t soc_con32;                          /* address offset: 0x0080 */
     uint32_t soc_con33;                          /* address offset: 0x0084 */
     uint32_t reserved0088;                       /* address offset: 0x0088 */
     uint32_t soc_con35;                          /* address offset: 0x008c */
     uint32_t soc_con36;                          /* address offset: 0x0090 */
     uint32_t soc_con37;                          /* address offset: 0x0094 */
     uint32_t soc_con38;                          /* address offset: 0x0098 */
     uint32_t soc_con39;                          /* address offset: 0x009c */
     uint32_t soc_con40;                          /* address offset: 0x00a0 */
     uint32_t soc_con41;                          /* address offset: 0x00a4 */
     uint32_t soc_con42;                          /* address offset: 0x00a8 */
     uint32_t soc_con43;                          /* address offset: 0x00ac */
     uint32_t reserved00b0[20];                   /* address offset: 0x00b0 */
     uint32_t soc_status0;                        /* address offset: 0x0100 */
     uint32_t soc_status1;                        /* address offset: 0x0104 */
     uint32_t soc_status2;                        /* address offset: 0x0108 */
     uint32_t reserved010c;                       /* address offset: 0x010c */
     uint32_t ddr_status0;                        /* address offset: 0x0110 */
     uint32_t ddr_status1;                        /* address offset: 0x0114 */
     uint32_t usbphy_status;                      /* address offset: 0x0118 */
     uint32_t reserved011c[13];                   /* address offset: 0x011c */
     uint32_t usbotg0_sig_detect_con;             /* address offset: 0x0150 */
     uint32_t usbotg0_sig_detect_status;          /* address offset: 0x0154 */
     uint32_t usbotg0_sig_detect_clr;             /* address offset: 0x0158 */
     uint32_t usbotg0_vbusvalid_detect_con;       /* address offset: 0x015c */
     uint32_t usbotg0_linestate_detect_con;       /* address offset: 0x0160 */
     uint32_t usbotg0_disconnect_detect_con;      /* address offset: 0x0164 */
     uint32_t usbotg0_bvalid_detect_con;          /* address offset: 0x0168 */
     uint32_t usbotg0_id_detect_con;              /* address offset: 0x016c */
     uint32_t usbotg1_sig_detect_con;             /* address offset: 0x0170 */
     uint32_t usbotg1_sig_detect_status;          /* address offset: 0x0174 */
     uint32_t usbotg1_sig_detect_clr;             /* address offset: 0x0178 */
     uint32_t usbotg1_vbusvalid_detect_con;       /* address offset: 0x017c */
     uint32_t usbotg1_linestate_detect_con;       /* address offset: 0x0180 */
     uint32_t usbotg1_disconnect_detect_con;      /* address offset: 0x0184 */
     uint32_t usbotg1_bvalid_detect_con;          /* address offset: 0x0188 */
     uint32_t usbotg1_id_detect_con;              /* address offset: 0x018c */
     uint32_t reserved0190[4];                    /* address offset: 0x0190 */
     uint32_t mac0_mcgr_ack;                      /* address offset: 0x01a0 */
     uint32_t mac1_mcgr_ack;                      /* address offset: 0x01a4 */
     uint32_t reserved01a8[22];                   /* address offset: 0x01a8 */
     uint32_t os_reg0;                            /* address offset: 0x0200 */
     uint32_t os_reg1;                            /* address offset: 0x0204 */
     uint32_t os_reg2;                            /* address offset: 0x0208 */
     uint32_t os_reg3;                            /* address offset: 0x020c */
     uint32_t os_reg4;                            /* address offset: 0x0210 */
     uint32_t os_reg5;                            /* address offset: 0x0214 */
     uint32_t os_reg6;                            /* address offset: 0x0218 */
     uint32_t os_reg7;                            /* address offset: 0x021c */
     uint32_t os_reg8;                            /* address offset: 0x0220 */
     uint32_t os_reg9;                            /* address offset: 0x0224 */
     uint32_t os_reg10;                           /* address offset: 0x0228 */
     uint32_t os_reg11;                           /* address offset: 0x022c */
     uint32_t reserved0230[52];                   /* address offset: 0x0230 */
     uint32_t soc_version;                        /* address offset: 0x0300 */
};

check_member(rk3506_grf_reg, soc_version, 0x0300);

/* grf_core register structure define */
struct rk3506_grf_core_reg {
     uint32_t pvtpll_con0_l;                      /* address offset: 0x0000 */
     uint32_t pvtpll_con0_h;                      /* address offset: 0x0004 */
     uint32_t pvtpll_con1;                        /* address offset: 0x0008 */
     uint32_t pvtpll_con2;                        /* address offset: 0x000c */
     uint32_t pvtpll_con3;                        /* address offset: 0x0010 */
     uint32_t pvtpll_osc_cnt;                     /* address offset: 0x0014 */
     uint32_t pvtpll_osc_cnt_avg;                 /* address offset: 0x0018 */
     uint32_t reserved001c[17];                   /* address offset: 0x001c */
     uint32_t cpu_status;                         /* address offset: 0x0060 */
     uint32_t cpu_con0;                           /* address offset: 0x0064 */
     uint32_t cpu_con1;                           /* address offset: 0x0068 */
     uint32_t cpu_mem_con0;                       /* address offset: 0x006c */
     uint32_t reserved0070[5];                    /* address offset: 0x0070 */
     uint32_t soc_con0;                           /* address offset: 0x0084 */
     uint32_t soc_con1;                           /* address offset: 0x0088 */
     uint32_t soc_con2;                           /* address offset: 0x008c */
     uint32_t soc_con3;                           /* address offset: 0x0090 */
     uint32_t soc_con4;                           /* address offset: 0x0094 */
     uint32_t soc_con5;                           /* address offset: 0x0098 */
};

check_member(rk3506_grf_core_reg, soc_con5, 0x0098);

/* grf_pmu register structure define */
struct rk3506_grf_pmu_reg {
     uint32_t soc_con0;                           /* address offset: 0x0000 */
     uint32_t soc_con1;                           /* address offset: 0x0004 */
     uint32_t soc_con2;                           /* address offset: 0x0008 */
     uint32_t soc_con3;                           /* address offset: 0x000c */
     uint32_t soc_con4;                           /* address offset: 0x0010 */
     uint32_t soc_con5;                           /* address offset: 0x0014 */
     uint32_t soc_con6;                           /* address offset: 0x0018 */
     uint32_t soc_con7;                           /* address offset: 0x001c */
     uint32_t soc_con8;                           /* address offset: 0x0020 */
     uint32_t soc_con9;                           /* address offset: 0x0024 */
     uint32_t soc_con10;                          /* address offset: 0x0028 */
     uint32_t soc_con11;                          /* address offset: 0x002c */
     uint32_t soc_con12;                          /* address offset: 0x0030 */
     uint32_t soc_con13;                          /* address offset: 0x0034 */
     uint32_t soc_con14;                          /* address offset: 0x0038 */
     uint32_t soc_con15;                          /* address offset: 0x003c */
     uint32_t soc_con16;                          /* address offset: 0x0040 */
     uint32_t soc_con17;                          /* address offset: 0x0044 */
     uint32_t soc_con18;                          /* address offset: 0x0048 */
     uint32_t reserved004c[45];                   /* address offset: 0x004c */
     uint32_t soc_status;                         /* address offset: 0x0100 */
     uint32_t reserved0104[63];                   /* address offset: 0x0104 */
     uint32_t os_reg0;                            /* address offset: 0x0200 */
     uint32_t os_reg1;                            /* address offset: 0x0204 */
     uint32_t os_reg2;                            /* address offset: 0x0208 */
     uint32_t os_reg3;                            /* address offset: 0x020c */
     uint32_t os_reg4;                            /* address offset: 0x0210 */
     uint32_t os_reg5;                            /* address offset: 0x0214 */
     uint32_t os_reg6;                            /* address offset: 0x0218 */
     uint32_t os_reg7;                            /* address offset: 0x021c */
     uint32_t os_reg8;                            /* address offset: 0x0220 */
     uint32_t os_reg9;                            /* address offset: 0x0224 */
     uint32_t os_reg10;                           /* address offset: 0x0228 */
     uint32_t os_reg11;                           /* address offset: 0x022c */
     uint32_t rstfunc_status;                     /* address offset: 0x0230 */
     uint32_t rstfunc_clr;                        /* address offset: 0x0234 */
     uint32_t reserved0238[882];                  /* address offset: 0x0238 */
     uint32_t mcu_iso_con0;                       /* address offset: 0x1000 */
     uint32_t mcu_iso_con1;                       /* address offset: 0x1004 */
     uint32_t mcu_iso_con2;                       /* address offset: 0x1008 */
     uint32_t mcu_iso_con3;                       /* address offset: 0x100c */
     uint32_t mcu_iso_con4;                       /* address offset: 0x1010 */
     uint32_t mcu_iso_con5;                       /* address offset: 0x1014 */
     uint32_t mcu_iso_con6;                       /* address offset: 0x1018 */
     uint32_t mcu_iso_con7;                       /* address offset: 0x101c */
     uint32_t mcu_iso_con8;                       /* address offset: 0x1020 */
     uint32_t mcu_iso_con9;                       /* address offset: 0x1024 */
     uint32_t mcu_iso_con10;                      /* address offset: 0x1028 */
     uint32_t mcu_iso_con11;                      /* address offset: 0x102c */
     uint32_t reserved1030[244];                  /* address offset: 0x1030 */
     uint32_t mcu_iso_ddr_con0;                   /* address offset: 0x1400 */
     uint32_t mcu_iso_ddr_con1;                   /* address offset: 0x1404 */
     uint32_t reserved1408[62];                   /* address offset: 0x1408 */
     uint32_t mcu_iso_lock;                       /* address offset: 0x1500 */
     uint32_t reserved1504[703];                  /* address offset: 0x1504 */
     uint32_t cpu_iso_con0;                       /* address offset: 0x2000 */
     uint32_t cpu_iso_con1;                       /* address offset: 0x2004 */
     uint32_t cpu_iso_con2;                       /* address offset: 0x2008 */
     uint32_t cpu_iso_con3;                       /* address offset: 0x200c */
     uint32_t cpu_iso_con4;                       /* address offset: 0x2010 */
     uint32_t cpu_iso_con5;                       /* address offset: 0x2014 */
     uint32_t cpu_iso_con6;                       /* address offset: 0x2018 */
     uint32_t cpu_iso_con7;                       /* address offset: 0x201c */
     uint32_t cpu_iso_con8;                       /* address offset: 0x2020 */
     uint32_t cpu_iso_con9;                       /* address offset: 0x2024 */
     uint32_t cpu_iso_con10;                      /* address offset: 0x2028 */
     uint32_t cpu_iso_con11;                      /* address offset: 0x202c */
     uint32_t reserved2030[244];                  /* address offset: 0x2030 */
     uint32_t cpu_iso_ddr_con0;                   /* address offset: 0x2400 */
     uint32_t cpu_iso_ddr_con1;                   /* address offset: 0x2404 */
     uint32_t reserved2408[62];                   /* address offset: 0x2408 */
     uint32_t cpu_iso_lock;                       /* address offset: 0x2500 */
};

check_member(rk3506_grf_pmu_reg, cpu_iso_lock, 0x2500);

#endif /*  _ASM_ARCH_GRF_RK3506_H  */
