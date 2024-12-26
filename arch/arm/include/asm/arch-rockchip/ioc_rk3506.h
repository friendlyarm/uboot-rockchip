/*
 * (C) Copyright 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#ifndef _ASM_ARCH_GRF_RK3506_H
#define _ASM_ARCH_GRF_RK3506_H

#include <common.h>

/* gpio0_ioc register structure define */
struct rk3506_gpio0_ioc_reg {
     uint32_t gpio0a_iomux_sel_0;                 /* address offset: 0x0000 */
     uint32_t gpio0a_iomux_sel_1;                 /* address offset: 0x0004 */
     uint32_t gpio0b_iomux_sel_0;                 /* address offset: 0x0008 */
     uint32_t gpio0b_iomux_sel_1;                 /* address offset: 0x000c */
     uint32_t gpio0c_iomux_sel_0;                 /* address offset: 0x0010 */
     uint32_t gpio0c_iomux_sel_1;                 /* address offset: 0x0014 */
     uint32_t reserved0018[58];                   /* address offset: 0x0018 */
     uint32_t gpio0a_ds_0;                        /* address offset: 0x0100 */
     uint32_t gpio0a_ds_1;                        /* address offset: 0x0104 */
     uint32_t gpio0a_ds_2;                        /* address offset: 0x0108 */
     uint32_t gpio0a_ds_3;                        /* address offset: 0x010c */
     uint32_t gpio0b_ds_0;                        /* address offset: 0x0110 */
     uint32_t gpio0b_ds_1;                        /* address offset: 0x0114 */
     uint32_t gpio0b_ds_2;                        /* address offset: 0x0118 */
     uint32_t gpio0b_ds_3;                        /* address offset: 0x011c */
     uint32_t gpio0c_ds_0;                        /* address offset: 0x0120 */
     uint32_t gpio0c_ds_1;                        /* address offset: 0x0124 */
     uint32_t gpio0c_ds_2;                        /* address offset: 0x0128 */
     uint32_t gpio0c_ds_3;                        /* address offset: 0x012c */
     uint32_t reserved0130[52];                   /* address offset: 0x0130 */
     uint32_t gpio0a_pull;                        /* address offset: 0x0200 */
     uint32_t gpio0b_pull;                        /* address offset: 0x0204 */
     uint32_t gpio0c_pull;                        /* address offset: 0x0208 */
     uint32_t reserved020c[61];                   /* address offset: 0x020c */
     uint32_t gpio0a_ie;                          /* address offset: 0x0300 */
     uint32_t gpio0b_ie;                          /* address offset: 0x0304 */
     uint32_t gpio0c_ie;                          /* address offset: 0x0308 */
     uint32_t reserved030c[61];                   /* address offset: 0x030c */
     uint32_t gpio0a_smt;                         /* address offset: 0x0400 */
     uint32_t gpio0b_smt;                         /* address offset: 0x0404 */
     uint32_t gpio0c_smt;                         /* address offset: 0x0408 */
     uint32_t reserved040c[61];                   /* address offset: 0x040c */
     uint32_t gpio0a_sus;                         /* address offset: 0x0500 */
     uint32_t gpio0b_sus;                         /* address offset: 0x0504 */
     uint32_t gpio0c_sus;                         /* address offset: 0x0508 */
     uint32_t reserved050c[61];                   /* address offset: 0x050c */
     uint32_t gpio0a_sl;                          /* address offset: 0x0600 */
     uint32_t gpio0b_sl;                          /* address offset: 0x0604 */
     uint32_t gpio0c_sl;                          /* address offset: 0x0608 */
     uint32_t reserved060c[61];                   /* address offset: 0x060c */
     uint32_t gpio0a_od;                          /* address offset: 0x0700 */
     uint32_t gpio0b_od;                          /* address offset: 0x0704 */
     uint32_t gpio0c_od;                          /* address offset: 0x0708 */
     uint32_t reserved070c[61];                   /* address offset: 0x070c */
     uint32_t gpio0_iddq;                         /* address offset: 0x0800 */
     uint32_t reserved0804[11];                   /* address offset: 0x0804 */
     uint32_t gpio0d_con;                         /* address offset: 0x0830 */
};

