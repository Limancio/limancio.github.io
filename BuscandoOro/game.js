/*
game.js for Perlenspiel 3.3.x
Last revision: 2022-03-15 (BM)

Perlenspiel is a scheme by Professor Moriarty (bmoriarty@wpi.edu).
This version of Perlenspiel (3.3.x) is hosted at <https://ps3.perlenspiel.net>
Perlenspiel is Copyright © 2009-22 Brian Moriarty.
This file is part of the standard Perlenspiel 3.3.x devkit distribution.

Perlenspiel is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Perlenspiel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You may have received a copy of the GNU Lesser General Public License
along with the Perlenspiel devkit. If not, see <http://www.gnu.org/licenses/>.
*/

/*
This JavaScript file is a template for creating new Perlenspiel 3.3.x games.
Any unused event-handling function templates can be safely deleted.
Refer to the tutorials and documentation at <https://ps3.perlenspiel.net> for details.
*/

/*
The following comment lines are for JSHint <https://jshint.com>, a tool for monitoring code quality.
You may find them useful if your development environment is configured to support JSHint.
If you don't use JSHint (or are using it with a configuration file), you can safely delete these two lines.
*/

/* jshint browser : true, devel : true, esversion : 6, freeze : true */
/* globals PS : true */

"use strict"; // Do NOT remove this directive!

