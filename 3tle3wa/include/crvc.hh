/**
 * @file crvc.hh
 * @author 3TLE3WA (https://github.com/3TLE3WA)
 * @brief 本文件目前用于占位和说明，注意，代码中的任何注释都要关注，不要轻易删除
 * @version 0.1
 * @date 2023-06-25
 *
 * @copyright Copyright (c) 2023
 *
 * @attention 没有该文件，测评机可能会无法识别并加入参数 -I include
 */

/*
黑魔法优化 1
源代码如下

源自 fpga 开发经验
编译由 gcc 完成

目前不实现，缺少的东西过多
本部分应该交给前端

union flt {
  struct {
    unsigned mas: 23;
    unsigned exp: 8;
    unsigned sign: 1;
  };
  float f;
} cvt ;

float __crvc_flt_sra(float a, constant_2pow b) {
  union flt ai, bi, ci;
  ai.f = a;
  bi.f = b;
  ci.mas = ai.mas;
  int m = bi.exp - 127;
  int texp = ai.exp - m;
  if (texp > 255) {
    texp = 255;
  } else if (texp < 0) {
    texp = 0;
  }
  ci.exp = texp;
  ci.sign = ai.sign ^ bi.sign;
  return ci.f;
}

float __crvc_flt_sll(float a, constant_2pow b) {
  union flt ai, bi, ci;
  ai.f = a;
  bi.f = b;
  ci.mas = ai.mas;
  int m = bi.exp - 127;
  int texp = ai.exp + m;
  if (texp > 255) {
    texp = 255;
  } else if (texp < 0) {
    texp = 0;
  }
  ci.exp = texp;
  ci.sign = ai.sign ^ bi.sign;
  return ci.f;
}

__crvc_flt_sra:
   fmv.x.w   a2,fa1
   fmv.x.w   a4,fa0
   srliw     a5,a2,23
   srliw     a3,a4,23
   andi      a5,a5,0xff
   andi      a3,a3,0xff
   addiw     a5,a5,-127
   subw      a5,a3,a5
   mv        a3,a5
   bge       a5,zero,.L2
   li        a3,0
   li        a5,0
.L2:
   li        a1,255
   ble       a5,a1,.L3
   li        a3,255
.L3:
   slli      a5,a4,41
   slliw     a3,a3,23
   srli      a5,a5,41
   or        a5,a5,a3
   srliw     a2,a2,31
   srliw     a4,a4,31
   xor       a4,a4,a2
   slli      a5,a5,33
   slliw     a4,a4,31
   srli      a5,a5,33
   or        a5,a5,a4
   fmv.w.x   fa0,a5
   ret

__crvc_flt_sll:
   fmv.x.w   a2,fa1
   fmv.x.w   a4,fa0
   srliw     a3,a2,23
   andi      a3,a3,0xff
   srliw     a5,a4,23
   addiw     a3,a3,-127
   andi      a5,a5,0xff
   addw      a5,a3,a5
   mv        a3,a5
   bge       a5,zero,.L6
   li        a3,0
   li        a5,0
.L6:
   li        a1,255
   ble       a5,a1,.L7
   li        a3,255
.L7:
   slli      a5,a4,41
   slliw     a3,a3,23
   srli      a5,a5,41
   or        a5,a5,a3
   srliw     a2,a2,31
   srliw     a4,a4,31
   xor       a4,a4,a2
   slli      a5,a5,33
   slliw     a4,a4,31
   srli      a5,a5,33
   or        a5,a5,a4
   fmv.w.x   fa0,a5
   ret
*/
