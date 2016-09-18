/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 *******************************************************************************/

//On zOS XLC linker can't handle files with same name at link time
//This workaround with pragma is needed. What this does is essentially
//give a different name to the codesection (csect) for this file. So it
//doesn't conflict with another file with same name.
#pragma csect(CODE,"TRZRealRegBase#C")
#pragma csect(STATIC,"TRZRealRegtBase#S")
#pragma csect(TEST,"TRZRealRegBase#T")

#include <stddef.h>                        // for NULL
#include <stdint.h>                        // for uint32_t, int32_t, uint8_t, etc
#include "codegen/RealRegister.hpp"        // for RealRegister, etc
#include "codegen/Register.hpp"            // for Register
#include "codegen/RegisterConstants.hpp"   // for TR_RegisterKinds, etc
#include "infra/Assert.hpp"                // for TR_ASSERT
#include "z/codegen/EndianConversion.hpp"  // for boi

namespace TR { class CodeGenerator; }

OMR::Z::RealRegister::RealRegister(TR::CodeGenerator *cg):
      OMR::RealRegister(cg, NoReg),
      _modified(0) {}

OMR::Z::RealRegister::RealRegister(TR_RegisterKinds rk, uint16_t w, RegState s,
                                          RegNum rn, RegMask m, TR::CodeGenerator* cg):
      OMR::RealRegister(rk, w, s, (uint16_t)rn, rn, m, cg),
      _modified(0) {}


void
OMR::Z::RealRegister::setBaseRegisterField(uint32_t *instruction)
   {
   TR::RealRegister::setBaseRegisterField(instruction,_registerNumber);
   }

void
OMR::Z::RealRegister::setIndexRegisterField(uint32_t *instruction)
   {
   TR::RealRegister::setIndexRegisterField(instruction,_registerNumber);
   }

void
OMR::Z::RealRegister::setRegisterField(uint32_t *instruction)
   {
   TR::RealRegister::setRegisterField(instruction,_registerNumber);
   }

// nibbleIndex-> rightmost nibble=0, leftmost nibble=7
void
OMR::Z::RealRegister::setRegisterField(uint32_t *instruction, int32_t nibbleIndex)
   {
   TR::RealRegister::setRegisterField(instruction,nibbleIndex,_registerNumber);
   }

void
OMR::Z::RealRegister::setRegister1Field(uint32_t *instruction)
   {
   self()->setRegisterField(instruction);
   }

void
OMR::Z::RealRegister::setRegister2Field(uint32_t *instruction)
   {
   self()->setIndexRegisterField(instruction);
   }

void
OMR::Z::RealRegister::setRegister3Field(uint32_t *instruction)
   {
   self()->setBaseRegisterField(instruction);
   }

void
OMR::Z::RealRegister::setRegister4Field(uint32_t *instruction)
   {
   TR::RealRegister::setRegister4Field(instruction, _registerNumber);
   }


TR::RealRegister *
OMR::Z::RealRegister::getHighWordRegister()
   {
   if (_registerNumber>=FirstGPR && _registerNumber<=LastGPR)
      {
      return _highWordRegister;
      }
   if (_registerNumber>=FirstHPR && _registerNumber<=LastHPR)
      {
      return self();
      }
   else
      {
      return NULL;
      }
   }

TR::RealRegister *
OMR::Z::RealRegister::getLowWordRegister()
      {
      if (_registerNumber>=FirstGPR && _registerNumber<=LastGPR)
         {
         return self();
         }
      if (_registerNumber>=FirstHPR && _registerNumber<=LastHPR)
         {
         return _lowWordRegister;
         }
      else
         {
         return NULL;
         }
      }

TR::RealRegister *
OMR::Z::RealRegister::getSiblingWordRegister()
      {
      if (_registerNumber>=FirstGPR && _registerNumber<=LastGPR)
         {
         return _highWordRegister;
         }
      if (_registerNumber>=FirstHPR && _registerNumber<=LastHPR)
         {
         return _lowWordRegister;
         }
      else
         return NULL;
      }

bool
OMR::Z::RealRegister::setHasBeenAssignedInMethod(bool b)
   {
   if (self()->getAssignedRegister() && (self()->getAssignedRegister()->isUpperBitsAreDirty() || self()->getAssignedRegister()->getKind() == TR_GPR64) )
      (b)? self()->setAssignedHigh(true) : self()->setAssignedHigh(false);

   return OMR::RealRegister::setHasBeenAssignedInMethod(b); //call base class
   }

bool
OMR::Z::RealRegister::isHighWordRegister()
   {
   return self() == self()->getHighWordRegister();
   }

