// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//	  Redistribution and use in source and binary forms, with or without
//	  modification, are permitted provided that the following conditions
//	  are met:
//
//	    Redistributions of source code must retain the above copyright
//	    notice, this list of conditions and the following disclaimer.
//
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the
//	    documentation and/or other materials provided with the
//	    distribution.
//
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Basic display functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "display.h"

// Table with memory address for each display element
const uint8_t * segments_lcdmem[] = {
	LCD_SYMB_AM_MEM,
	LCD_SYMB_PM_MEM,
	LCD_SYMB_ARROW_UP_MEM,
	LCD_SYMB_ARROW_DOWN_MEM,
	LCD_SYMB_PERCENT_MEM,
	LCD_SYMB_TOTAL_MEM,
	LCD_SYMB_AVERAGE_MEM,
	LCD_SYMB_MAX_MEM,
	LCD_SYMB_BATTERY_MEM,
	LCD_UNIT_L1_FT_MEM,
	LCD_UNIT_L1_K_MEM,
	LCD_UNIT_L1_M_MEM,
	LCD_UNIT_L1_I_MEM,
	LCD_UNIT_L1_PER_S_MEM,
	LCD_UNIT_L1_PER_H_MEM,
	LCD_UNIT_L1_DEGREE_MEM,
	LCD_UNIT_L2_KCAL_MEM,
	LCD_UNIT_L2_KM_MEM,
	LCD_UNIT_L2_MI_MEM,
	LCD_ICON_HEART_MEM,
	LCD_ICON_STOPWATCH_MEM,
	LCD_ICON_RECORD_MEM,
	LCD_ICON_ALARM_MEM,
	LCD_ICON_BEEPER1_MEM,
	LCD_ICON_BEEPER2_MEM,
	LCD_ICON_BEEPER3_MEM,
	LCD_SEG_L1_3_MEM,
	LCD_SEG_L1_2_MEM,
	LCD_SEG_L1_1_MEM,
	LCD_SEG_L1_0_MEM,
	LCD_SEG_L1_COL_MEM,
	LCD_SEG_L1_DP1_MEM,
	LCD_SEG_L1_DP0_MEM,
	LCD_SEG_L2_5_MEM,
	LCD_SEG_L2_4_MEM,
	LCD_SEG_L2_3_MEM,
	LCD_SEG_L2_2_MEM,
	LCD_SEG_L2_1_MEM,
	LCD_SEG_L2_0_MEM,
	LCD_SEG_L2_COL1_MEM,
	LCD_SEG_L2_COL0_MEM,
	LCD_SEG_L2_DP_MEM,
};

// Table with bit mask for each display element
const uint8_t segments_bitmask[] = {
	(uint8_t)LcdBitMask::LCD_SYMB_AM_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_PM_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_ARROW_UP_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_ARROW_DOWN_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_PERCENT_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_TOTAL_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_AVERAGE_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_MAX_MASK,
	(uint8_t)LcdBitMask::LCD_SYMB_BATTERY_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_FT_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_K_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_M_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_I_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_PER_S_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_PER_H_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L1_DEGREE_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L2_KCAL_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L2_KM_MASK,
	(uint8_t)LcdBitMask::LCD_UNIT_L2_MI_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_HEART_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_STOPWATCH_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_RECORD_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_ALARM_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_BEEPER1_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_BEEPER2_MASK,
	(uint8_t)LcdBitMask::LCD_ICON_BEEPER3_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_3_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_2_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_1_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_0_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_COL_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_DP1_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L1_DP0_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_5_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_4_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_3_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_2_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_1_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_0_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_COL1_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_COL0_MASK,
	(uint8_t)LcdBitMask::LCD_SEG_L2_DP_MASK,
};