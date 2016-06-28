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

#include "NstInpDevice.hpp"
#include "NstInpAdapter.hpp"

namespace Nes
{
    namespace Core
    {
        namespace Input
        {
            
            
            
            
            Adapter::Adapter(Type t)
            : type(t) {}
            
            AdapterTwo::AdapterTwo(Device& a,Device& b,Type t)
            : Adapter(t)
            {
                devices[0] = &a;
                devices[1] = &b;
            }
            
            bool AdapterTwo::SetType(Type t)
            {
                if (type != t)
                {
                    type = t;
                    return true;
                }
                
                return false;
            }
            
            Device& AdapterTwo::Connect(uint port,Device& device)
            {
                (__builtin_expect(!(!!(port < 2)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/input/NstInpAdapter.cpp", 61, "!!(port < 2)") : (void)0);
                
                Device& old = *devices[port];
                devices[port] = &device;
                
                return old;
            }
            
            void AdapterTwo::Initialize(bool arcade)
            {
                for (uint i=0; i < 2; ++i)
                    devices[i]->Initialize( arcade );
            }
            
            void AdapterTwo::Reset()
            {
                for (uint i=0; i < 2; ++i)
                    devices[i]->Reset();
            }
            
            
            
            
            
            uint AdapterTwo::NumPorts() const
            {
                return 2;
            }
            
            Device& AdapterTwo::GetDevice(uint port) const
            {
                (__builtin_expect(!(!!(port < 2)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/input/NstInpAdapter.cpp", 92, "!!(port < 2)") : (void)0);
                return *devices[port];
            }
            
            void AdapterTwo::BeginFrame(Controllers* input)
            {
                devices[0]->BeginFrame( input );
                devices[1]->BeginFrame( input );
            }
            
            void AdapterTwo::Poke(uint data)
            {
                devices[0]->Poke( data );
                devices[1]->Poke( data );
            }
            
            uint AdapterTwo::Peek(uint line)
            {
                (__builtin_expect(!(!!(line < 2)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/input/NstInpAdapter.cpp", 110, "!!(line < 2)") : (void)0);
                return devices[line]->Peek( line );
            }
            
            
            
            
            
            AdapterFour::AdapterFour(Device& a,Device& b,Device& c,Device& d,Type t)
            : Adapter(t), increaser(1)
            {
                count[0] = 0;
                count[1] = 0;
                
                devices[0] = &a;
                devices[1] = &b;
                devices[2] = &c;
                devices[3] = &d;
            }
            
            Device& AdapterFour::Connect(uint port,Device& device)
            {
                (__builtin_expect(!(!!(port < 4)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/input/NstInpAdapter.cpp", 132, "!!(port < 4)") : (void)0);
                
                Device& old = *devices[port];
                devices[port] = &device;
                
                return old;
            }
            
            void AdapterFour::Initialize(bool arcade)
            {
                for (uint i=0; i < 4; ++i)
                    devices[i]->Initialize( arcade );
            }
            
            void AdapterFour::Reset()
            {
                increaser = 1;
                
                count[0] = 0;
                count[1] = 0;
                
                for (uint i=0; i < 4; ++i)
                    devices[i]->Reset();
            }
            
            bool AdapterFour::SetType(Type t)
            {
                if (type != t)
                {
                    type = t;
                    increaser = 1;
                    count[0] = 0;
                    count[1] = 0;
                    
                    return true;
                }
                
                return false;
            }
            
            void AdapterFour::SaveState(State::Saver& state,const dword chunk) const
            {
                if (type == Api::Input::ADAPTER_NES)
                {
                    const byte data[3] =
                    {
                        increaser ^ 1, count[0], count[1]
                    };
                    
                    state.Begin( chunk ).Write( data ).End();
                }
            }
            
            void AdapterFour::LoadState(State::Loader& state)
            {
                if (type == Api::Input::ADAPTER_NES)
                {
                    State::Loader::Data<3> data( state );
                    
                    increaser = ~data[0] & 0x1;
                    count[0] = (data[1] <= 20) ? data[1] : 0;
                    count[1] = (data[2] <= 20) ? data[2] : 0;
                }
            }
            
            
            
            
            
            uint AdapterFour::NumPorts() const
            {
                return 4;
            }
            
            Device& AdapterFour::GetDevice(uint port) const
            {
                (__builtin_expect(!(!!(port < 4)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/input/NstInpAdapter.cpp", 208, "!!(port < 4)") : (void)0);
                return *devices[port];
            }
            
            void AdapterFour::BeginFrame(Controllers* input)
            {
                for (uint i=0; i < 4; ++i)
                    devices[i]->BeginFrame( input );
            }
            
            void AdapterFour::Poke(const uint data)
            {
                if (type == Api::Input::ADAPTER_NES)
                {
                    increaser = ~data & 0x1;
                    
                    if (!increaser)
                    {
                        count[0] = 0;
                        count[1] = 0;
                    }
                }
                
                for (uint i=0; i < 4; ++i)
                    devices[i]->Poke( data );
            }
            
            uint AdapterFour::Peek(const uint line)
            {
                (__builtin_expect(!(!!(line < 2)), 0) ? __assert_rtn(__func__, "/Users/Jorrit/iOS/nestopia/core/input/NstInpAdapter.cpp", 237, "!!(line < 2)") : (void)0);
                
                if (type == Api::Input::ADAPTER_NES)
                {
                    const uint index = count[line];
                    
                    if (index < 20)
                    {
                        count[line] += increaser;
                        
                        if (index < 16)
                        {
                            return devices[line + (index < 8 ? 0 : 2)]->Peek( line );
                        }
                        else if (index >= 18)
                        {
                            return (index - 18) ^ line;
                        }
                    }
                    
                    return 0;
                }
                else
                {
                    return
                    (
                     (devices[line+0]->Peek( line ) << 0 & 0x1) |
                     (devices[line+2]->Peek( line ) << 1 & 0x2)
                     );
                }
            }
        }
    }
}