bool
OMR::Z::RealRegister::isLowWordRegister()
   {
   return self() == self()->getLowWordRegister();
   }


// static method
void
OMR::Z::RealRegister::setRegisterRXBField(uint32_t *instruction, RegNum reg, int index)
   {
   TR_ASSERT(TR::RealRegister::isVRF(reg), "Only VRF Register has RXB field.");
   TR_ASSERT(index >= 1 && index <= 4, "index out of range.");
   uint32_t RXB = boi( (_fullRegBinaryEncodings[reg] & 0x10) >> 4 << (4 - index) << 24 );
   *(instruction + 1) |= RXB; // RXB field is in the lower 4 bits of fifth byte
   }

// static method
void
OMR::Z::RealRegister::setBaseRegisterField(uint32_t *instruction,RegNum reg)
   {
   *instruction &= boi(0xFFFF0FFF); // clear out the memory first
   *instruction |= boi( (_fullRegBinaryEncodings[reg] & 0x0f) << 12);
   if (TR::RealRegister::isVRF(reg))
      TR::RealRegister::setRegisterRXBField(instruction, reg, 3); // base reg is in the 3rd register field
   }

// static method
void
OMR::Z::RealRegister::setIndexRegisterField(uint32_t *instruction,RegNum reg)
   {
   *instruction &= boi(0xFFF0FFFF); // clear out the memory first
   *instruction |= boi( (_fullRegBinaryEncodings[reg] & 0x0f) << 16);
   if (TR::RealRegister::isVRF(reg))
      TR::RealRegister::setRegisterRXBField(instruction, reg, 2); // index reg is in the 2nd register field
   }

// static method
void
OMR::Z::RealRegister::setRegisterField(uint32_t *instruction,RegNum reg)
   {
   *instruction &= boi(0xFF0FFFFF); // clear out the memory first
   *instruction |= boi( (_fullRegBinaryEncodings[reg] & 0x0f) << 20);
   if (TR::RealRegister::isVRF(reg))
      TR::RealRegister::setRegisterRXBField(instruction, reg, 1); // 1st register field
   }

// static method
void
OMR::Z::RealRegister::setRegisterField(uint32_t *instruction, int32_t nibbleIndex,RegNum reg)
   {
   TR_ASSERT(nibbleIndex>=0 && nibbleIndex<=7,
     "OMR::Z::RealRegister::RealRegister: bad nibbleIndex\n");

   uint32_t maskInstruction = *instruction&(0xF<<nibbleIndex*4);
   *instruction ^= boi(maskInstruction); // clear out the memory of byte
   *instruction |= boi((_fullRegBinaryEncodings[reg] & 0x0f) << nibbleIndex*4);
   }

// static method
void
OMR::Z::RealRegister::setRegister1Field(uint32_t *instruction,RegNum reg)
   {
   TR::RealRegister::setRegisterField(instruction,reg);
   }

// static method
void
OMR::Z::RealRegister::setRegister2Field(uint32_t *instruction,RegNum reg)
   {
   TR::RealRegister::setIndexRegisterField(instruction,reg);
   }

// static method
void
OMR::Z::RealRegister::setRegister3Field(uint32_t *instruction,RegNum reg)
   {
   TR::RealRegister::setBaseRegisterField(instruction,reg);
   }

// static method
void
OMR::Z::RealRegister::setRegister4Field(uint32_t *instruction,RegNum reg)
   {
   TR_ASSERT(TR::RealRegister::isVRF(reg), "setRegister4Field is only for vector registers");
   TR::RealRegister::setRegisterField(instruction + 1, 7, reg); // the 4th reg field is in the 1st 4-bit of the 3rd halfword of the instuction
   TR::RealRegister::setRegisterRXBField(instruction, reg, 4);
   }

// static method
bool
OMR::Z::RealRegister::isPseudoRealReg(RegNum reg)
  {
  if (reg < FirstGPR                              ||
      (reg > LastAssignableGPR && reg < FirstFPR) ||
       reg > LastAssignableFPR)
     {
     return true;
     }
  else
     {
    return false;
    }
  }

// static method
bool
OMR::Z::RealRegister::isRealReg(RegNum reg)
   {
   return !TR::RealRegister::isPseudoRealReg(reg);
   }

// static method
bool
OMR::Z::RealRegister::isGPR(RegNum reg)
   {
   if (reg >= FirstGPR && reg <= LastAssignableGPR)
       return true;
   else
      return false;
   }

// static method
bool
OMR::Z::RealRegister::isFPR(RegNum reg)
   {
   if(reg >= FirstFPR && reg <= LastAssignableFPR)
     return true;
   else
     return false;
   }