/*
PS.init( system, options )
Called once after engine is initialized but before event-polling begins.
This function doesn't have to do anything, although initializing the grid dimensions with PS.gridSize() is recommended.
If PS.grid() is not called, the default grid dimensions (8 x 8 beads) are applied.
Any value returned is ignored.
[system : Object] = A JavaScript object containing engine and host platform information properties; see API documentation for details.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

var G = {
	point_start: null,
	point_end: null,
	shadow_status: false,
	load_level: null,
	next_level: null,
	current_level: 0,
	levels: null,
	jewel_colors: [
		PS.COLOR_RED,
		PS.COLOR_ORANGE,
		PS.COLOR_GREEN,
		PS.COLOR_BLUE,
		PS.COLOR_INDIGO,
		PS.COLOR_VIOLET,
		PS.COLOR_CYAN,
		PS.COLOR_MAGENTA,
	]
};

PS.init = function( system, options ) {
	// Uncomment the following code line
	// to verify operation:

	// PS.debug( "PS.init() called\n" );

	// This function should normally begin
	// with a call to PS.gridSize( x, y )
	// where x and y are the desired initial
	// dimensions of the grid.
	// Call PS.gridSize() FIRST to avoid problems!
	// The sample call below sets the grid to the
	// default dimensions (8 x 8).
	// Uncomment the following code line and change
	// the x and y parameters as needed.

	// PS.gridSize( 9, 9 );
	
	// This is also a good place to display
	// your game title or a welcome message
	// in the status line above the grid.
	// Uncomment the following code line and
	// change the string parameter as needed.
	
	PS.statusText( "Game" );

	// Add any other initialization code you need here.
	
	G.levels = [
		{ 
			title: "Mi primera joya", width: 7, height: 3, map: 
			[
				0, 0, 0, 0, 0, 0, 0,
				0, 0, 1, 1, 2, 0, 0, 
				0, 0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "El camino del Inca", width: 9, height: 3, map: 
			[
				0, 0, 0, 0, 0, 0, 0, 0, 0, 
				0, 1, 1, 2, 1, 2, 1, 2, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "De las piedras en el camino", width: 8, height: 3, map: 
			[
				0, 0, 0, 0, 0, 0, 0, 0, 
				0, 1, 1, 1, 1, 2, 1, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "De la caida al vuelo", width: 7, height: 5, map: 
			[
				0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 2, 0, 0, 0, 
				0, 1, 1, 1, 1, 1, 0, 
				0, 0, 0, 1, 0, 0, 0, 
				0, 0, 0, 0, 0, 0, 0,
			]
		},
		{ 
			title: "Repitiendo mis coplas", width: 6, height: 6, map: 
			[
			0, 0, 0, 0, 0, 0, 
			0, 0, 0, 1, 0, 0, 
			0, 1, 1, 2, 0, 0, 
			0, 0, 2, 2, 1, 0, 
			0, 0, 1, 0, 0, 0, 
			0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "Juego de niños", width: 6, height: 6, map: 
			[
				0, 0, 0, 0, 0, 0, 
				0, 1, 1, 2, 1, 0, 
				0, 2, 2, 1, 1, 0, 
				0, 1, 1, 2, 1, 0, 
				0, 1, 1, 2, 1, 0, 
				0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "Receptor de reconocimiento de patrones", width: 6, height: 7, map: 
			[
				0, 0, 0, 0, 0, 0, 
				0, 1, 1, 1, 1, 0, 
				0, 1, 1, 1, 1, 0, 
				0, 2, 2, 2, 2, 0, 
				0, 1, 0, 1, 2, 0, 
				0, 2, 1, 2, 2, 0, 
				0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "Instrucciones sobre Escaleras I", width: 7, height: 7, map: 
			[
				0, 0, 0, 0, 0, 0, 0, 
				0, 1, 1, 2, 1, 2, 0, 
				0, 2, 2, 1, 1, 1, 0, 
				0, 2, 1, 2, 2, 1, 0, 
				0, 2, 2, 1, 2, 2, 0, 
				0, 2, 2, 2, 1, 2, 0, 
				0, 0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "Instrucciones sobre Escaleras II", width: 7, height: 7, map: 
			[
				0, 0, 0, 0, 0, 0, 0, 
				0, 2, 2, 1, 1, 2, 0, 
				0, 1, 2, 1, 2, 2, 0, 
				0, 2, 2, 2, 1, 2, 0, 
				0, 1, 1, 2, 1, 2, 0, 
				0, 1, 2, 2, 2, 1, 0, 
				0, 0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "Entrepiso", width: 7, height: 5, map: 
			[
				0, 0, 0, 0, 0, 0, 0, 
				0, 2, 2, 2, 0, 0, 0, 
				0, 1, 1, 1, 1, 1, 0, 
				0, 1, 1, 1, 1, 1, 0, 
				0, 0, 0, 0, 0, 0, 0, 
			]
		},
		{ 
			title: "Gestalt", width: 7, height: 8, map: 
			[
				2, 1, 2, 2, 2, 2, 2, 
				2, 1, 1, 1, 2, 2, 2, 
				1, 2, 2, 2, 2, 2, 2, 
				2, 1, 2, 1, 1, 1, 2, 
				2, 1, 2, 2, 1, 2, 2, 
				2, 2, 2, 2, 2, 2, 1, 
				1, 1, 2, 1, 2, 1, 2, 
				1, 1, 2, 1, 1, 2, 2, 
			]
		},
		{ 
			title: "Taijitu", width: 8, height: 8, map: 
			[
				0, 0, 0, 0, 0, 0, 0, 0, 
				0, 2, 2, 2, 1, 1, 1, 0, 
				0, 2, 2, 2, 1, 2, 1, 0, 
				0, 2, 2, 2, 1, 1, 1, 0, 
				0, 2, 2, 2, 1, 1, 1, 0, 
				0, 2, 0, 2, 1, 1, 1, 0, 
				0, 2, 2, 2, 1, 1, 1, 0, 
				0, 0, 0, 0, 0, 0, 0, 0, 
			]
		},
	]
	// G.current_level = G.levels.length-1;

	G.load_level = function () {
		G.point_start = null;
		G.point_end = null;

		var level = G.levels[G.current_level];

		PS.statusText( level.title );
		PS.gridSize( level.width, level.height );
		PS.gridColor( 0x303030 ); 
		PS.statusColor( PS.COLOR_WHITE );

		var x, y, ptr, data, val;
		ptr = 0; 

		for ( y = 0; y < level.height; y += 1 ) {
			for ( x = 0; x < level.width; x += 1 ) {
				data = level.map [ ptr ]; 
				if ( data === 0 ) { 
					val = ( PS.random(32) - 1 ) + 128; // @NOTE: Random color.
					PS.color( x, y, val, val, val );
				}
				else if ( data === 1 ) { 
					PS.data(x, y, data);
					PS.color( x, y, G.jewel_colors[PS.random(G.jewel_colors.length-1)] );
				}
				else if ( data === 2 ) { 
					PS.data(x, y, data);
					PS.color( x, y, PS.COLOR_WHITE );
				}

				ptr += 1; 
			}
		}
	}
	
	G.next_level = function () {
		var level = G.levels[G.current_level];

		var x, y, ptr, data, gold_count;
		gold_count = 0;

		for ( y = 0; y < level.height; y += 1 ) {
			for ( x = 0; x < level.width; x += 1 ) {
				data = PS.data(x, y); 
				if ( data != 0 && data === 1 ) { 
					gold_count += 1;
				}

			}
		}

		if(gold_count == 1) {
			G.current_level += 1;
			PS.audioPlay( "perc_triangle" );
			PS.gridShadow( true, G.jewel_colors[PS.random(G.jewel_colors.length-1)] );
			G.load_level();
		} else {
			PS.audioPlay( "fx_tick" );
		}

	}

	G.load_level();
};

/*
PS.touch ( x, y, data, options )
Called when the left mouse button is clicked over bead(x, y), or when bead(x, y) is touched.
This function doesn't have to do anything. Any value returned is ignored.
[x : Number] = zero-based x-position (column) of the bead on the grid.
[y : Number] = zero-based y-position (row) of the bead on the grid.
[data : *] = The JavaScript value previously associated with bead(x, y) using PS.data(); default = 0.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.touch = function( x, y, data, options ) {
	// Uncomment the following code line
	// to inspect x/y parameters:

	// PS.debug( "PS.touch() @ " + x + ", " + y + "\n" );

	// Add code here for mouse clicks/touches
	// over a bead.

	if(G.shadow_status) {
		G.shadow_status = false;
		PS.gridShadow( false );
	}

	if(G.point_start != null && G.point_end != null) {
		PS.border( G.point_start.x, G.point_start.y, PS.DEFAULT );
		PS.border( G.point_end.x, G.point_end.y, PS.DEFAULT );
		PS.borderColor( G.point_start.x, G.point_start.y, PS.DEFAULT );
		PS.borderColor( G.point_end.x, G.point_end.y, PS.DEFAULT );
		
		G.point_start = null;
		G.point_end = null;
	}

	if(G.point_start != null && G.point_start.x == x && G.point_start.y == y) {
		G.point_start = null;
		PS.borderColor( x, y, PS.DEFAULT );
		PS.border( x, y, PS.DEFAULT );
		return;
	}

 	if ( data === 1 ) {
		if (G.point_start == null) {
			PS.audioPlay( "fx_click" );
			PS.borderColor( x, y, PS.COLOR_YELLOW );
			PS.border( x, y, 4 );
			G.point_start = {x: x, y: y};
		} 
	} else if(data === 2 && G.point_start != null) {
		if (G.selection_b == null) {
			PS.audioPlay( "fx_click" );
			PS.borderColor( x, y, PS.COLOR_YELLOW );
			PS.border( x, y, 4 );
			G.point_end = {x: x, y: y};
		}
	}

};

/*
PS.release ( x, y, data, options )
Called when the left mouse button is released, or when a touch is lifted, over bead(x, y).
This function doesn't have to do anything. Any value returned is ignored.
[x : Number] = zero-based x-position (column) of the bead on the grid.
[y : Number] = zero-based y-position (row) of the bead on the grid.
[data : *] = The JavaScript value previously associated with bead(x, y) using PS.data(); default = 0.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.release = function( x, y, data, options ) {
	// Uncomment the following code line to inspect x/y parameters:

	// G.selection_active = false;
 	// PS.border( x, y, PS.DEFAULT );
	// PS.debug( "PS.release() @ " + x + ", " + y + "\n" );

	// Add code here for when the mouse button/touch is released over a bead.

	if(G.point_start && G.point_end) {
		var take_gold = false;
		var start, mid, end;
		var _x = -1, _y = -1;

		var x_distance = Math.abs(G.point_start.x - G.point_end.x);
		var y_distance = Math.abs(G.point_start.y - G.point_end.y);

		if(G.point_start.x == G.point_end.x && y_distance == 2) {
			if(G.point_start.y < G.point_end.y) {
				_x = G.point_start.x;
				_y = G.point_start.y + 1;

			} else {
				_x = G.point_start.x;
				_y = G.point_start.y - 1;
			}
		}
		
		if(G.point_start.y == G.point_end.y && x_distance == 2) {
			if(G.point_start.x < G.point_end.x) {
				_x = G.point_start.x + 1;
				_y = G.point_start.y;
			} else {
				_x = G.point_start.x - 1;
				_y = G.point_start.y;
			}
		}

		if(_x != -1 && _y != -1) {
			mid = {x: _x, y: _y};

			var mid_bead_data = PS.data(mid.x, mid.y);
			var end_bead_data = PS.data(G.point_end.x, G.point_end.y);
			if(mid_bead_data == 1 && end_bead_data == 2) {
				take_gold = true;
				start = G.point_start;
				end   = G.point_end;
			}
		}

		if(take_gold) {
			G.shadow_status = true;
			
			PS.color( start.x, start.y, PS.DEFAULT );
			PS.color( mid.x, mid.y, PS.DEFAULT );
			PS.color( end.x, end.y, G.jewel_colors[PS.random(G.jewel_colors.length-1)] );

			PS.data( start.x, start.y, 2 );
			PS.data( mid.x, mid.y, 2 );
			PS.data( end.x, end.y, 1 );

			PS.borderColor( start.x, start.y, 0, 255, 0 );
			PS.borderColor( end.x, end.y, 0, 255, 0 );

			//
			G.next_level();

		} else {
			PS.audioPlay( "perc_conga_low" );
			PS.borderColor( G.point_start.x, G.point_start.y, 255, 0, 0 );
			PS.borderColor( G.point_end.x, G.point_end.y, 255, 0, 0 );
		}
	}

};

/*
PS.enter ( x, y, button, data, options )
Called when the mouse cursor/touch enters bead(x, y).
This function doesn't have to do anything. Any value returned is ignored.
[x : Number] = zero-based x-position (column) of the bead on the grid.
[y : Number] = zero-based y-position (row) of the bead on the grid.
[data : *] = The JavaScript value previously associated with bead(x, y) using PS.data(); default = 0.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.enter = function( x, y, data, options ) {
	// Uncomment the following code line to inspect x/y parameters:

	// PS.debug( "PS.enter() @ " + x + ", " + y + "\n" );
 	
	// Add code here for when the mouse cursor/touch enters a bead.
};

/*
PS.exit ( x, y, data, options )
Called when the mouse cursor/touch exits bead(x, y).
This function doesn't have to do anything. Any value returned is ignored.
[x : Number] = zero-based x-position (column) of the bead on the grid.
[y : Number] = zero-based y-position (row) of the bead on the grid.
[data : *] = The JavaScript value previously associated with bead(x, y) using PS.data(); default = 0.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.exit = function( x, y, data, options ) {
	// Uncomment the following code line to inspect x/y parameters:

	// PS.debug( "PS.exit() @ " + x + ", " + y + "\n" );

	// Add code here for when the mouse cursor/touch exits a bead.
};

/*
PS.exitGrid ( options )
Called when the mouse cursor/touch exits the grid perimeter.
This function doesn't have to do anything. Any value returned is ignored.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.exitGrid = function( options ) {
	// Uncomment the following code line to verify operation:

	// PS.debug( "PS.exitGrid() called\n" );

	// Add code here for when the mouse cursor/touch moves off the grid.
};

/*
PS.keyDown ( key, shift, ctrl, options )
Called when a key on the keyboard is pressed.
This function doesn't have to do anything. Any value returned is ignored.
[key : Number] = ASCII code of the released key, or one of the PS.KEY_* constants documented in the API.
[shift : Boolean] = true if shift key is held down, else false.
[ctrl : Boolean] = true if control key is held down, else false.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.keyDown = function( key, shift, ctrl, options ) {
	// Uncomment the following code line to inspect first three parameters:

	// PS.debug( "PS.keyDown(): key=" + key + ", shift=" + shift + ", ctrl=" + ctrl + "\n" );
	
	// Add code here for when a key is pressed.
	
	if(key == PS.KEY_SPACE) {
		G.load_level();
	}
};

/*
PS.keyUp ( key, shift, ctrl, options )
Called when a key on the keyboard is released.
This function doesn't have to do anything. Any value returned is ignored.
[key : Number] = ASCII code of the released key, or one of the PS.KEY_* constants documented in the API.
[shift : Boolean] = true if shift key is held down, else false.
[ctrl : Boolean] = true if control key is held down, else false.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
*/

PS.keyUp = function( key, shift, ctrl, options ) {
	// Uncomment the following code line to inspect first three parameters:

	// PS.debug( "PS.keyUp(): key=" + key + ", shift=" + shift + ", ctrl=" + ctrl + "\n" );

	// Add code here for when a key is released.
};

/*
PS.input ( sensors, options )
Called when a supported input device event (other than those above) is detected.
This function doesn't have to do anything. Any value returned is ignored.
[sensors : Object] = A JavaScript object with properties indicating sensor status; see API documentation for details.
[options : Object] = A JavaScript object with optional data properties; see API documentation for details.
NOTE: Currently, only mouse wheel events are reported, and only when the mouse cursor is positioned directly over the grid.
*/

PS.input = function( sensors, options ) {
	// Uncomment the following code lines to inspect first parameter:

//	 var device = sensors.wheel; // check for scroll wheel
//
//	 if ( device ) {
//	   PS.debug( "PS.input(): " + device + "\n" );
//	 }

	// Add code here for when an input event is detected.
};

