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
#include "../NstClock.hpp"
#include "NstBrdTf1201.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Boards
        {
            
            
            
            
            Tf1201::Tf1201(Context& c)
            :
            Mapper (c,CROM_MAX_256K),
            irq (c.cpu,c.ppu)
            {
            }
            
            void Tf1201::Irq::Reset(bool)
            {
                enabled = false;
                count = 0;
            }
            
            void Tf1201::SubReset(const bool hard)
            {
                irq.Reset( true, true );
                
                if (hard)
                    prgSelect = 0;
                
                for (uint i=0x0000; i < 0x1000; i += 0x4)
                {
                    Map( 0x8000 + i, &Tf1201::Poke_8000 );
                    Map( 0x9000 + i, NMT_SWAP_HV );
                    Map( 0x9001 + i, &Tf1201::Poke_9001 );
                    Map( 0xA000 + i, PRG_SWAP_8K_1 );
                    Map( 0xF000 + i, &Tf1201::Poke_F000 );
                    Map( 0xF001 + i, &Tf1201::Poke_F001 );
                    Map( 0xF002 + i, &Tf1201::Poke_F002 );
                    Map( 0xF003 + i, &Tf1201::Poke_F001 );
                }
                
                for (uint i=0x0000; i < 0x3004; i += 0x4)
                {
                    Map( 0xB000 + i, 0xB001 + i, &Tf1201::Poke_B000 );
                    Map( 0xB002 + i, 0xB003 + i, &Tf1201::Poke_B002 );
                }
            }
            
            void Tf1201::SubLoad(State::Loader& state)
            {
                while (const dword chunk = state.Begin())
                {
                    switch (chunk)
                    {
                        case AsciiId<'R','E','G'>::V:
                            
                            prgSelect = state.Read8();
                            break;
                            
                        case AsciiId<'I','R','Q'>::V:
                        {
                            State::Loader::Data<2> data( state );
                            
                            irq.unit.enabled = data[0] & 0x1;
                            irq.unit.count = data[2];
                            break;
                        }
                    }
                    
                    state.End();
                }
            }
            
            void Tf1201::SubSave(State::Saver& state) const
            {
                state.Begin( AsciiId<'R','E','G'>::V ).Write8( prgSelect ).End();
                
                const byte data[2] =
                {
                    irq.unit.enabled ? 0x1 : 0x0,
                    irq.unit.count & 0xFF
                };
                
                state.Begin( AsciiId<'I','R','Q'>::V ).Write( data ).End();
            }
            
            
            
            
            
            void Tf1201::UpdatePrg(uint bank)
            {
                prg.SwapBank<SIZE_8K,0x0000>( (prgSelect & 0x2) ? ~1U : bank );
                prg.SwapBank<SIZE_8K,0x4000>( (prgSelect & 0x2) ? bank : ~1U );
            }
            
            void Tf1201::Poke_8000(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_8000(i_,j_); } inline void Tf1201::Poke_M_8000(Address,Data data)
            {
                UpdatePrg( data );
            }
            
            void Tf1201::Poke_9001(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_9001(i_,j_); } inline void Tf1201::Poke_M_9001(Address,Data data)
            {
                prgSelect = data;
                UpdatePrg( prg.GetBank<SIZE_8K,0x0000>() );
            }
            
            void Tf1201::Poke_B000(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_B000(i_,j_); } inline void Tf1201::Poke_M_B000(Address address,Data data)
            {
                ppu.Update();
                address = (((address >> 11) - 6) | (address & 0x1)) << 10 & 0x1FFF;
                chr.SwapBank<SIZE_1K>( address, (chr.GetBank<SIZE_1K>(address) & 0xF0) | (data << 0 & 0x0F) );
            }
            
            void Tf1201::Poke_B002(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_B002(i_,j_); } inline void Tf1201::Poke_M_B002(Address address,Data data)
            {
                ppu.Update();
                address = (((address >> 11) - 6) | (address & 0x1)) << 10 & 0x1FFF;
                chr.SwapBank<SIZE_1K>( address, (chr.GetBank<SIZE_1K>(address) & 0x0F) | (data << 4 & 0xF0) );
            }
            
            void Tf1201::Poke_F000(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_F000(i_,j_); } inline void Tf1201::Poke_M_F000(Address,Data data)
            {
                irq.Update();
                irq.unit.count = (irq.unit.count & 0xF0) | (data << 0 & 0x0F);
            }
            
            void Tf1201::Poke_F002(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_F002(i_,j_); } inline void Tf1201::Poke_M_F002(Address,Data data)
            {
                irq.Update();
                irq.unit.count = (irq.unit.count & 0x0F) | (data << 4 & 0xF0);
            }
            
            void Tf1201::Poke_F001(void* p_,Address i_,Data j_) { static_cast<Tf1201*>(p_)->Poke_M_F001(i_,j_); } inline void Tf1201::Poke_M_F001(Address,Data data)
            {
                irq.Update();
                irq.unit.enabled = data & 0x2;
                irq.ClearIRQ();
                
                ppu.Update();
                
                if (ppu.GetScanline() < 240)
                    irq.unit.count -= 8;
            }
            
            bool Tf1201::Irq::Clock()
            {
                return enabled && (++count & 0xFF) == 238;
            }
            
            void Tf1201::Sync(Event event,Input::Controllers*)
            {
                if (event == EVENT_END_FRAME)
                    irq.VSync();
            }
        }
    }
}