check_member(rk3506_gpio0_ioc_reg, gpio0d_con, 0x0830);

/* gpio1_ioc register structure define */
struct rk3506_gpio1_ioc_reg {
     uint32_t reserved0000[8];                    /* address offset: 0x0000 */
     uint32_t gpio1a_iomux_sel_0;                 /* address offset: 0x0020 */
     uint32_t gpio1a_iomux_sel_1;                 /* address offset: 0x0024 */
     uint32_t gpio1b_iomux_sel_0;                 /* address offset: 0x0028 */
     uint32_t gpio1b_iomux_sel_1;                 /* address offset: 0x002c */
     uint32_t gpio1c_iomux_sel_0;                 /* address offset: 0x0030 */
     uint32_t gpio1c_iomux_sel_1;                 /* address offset: 0x0034 */
     uint32_t gpio1d_iomux_sel_0;                 /* address offset: 0x0038 */
     uint32_t reserved003c[65];                   /* address offset: 0x003c */
     uint32_t gpio1a_ds_0;                        /* address offset: 0x0140 */
     uint32_t gpio1a_ds_1;                        /* address offset: 0x0144 */
     uint32_t gpio1a_ds_2;                        /* address offset: 0x0148 */
     uint32_t gpio1a_ds_3;                        /* address offset: 0x014c */
     uint32_t gpio1b_ds_0;                        /* address offset: 0x0150 */
     uint32_t gpio1b_ds_1;                        /* address offset: 0x0154 */
     uint32_t gpio1b_ds_2;                        /* address offset: 0x0158 */
     uint32_t gpio1b_ds_3;                        /* address offset: 0x015c */
     uint32_t gpio1c_ds_0;                        /* address offset: 0x0160 */
     uint32_t gpio1c_ds_1;                        /* address offset: 0x0164 */
     uint32_t gpio1c_ds_2;                        /* address offset: 0x0168 */
     uint32_t gpio1c_ds_3;                        /* address offset: 0x016c */
     uint32_t gpio1d_ds_0;                        /* address offset: 0x0170 */
     uint32_t gpio1d_ds_1;                        /* address offset: 0x0174 */
     uint32_t reserved0178[38];                   /* address offset: 0x0178 */
     uint32_t gpio1a_pull;                        /* address offset: 0x0210 */
     uint32_t gpio1b_pull;                        /* address offset: 0x0214 */
     uint32_t gpio1c_pull;                        /* address offset: 0x0218 */
     uint32_t gpio1d_pull;                        /* address offset: 0x021c */
     uint32_t reserved0220[60];                   /* address offset: 0x0220 */
     uint32_t gpio1a_ie;                          /* address offset: 0x0310 */
     uint32_t gpio1b_ie;                          /* address offset: 0x0314 */
     uint32_t gpio1c_ie;                          /* address offset: 0x0318 */
     uint32_t gpio1d_ie;                          /* address offset: 0x031c */
     uint32_t reserved0320[60];                   /* address offset: 0x0320 */
     uint32_t gpio1a_smt;                         /* address offset: 0x0410 */
     uint32_t gpio1b_smt;                         /* address offset: 0x0414 */
     uint32_t gpio1c_smt;                         /* address offset: 0x0418 */
     uint32_t gpio1d_smt;                         /* address offset: 0x041c */
     uint32_t reserved0420[60];                   /* address offset: 0x0420 */
     uint32_t gpio1a_sus;                         /* address offset: 0x0510 */
     uint32_t gpio1b_sus;                         /* address offset: 0x0514 */
     uint32_t gpio1c_sus;                         /* address offset: 0x0518 */
     uint32_t gpio1d_sus;                         /* address offset: 0x051c */
     uint32_t reserved0520[60];                   /* address offset: 0x0520 */
     uint32_t gpio1a_sl;                          /* address offset: 0x0610 */
     uint32_t gpio1b_sl;                          /* address offset: 0x0614 */
     uint32_t gpio1c_sl;                          /* address offset: 0x0618 */
     uint32_t gpio1d_sl;                          /* address offset: 0x061c */
     uint32_t reserved0620[60];                   /* address offset: 0x0620 */
     uint32_t gpio1a_od;                          /* address offset: 0x0710 */
     uint32_t gpio1b_od;                          /* address offset: 0x0714 */
     uint32_t gpio1c_od;                          /* address offset: 0x0718 */
     uint32_t gpio1d_od;                          /* address offset: 0x071c */
     uint32_t reserved0720[60];                   /* address offset: 0x0720 */
     uint32_t gpio1_iddq;                         /* address offset: 0x0810 */
};

