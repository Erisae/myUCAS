`ifndef MYCPU_H
    `define MYCPU_H

    `define BR_BUS_WD         35
    `define FS_TO_DS_BUS_WD   75
    `define DS_TO_ES_BUS_WD   179
    `define ES_TO_MS_BUS_WD   147
    `define MS_TO_WS_BUS_WD   175
    `define WS_TO_RF_BUS_WD   41
    `define ES_FORWARDING_BUS_WD 43
    `define MS_FORWARDING_BUS_WD 45
    `define WS_FORWARDING_BUS_WD 107
    `define WS_TO_FS_BUS_WD   88
    `define WS_TO_ES_BUS_WD   23
    `define FS_TLB_BUS_WD     20   
    `define ES_TLB_BUS_WD     21
    `define CP0_STATUS   12
    `define CP0_CAUSE    13
    `define CP0_EPC      14
    `define CP0_COUNT     9
    `define CP0_COMPARE  11
    `define CP0_BADVADDR  8
    `define CP0_ENTRYHI  10
    `define CP0_INDEX     0
    `define CP0_ENTRYLO0  2
    `define CP0_ENTRYLO1  3
    `define Int   0
    `define AdEL  4
    `define AdES  5
    `define Ov    12
    `define Sys   8
    `define Bp    9
    `define Ri    10
    `define Mod   1
    `define Tlbl  2
    `define Tlbs  3
`endif
