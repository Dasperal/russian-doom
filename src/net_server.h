//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2023 Julian Nechaevsky
// Copyright(C) 2020-2025 Leonid Murin (Dasperal)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Network server code
//


#pragma once


// initialize server and wait for connections

void NET_SV_Init(void);

// run server: check for new packets received etc.

void NET_SV_Run(void);

// Shut down the server
// Blocks until all clients disconnect, or until a 5 second timeout

void NET_SV_Shutdown(void);

// Add a network module to the context used by the server

void NET_SV_AddModule(net_module_t *module);

// Register server with master server.

void NET_SV_RegisterWithMaster(void);