check_member(rk3506_gpio1_ioc_reg, gpio1_iddq, 0x0810);

/* gpio2_ioc register structure define */
struct rk3506_gpio2_ioc_reg {
     uint32_t reserved0000[16];                   /* address offset: 0x0000 */
     uint32_t gpio2a_iomux_sel_0;                 /* address offset: 0x0040 */
     uint32_t gpio2a_iomux_sel_1;                 /* address offset: 0x0044 */
     uint32_t gpio2b_iomux_sel_0;                 /* address offset: 0x0048 */
     uint32_t gpio2b_iomux_sel_1;                 /* address offset: 0x004c */
     uint32_t gpio2c_iomux_sel_0;                 /* address offset: 0x0050 */
     uint32_t reserved0054[75];                   /* address offset: 0x0054 */
     uint32_t gpio2a_ds_0;                        /* address offset: 0x0180 */
     uint32_t gpio2a_ds_1;                        /* address offset: 0x0184 */
     uint32_t gpio2a_ds_2;                        /* address offset: 0x0188 */
     uint32_t reserved018c;                       /* address offset: 0x018c */
     uint32_t gpio2b_ds_0;                        /* address offset: 0x0190 */
     uint32_t gpio2b_ds_1;                        /* address offset: 0x0194 */
     uint32_t gpio2b_ds_2;                        /* address offset: 0x0198 */
     uint32_t gpio2b_ds_3;                        /* address offset: 0x019c */
     uint32_t gpio2c_ds_0;                        /* address offset: 0x01a0 */
     uint32_t reserved01a4[31];                   /* address offset: 0x01a4 */
     uint32_t gpio2a_pull;                        /* address offset: 0x0220 */
     uint32_t gpio2b_pull;                        /* address offset: 0x0224 */
     uint32_t gpio2c_pull;                        /* address offset: 0x0228 */
     uint32_t reserved022c[61];                   /* address offset: 0x022c */
     uint32_t gpio2a_ie;                          /* address offset: 0x0320 */
     uint32_t gpio2b_ie;                          /* address offset: 0x0324 */
     uint32_t gpio2c_ie;                          /* address offset: 0x0328 */
     uint32_t reserved032c[61];                   /* address offset: 0x032c */
     uint32_t gpio2a_smt;                         /* address offset: 0x0420 */
     uint32_t gpio2b_smt;                         /* address offset: 0x0424 */
     uint32_t gpio2c_smt;                         /* address offset: 0x0428 */
     uint32_t reserved042c[61];                   /* address offset: 0x042c */
     uint32_t gpio2a_sus;                         /* address offset: 0x0520 */
     uint32_t gpio2b_sus;                         /* address offset: 0x0524 */
     uint32_t gpio2c_sus;                         /* address offset: 0x0528 */
     uint32_t reserved052c[61];                   /* address offset: 0x052c */
     uint32_t gpio2a_sl;                          /* address offset: 0x0620 */
     uint32_t gpio2b_sl;                          /* address offset: 0x0624 */
     uint32_t gpio2c_sl;                          /* address offset: 0x0628 */
     uint32_t reserved062c[61];                   /* address offset: 0x062c */
     uint32_t gpio2a_od;                          /* address offset: 0x0720 */
     uint32_t gpio2b_od;                          /* address offset: 0x0724 */
     uint32_t gpio2c_od;                          /* address offset: 0x0728 */
     uint32_t reserved072c[61];                   /* address offset: 0x072c */
     uint32_t gpio2_iddq;                         /* address offset: 0x0820 */
};

