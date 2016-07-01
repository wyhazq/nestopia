////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2007 Martin Freij
//
// This file is part of Nestopia.
//
// Nestopia is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Nestopia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Nestopia; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////////////

#include "../NstMapper.hpp"
#include "NstMapper061.hpp"

namespace Nes
{
    namespace Core
    {
        
        
        
        
        void Mapper61::SubReset(const bool hard)
        {
            Map( 0x8000U, 0xFFFFU, &Mapper61::Poke_Prg );
            
            if (hard)
                Poke_Prg(this,0x8000,0x00);
        }
        
        
        
        
        
        void Mapper61::Poke_Prg(void* p_,Address i_,Data j_) { static_cast<Mapper61*>(p_)->Poke_M_Prg(i_,j_); } inline void Mapper61::Poke_M_Prg(Address address,Data)
        {
            ppu.SetMirroring( (address & 0x80) ? Ppu::NMT_HORIZONTAL : Ppu::NMT_VERTICAL );
            
            switch (address & 0x30)
            {
                case 0x00:
                case 0x30:
                    
                    prg.SwapBank<SIZE_32K,0x0000>( address & 0xF );
                    break;
                    
                case 0x20:
                case 0x10:
                    
                    address = (address << 1 & 0x1E) | (address >> 4 & 0x02);
                    prg.SwapBanks<SIZE_16K,0x0000>( address, address );
                    break;
            }
        }
    }
}