// static method
bool
OMR::Z::RealRegister::isAR(RegNum reg)
   {
   if(reg >= FirstAR && reg <= LastAR)
      return true;
   else
      return false;
   }

// static method
bool
OMR::Z::RealRegister::isHPR(RegNum reg)
   {
   if(reg >= FirstHPR && reg <= LastHPR)
      return true;
   else
      return false;
   }

// static method
bool
OMR::Z::RealRegister::isVRF(RegNum reg)
   {
   if(reg >= FirstVRF && reg <= LastVRF)
      return true;
   else
      return false;
   }

// static method
uint64_t
OMR::Z::RealRegister::getBitMask(RegNum reg)
   {
   if (TR::RealRegister::isGPR(reg))
      return 0x1 << (reg - FirstGPR);
   else if (TR::RealRegister::isHPR(reg))
      return 0x1 << (reg - FirstHPR + LastGPR);
   else if (TR::RealRegister::isAR(reg))
      return 0x1 << (reg - FirstAR);
   else if (TR::RealRegister::isFPR(reg))
      return 0x1 << (reg - FirstFPR);
   else if (TR::RealRegister::isVRF(reg))
      return 0x1 << (reg - FirstVRF);
   else
      return 0;
   }

// static method
uint64_t
OMR::Z::RealRegister::getBitMask(int32_t regNum)
   {
   return TR::RealRegister::getBitMask((TR::RealRegister::RegNum)regNum);
   }


const uint8_t OMR::Z::RealRegister::_fullRegBinaryEncodings[TR::RealRegister::NumRegisters] =
   {
   0x00,        // NoReg
   0x00,        // GPR0
   0x01,        // GPR1
   0x02,        // GPR2
   0x03,        // GPR3
   0x04,        // GPR4
   0x05,        // GPR5
   0x06,        // GPR6
   0x07,        // GPR7
   0x08,        // GPR8
   0x09,        // GPR9
   0x0A,        // GPR10
   0x0B,        // GPR11
   0x0C,        // GPR12
   0x0D,        // GPR13
   0x0E,        // GPR14
   0x0F,        // GPR15
   0x00,        // FPR0
   0x01,        // FPR1
   0x02,        // FPR2
   0x03,        // FPR3
   0x04,        // FPR4
   0x05,        // FPR5
   0x06,        // FPR6
   0x07,        // FPR7
   0x08,        // FPR8
   0x09,        // FPR9
   0x0A,        // FPR10
   0x0B,        // FPR11
   0x0C,        // FPR12
   0x0D,        // FPR13
   0x0E,        // FPR14
   0x0F,        // FPR15
/* VRF0-15 initialized same as FPR */
/*
   0x00,        // VRF0
   0x01,        // VRF1
   0x02,        // VRF2
   0x03,        // VRF3
   0x04,        // VRF4
   0x05,        // VRF5
   0x06,        // VRF6
   0x07,        // VRF7
   0x08,        // VRF8
   0x09,        // VRF9
   0x0A,        // VRF10
   0x0B,        // VRF11
   0x0C,        // VRF12
   0x0D,        // VRF13
   0x0E,        // VRF14
   0x0F,        // VRF15
*/
   0x10,        // VRF16
   0x11,        // VRF17
   0x12,        // VRF18
   0x13,        // VRF19
   0x14,        // VRF20
   0x15,        // VRF21
   0x16,        // VRF22
   0x17,        // VRF23
   0x18,        // VRF24
   0x19,        // VRF25
   0x1A,        // VRF26
   0x1B,        // VRF27
   0x1C,        // VRF28
   0x1D,        // VRF29
   0x1E,        // VRF30
   0x1F,        // VRF31

   0x00,        // AR0
   0x01,        // AR1
   0x02,        // AR2
   0x03,        // AR3
   0x04,        // AR4
   0x05,        // AR5
   0x06,        // AR6
   0x07,        // AR7
   0x08,        // AR8
   0x09,        // AR9
   0x0A,        // AR10
   0x0B,        // AR11
   0x0C,        // AR12
   0x0D,        // AR13
   0x0E,        // AR14
   0x0F,        // AR15
   0x00,        // HPR0
   0x01,        // HPR1
   0x02,        // HPR2
   0x03,        // HPR3
   0x04,        // HPR4
   0x05,        // HPR5
   0x06,        // HPR6
   0x07,        // HPR7
   0x08,        // HPR8
   0x09,        // HPR9
   0x0A,        // HPR10
   0x0B,        // HPR11
   0x0C,        // HPR12
   0x0D,        // HPR13
   0x0E,        // HPR14
   0x0F,        // HPR15
   };