check_member(rk3506_gpio2_ioc_reg, gpio2_iddq, 0x0820);

/* gpio3_ioc register structure define */
struct rk3506_gpio3_ioc_reg {
     uint32_t reserved0000[24];                   /* address offset: 0x0000 */
     uint32_t gpio3a_iomux_sel_0;                 /* address offset: 0x0060 */
     uint32_t gpio3a_iomux_sel_1;                 /* address offset: 0x0064 */
     uint32_t gpio3b_iomux_sel_0;                 /* address offset: 0x0068 */
     uint32_t gpio3b_iomux_sel_1;                 /* address offset: 0x006c */
     uint32_t reserved0070[84];                   /* address offset: 0x0070 */
     uint32_t gpio3a_ds_0;                        /* address offset: 0x01c0 */
     uint32_t gpio3a_ds_1;                        /* address offset: 0x01c4 */
     uint32_t gpio3a_ds_2;                        /* address offset: 0x01c8 */
     uint32_t gpio3a_ds_3;                        /* address offset: 0x01cc */
     uint32_t gpio3b_ds_0;                        /* address offset: 0x01d0 */
     uint32_t gpio3b_ds_1;                        /* address offset: 0x01d4 */
     uint32_t gpio3b_ds_2;                        /* address offset: 0x01d8 */
     uint32_t gpio3b_ds_3;                        /* address offset: 0x01dc */
     uint32_t reserved01e0[20];                   /* address offset: 0x01e0 */
     uint32_t gpio3a_pull;                        /* address offset: 0x0230 */
     uint32_t gpio3b_pull;                        /* address offset: 0x0234 */
     uint32_t reserved0238[62];                   /* address offset: 0x0238 */
     uint32_t gpio3a_ie;                          /* address offset: 0x0330 */
     uint32_t gpio3b_ie;                          /* address offset: 0x0334 */
     uint32_t reserved0338[62];                   /* address offset: 0x0338 */
     uint32_t gpio3a_smt;                         /* address offset: 0x0430 */
     uint32_t gpio3b_smt;                         /* address offset: 0x0434 */
     uint32_t reserved0438[62];                   /* address offset: 0x0438 */
     uint32_t gpio3a_sus;                         /* address offset: 0x0530 */
     uint32_t gpio3b_sus;                         /* address offset: 0x0534 */
     uint32_t reserved0538[62];                   /* address offset: 0x0538 */
     uint32_t gpio3a_sl;                          /* address offset: 0x0630 */
     uint32_t gpio3b_sl;                          /* address offset: 0x0634 */
     uint32_t reserved0638[62];                   /* address offset: 0x0638 */
     uint32_t gpio3a_od;                          /* address offset: 0x0730 */
     uint32_t gpio3b_od;                          /* address offset: 0x0734 */
     uint32_t reserved0738[58];                   /* address offset: 0x0738 */
     uint32_t gpio3_iddq;                         /* address offset: 0x0820 */
};

check_member(rk3506_gpio3_ioc_reg, gpio3_iddq, 0x0820);

/* gpio4_ioc register structure define */
struct rk3506_gpio4_ioc_reg {
     uint32_t reserved0000[528];                  /* address offset: 0x0000 */
     uint32_t saradc_con;                         /* address offset: 0x0840 */
};

check_member(rk3506_gpio4_ioc_reg, saradc_con, 0x0840);

#endif /*  _ASM_ARCH_GRF_RK3506_H  */
