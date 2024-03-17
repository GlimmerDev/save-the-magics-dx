#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_FontCache.h>
#include "include/enum.h"
#include "include/util.h"
#include "include/object.h"
#include "include/draw.h"

// Fonts
FC_Font** FONTS;
const char* FONT_FILENAMES[NUM_FONTS] = {
	"RPGSystem.ttf"
};

extern const int AUTOSAVE_INTERVALS[];

SDL_Color COL[]  = {
	(SDL_Color){0, 0, 0, 255}, 		 // BLACK
	(SDL_Color){255, 255, 255, 255}, // WHITE
	(SDL_Color){255, 0, 0, 255},	 // RED
	(SDL_Color){0, 255, 0, 255}, 	 // GREEN
	(SDL_Color){240, 240, 240, 255}, // OFFWHITE
	(SDL_Color){128, 128, 128, 255}, // LOCKED
	(SDL_Color){10, 10, 20, 255},	 // BACKGROUND
	(SDL_Color){14, 14, 24, 255},	 // PLANET1
	(SDL_Color){27, 26, 38, 255},    // PLANET2
	(SDL_Color){32, 31, 42, 255},    // PLANET3
	(SDL_Color){237, 227, 255, 100}, // EXPL1
	(SDL_Color){224, 255, 255, 40},  // EXPL2
	(SDL_Color){152, 251, 152, 40},  // EXPL3
	(SDL_Color){255, 228, 181, 100}, // EXPL4
	(SDL_Color){32, 32, 41, 255},    // SHIP_COL
	(SDL_Color){129, 19, 49, 255},	 // SHIP_GLOW
	(SDL_Color){215, 161, 249, 255}, // B_PURPLE (idle)
	(SDL_Color){160, 32, 240, 255},  // B_PURPLE (selected)
	(SDL_Color){237, 227, 255, 255}, // B_LIGHTPURPLE (idle)
	(SDL_Color){175, 159, 201, 255}, // B_LIGHTPURPLE (selected)
	(SDL_Color){144, 99, 205, 255},  // B_DARKPURPLE (idle)
	(SDL_Color){53, 37, 77, 255},	 // B_DARKPURPLE (selected)
	(SDL_Color){72, 209, 204, 255},	 // B_BLUE (idle)
	(SDL_Color){0, 139, 139, 255},   // B_BLUE (selected)
	(SDL_Color){224, 255, 255, 255}, // B_LIGHTBLUE (idle)
	(SDL_Color){95, 158, 160, 255},	 // B_LIGHTBLUE (selected)
	(SDL_Color){152, 251, 152, 255}, // B_GREEN (idle)
	(SDL_Color){60, 179, 113, 255},	 // B_GREEN (selected)
	(SDL_Color){255, 99, 71, 255},	 // B_RED (idle)
	(SDL_Color){178, 34, 34, 255},	 // B_RED (selected)
	(SDL_Color){253, 183, 80, 255},	 // B_ORANGE (idle)
	(SDL_Color){253, 127, 32, 255},	 // B_ORANGE (selected)
	(SDL_Color){173, 26, 66, 255},   // B_DARK (idle)
	(SDL_Color){129, 19, 49, 255},	 // B_DARK (selected)
    (SDL_Color){50, 173, 97, 255},   // B_DARKGREEN (idle)
	(SDL_Color){40, 112, 65, 255},   // B_DARKGREEN (selected)
	(SDL_Color){255, 120, 174, 255}, // B_PINK (idle)
	(SDL_Color){255, 0, 138, 255},   // B_PINK (selected)
    (SDL_Color){255, 228, 181, 255}, // B_MENU (idle)
	(SDL_Color){255, 181, 54, 255},	 // B_MENU (selected)
	(SDL_Color){255, 181, 54, 255},	 // B_SAVE_CLEAR (idle)
	(SDL_Color){255, 228, 181, 255}, // B_SAVE_CLEAR (selected)
	(SDL_Color){35, 38, 47, 255},	 // STAR1
	(SDL_Color){38, 38, 47, 255},    // STAR2
	(SDL_Color){38, 30, 27, 255},    // STAR3
	(SDL_Color){38, 35, 38, 255},    // STAR4
	(SDL_Color){255, 255, 255, 90 }, // TRANS_WHITE
	(SDL_Color){0, 0, 0, SDL_ALPHA_TRANSPARENT} // TRANSPARENT
};

Shape BG_SHAPES[13];

Star BG_STARS[NUM_STARS];

Shape END_SHIP_SHAPES[6];

// *** PRIMITIVE SHAPES ***

int SDL_RenderDrawCircle(SDL_Renderer * renderer, const int x, const int y, const int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

void SDL_RenderFillRing(SDL_Renderer *renderer, int cx, int cy, int outerRadius, int innerRadius) {
    const int precision = 720; // Increase for smoother circles
    double step = M_PI * 2 / precision; // Calculate step for each segment

    for (double angle = 0; angle < M_PI * 2; angle += step) {
        // Calculate outer edge points
        int outerX = cx + cos(angle) * outerRadius;
        int outerY = cy + sin(angle) * outerRadius;

        // Calculate inner edge points
        int innerX = cx + cos(angle) * innerRadius;
        int innerY = cy + sin(angle) * innerRadius;

        // Draw a line from the inner edge to the outer edge
        SDL_RenderDrawLine(renderer, innerX, innerY, outerX, outerY);
    }
}

int SDL_RenderFillCircle(SDL_Renderer * renderer, const int x, const int y, const int radius)
{
    int offsetx, offsety, d;
    int status;
	
    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

void SDL_RenderFillTriangle(SDL_Renderer* renderer, const E_ColorIndex color, int x1, int y1, int x2, int y2, int x3, int y3) {
    SDL_Vertex vertices[3] = {
        { .position = {x1, y1}, .color = COL[color] },
        { .position = {x2, y2}, .color = COL[color] }, 
        { .position = {x3, y3}, .color = COL[color] }  
    };
    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

// *** DRAW-RELATED INIT FUNCTIONS ***

int init_fonts(SDL_Renderer* renderer) {
	FC_Font** fonts = (FC_Font**)malloc(sizeof(FC_Font*) * NUM_PER_FONT * NUM_FONTS);
	if (!fonts) return -1;

	char path_buf[MAX_PATH_LEN];

	for (int i = 0; i < NUM_FONTS; ++i) {
		#ifdef __ANDROID__
		android_load_asset_file(FONT_FILENAMES[i]);
		snprintf(path_buf, MAX_PATH_LEN, "%s", FONT_FILENAMES[i]);
		#else
		snprintf(path_buf, MAX_PATH_LEN, "res/%s", FONT_FILENAMES[i]);
		#endif
		for (int j = 0; j < NUM_PER_FONT; ++j) {
			const int offset = i*NUM_PER_FONT + j;
			fonts[offset] = FC_CreateFont();
			if (!fonts[offset]) {
				for (int k = 0; k < offset; ++k) {
					FC_FreeFont(fonts[k]);
				}
				free(fonts);
				return -1;
			}
			FC_LoadFont(fonts[offset], renderer, path_buf, j*2+MIN_FONT_SIZE, COL[WHITE], TTF_STYLE_NORMAL);
		}
	}
	FONTS = fonts;
	return 0;
}

void magics_font_cleanup() {
	for (int i = 0; i < NUM_FONTS*NUM_PER_FONT; ++i) {
		FC_FreeFont(FONTS[i]);
	}
	free(FONTS);
}

void bg_vertex_cleanup() {
	for (int i = 0; i < sizeof(BG_SHAPES) / sizeof(Shape); ++i) {
		if (BG_SHAPES[i].v) {
			free(BG_SHAPES[i].v);
		}
	}
}

int init_tree_shapes() {
	for (int i = 0; i < sizeof(BG_SHAPES) / sizeof(Shape); ++i) {
		Shape* tree = &BG_SHAPES[i];
		if (tree->type != SHAPE_TYPE_TREE) {
			continue;
		}
		
		tree->v = (int*)safe_calloc(18, sizeof(int));
		for (int j = 0; j < 3; ++j) {
			tree->v[j*6]   = tree->x;
			tree->v[j*6+1] = tree->y - 100 - j*60;
			tree->v[j*6+2] = tree->x - 50;
			tree->v[j*6+3] = tree->y - j*60;
			tree->v[j*6+4] = tree->x + 50;
			tree->v[j*6+5] = tree->y - j*60;
		}
	}
	return 0;
}

int init_hill_shapes() {
	for (int i = 0; i < sizeof(BG_SHAPES) / sizeof(Shape); ++i) {
		Shape* hill = &BG_SHAPES[i];
		if (hill->type != SHAPE_TYPE_HILL) {
			continue;
		}
		
		hill->v = (int*)safe_calloc(6, sizeof(int));
		for (int j = 0; j < 3; ++j) {
			hill->v[1] = screen_height();
			hill->v[2] = screen_width();
			hill->v[3] = screen_height() - hill->w;
			hill->v[4] = screen_width();
			hill->v[5] = screen_height();
		}
	}
	return 0;
}

void init_starfield() {
    for (int i = 0; i < NUM_STARS; ++i) {
        BG_STARS[i].x = rand() % screen_width();
        BG_STARS[i].y = rand() % screen_height();
        BG_STARS[i].speed = 1 + (rand() % 4); // Random speed for parallax effect
		BG_STARS[i].size = 2 + (rand() % 4);
		BG_STARS[i].color = STAR1 + (rand() % 3);
    }
}

void init_bg_shapes() {
	BG_SHAPES[0] =  (Shape){SHAPE_TYPE_CIRCLE, PLANET1, screen_center_x(), screen_center_y(), 240, 0, NULL};		            // BG_PLANET1
	BG_SHAPES[1] = 	(Shape){SHAPE_TYPE_CIRCLE, PLANET2, screen_center_x(), screen_center_y(), 230, 0, NULL};		            // BG_PLANET2
	BG_SHAPES[2] = 	(Shape){SHAPE_TYPE_CIRCLE, PLANET3, screen_center_x(), screen_center_y(), 224, 0, NULL};		            // BG_PLANET3
	BG_SHAPES[3] = 	(Shape){SHAPE_TYPE_CIRCLE, PLANET3, screen_center_x()-200, screen_center_y()-150, 80, 0, NULL};             // BG_MEDI_SUN1
	BG_SHAPES[4] = 	(Shape){SHAPE_TYPE_CIRCLE, PLANET2, screen_center_x()-266, screen_center_y()-200, 40, 0, NULL};             // BG_MEDI_SUN2
	BG_SHAPES[5] = 	(Shape){SHAPE_TYPE_CIRCLE, PLANET1, screen_center_x()-134, screen_center_y()-280, 60, 0, NULL};             // BG_MEDI_SUN3
	BG_SHAPES[6] = 	(Shape){SHAPE_TYPE_TREE, PLANET2, screen_ratio()*525, screen_height()-screen_ratio()*120, 0, 0, NULL};      // BG_MEDI_TREE1
	BG_SHAPES[7] = 	(Shape){SHAPE_TYPE_TREE, PLANET2, screen_ratio()*75, screen_height()-screen_ratio()*8, 0, 0, NULL};         // BG_MEDI_TREE2
	BG_SHAPES[8] = 	(Shape){SHAPE_TYPE_TREE, PLANET1, screen_ratio()*300, screen_height()-screen_ratio()*60, 0, 0, NULL};	    // BG_MEDI_TREE3
	BG_SHAPES[9] = 	(Shape){SHAPE_TYPE_HILL, PLANET3, 0, 0, screen_ratio()*150, 0, NULL}; 						                // BG_MEDI_HILL1
	BG_SHAPES[10] = (Shape){SHAPE_TYPE_HILL, PLANET2, 0, 0, screen_ratio()*113, 0, NULL}; 						                // BG_MEDI_HILL2
	BG_SHAPES[11] = (Shape){SHAPE_TYPE_HILL, PLANET1, 0, 0, screen_ratio()*75, 0, NULL}; 							            // BG_MEDI_HILL3
	BG_SHAPES[12] = (Shape){SHAPE_TYPE_RECT, TRANS_WHITE, screen_center_x()-393, 316, 784, 233, NULL}; 			            	// BG_MENU_RECT
}

void init_end_ship_shapes() {
	END_SHIP_SHAPES[0] = (Shape){SHAPE_TYPE_RECT, SHIP_COL, screen_center_x()-150, screen_center_y()-40, 300, 80, NULL}; // END_SHIP
    END_SHIP_SHAPES[1] = (Shape){SHAPE_TYPE_RECT, SHIP_COL, screen_center_x()-250, screen_center_y()-10, 500, 20, NULL}; 
    END_SHIP_SHAPES[2] = (Shape){SHAPE_TYPE_RECT, SHIP_COL, screen_center_x()-50, screen_center_y()-50, 100, 100, NULL};
    END_SHIP_SHAPES[3] = (Shape){SHAPE_TYPE_RECT, SHIP_COL, screen_center_x()-10, screen_center_y()-130, 20, 200, NULL};
	END_SHIP_SHAPES[4] = (Shape){SHAPE_TYPE_RECT, SHIP_COL, screen_center_x()-30, screen_center_y()-5, 20, 10, NULL};    // END_SHIP_EYEL
	END_SHIP_SHAPES[5] = (Shape){SHAPE_TYPE_RECT, SHIP_COL, screen_center_x()+10, screen_center_y()-5, 20, 10, NULL};    // END_SHIP_EYER
}

void init_shapes(Config* const config) {
	// Create rects for upgrade buttons
	set_upgrade_rects(config->upgrades, NUM_UPGRADES, 0);
	set_upgrade_rects(config->upgrades, NUM_PRINCESSES, PRINCESS_OFFSET);
	set_upgrade_rects(config->upgrades, NUM_INCANTATIONS, INCANTATION_OFFSET);
	// init shape vertices
	init_bg_shapes();
	init_end_ship_shapes();
	init_tree_shapes();
	init_hill_shapes();
	// init stars
	init_starfield();	
}

// *** DRAW FUNCTIONS ***

void draw_starfield(SDL_Renderer* renderer) {
    for (int i = 0; i < NUM_STARS; ++i) {
        SDL_Rect starRect = {(int)BG_STARS[i].x, (int)BG_STARS[i].y, BG_STARS[i].size, BG_STARS[i].size};
        draw_rect(&starRect, BG_STARS[i].color, renderer);
    }
}

void update_starfield(GameState* state) {
    for (int i = 0; i < NUM_STARS; ++i) {
        BG_STARS[i].y += (BG_STARS[i].speed*3)/get_fps(); // Move star down (vertical scrolling)
        // Reset star to top of the screen if it moves off the bottom
        if (BG_STARS[i].y >= screen_height()) {
            BG_STARS[i].x = rand() % screen_width();
            BG_STARS[i].y = 0;
            BG_STARS[i].speed = 1 + (rand() % 4);
        }
    }
}

int clear_screen(SDL_Renderer* renderer, const E_ColorIndex color) {
	if (color >= NUM_COLORS) {
		LOG_E("Error: Cannot  clear screen, invalid color index %d\n", color);
		return -1;
	}
	set_draw_color(renderer, color);
	SDL_RenderClear(renderer);
	return 0;
}

void draw_shape(SDL_Renderer* renderer, Shape* shape) {
	int* v;
	switch(shape->type) {
		case SHAPE_TYPE_RECT:
			set_draw_color(renderer, shape->color);
			SDL_Rect rect = (SDL_Rect){shape->x, shape->y, shape->w, shape->h};
			SDL_RenderFillRect(renderer, &rect);
			break;
		case SHAPE_TYPE_CIRCLE:
			set_draw_color(renderer, shape->color);
			SDL_RenderFillCircle(renderer, shape->x, shape->y, shape->w);
			break;
		case SHAPE_TYPE_TREE:
			v = shape->v;
			for (int i = 0; i < 18; i += 6) {
				SDL_RenderFillTriangle(renderer, shape->color, v[i], v[i+1], 
				v[i+2], v[i+3], v[i+4], v[i+5]);
			}
			break;
		case SHAPE_TYPE_HILL:
		case SHAPE_TYPE_TRI:
			v = shape->v;
			SDL_RenderFillTriangle(renderer, shape->color, v[0], v[1], 
				v[2], v[3], v[4], v[5]);
			break;
	}
}

void draw_background_planet(SDL_Renderer* renderer) {
	for (int i = BG_PLANET1; i <= BG_PLANET3; ++i) {
		draw_shape(renderer, &BG_SHAPES[i]);
	}
}

void draw_background_meditate(SDL_Renderer* renderer) {	
	for (int i = BG_MEDI_SUN1; i <= BG_MEDI_HILL3; ++i) {
		draw_shape(renderer, &BG_SHAPES[i]);
	}
}

int draw_rect(const SDL_Rect* rect, const E_ColorIndex color, SDL_Renderer* renderer) {
	set_draw_color(renderer, color);
	return SDL_RenderFillRect(renderer, rect);
}

int _draw_button(Config* config, bool set_ptrs, bool by_ref, const E_ButtonIndex b_index, Button* b_ref) {
	static Button* buttons = NULL;
	static SDL_Renderer* renderer = NULL;
	if (set_ptrs) {
		buttons = config->buttons;
		renderer = config->renderer;
		return 0;
	} 
	else if (!buttons || !renderer) { return -1; }
	
	Button* button = NULL;
	if (by_ref) { button = b_ref; } 
	else { button = &(buttons[b_index]); }
	if (!button) { return -1; }
	
	if (button->locked) {
		set_draw_color(renderer, LOCKED);
	}
	else if (button->color+1 >= NUM_COLORS) {
		LOG_E("Error: invalid color index %d for draw_button", button->color);
		return -1;
	}
	else if (button->state == STATE_BTN_CLICKED) {
		set_draw_color(renderer, button->color+1);
	}
	else { 
		set_draw_color(renderer, button->color);
	}
	SDL_RenderFillRect(renderer, &(button->rect));
	
	if (!button->name) { return 0; }
	const char* text = "";
	
	if (button->locked) {
		if (!button->hide_locked_text) {
			text = "???";
		}
	}
	else text = button->name;

	draw_text(text, button->centerx, button->centery - button->text_offset, 
				FONT_RPG, button->text_size, button->text_color, renderer);
	
	return 0;
}

int draw_button(const E_ButtonIndex b) {
	return _draw_button(NULL, false, false, b, NULL);
}

int draw_button_by_ref(Button* bptr) {
	return _draw_button(NULL, false, true, -1, bptr);
}

int draw_button_set_ptrs(Config* cptr) {
	return _draw_button(cptr, true, false, -1, NULL);
}

void draw_upgrade_text(const Upgrade* upgrade, const GameState* state, SDL_Renderer* renderer) {
	static char strbuf[256];

	const Button* bptr = upgrade->button;
	unsigned int max_level = state->upgrade_max;

	if (upgrade->upgrade_type == UPGRADE_TYPE_INCANTATION) {
		if (upgrade->button->locked) {
			snprintf(strbuf, 256, "[%s]", frames_to_time_str(upgrade->cooldown, state));
		} else {
			snprintf(strbuf, 256, "[Ready!]");
		}
	} else {
		if (upgrade->upgrade_type == UPGRADE_TYPE_PRINCESS) {
			max_level = state->princess_max;
		}
		snprintf(strbuf, 256, "[%d/%d]", upgrade->count, max_level);
	}
	draw_text(strbuf, bptr->centerx, bptr->centery+38-9, FONT_RPG, 18, BLACK, renderer);

	draw_text(upgrade->name, bptr->centerx, bptr->centery-40-10, FONT_RPG, 20, BLACK, renderer);
	draw_text(upgrade->description, bptr->centerx, bptr->centery-20-7, FONT_RPG, 14, BLACK, renderer);

	// ignore cost & multiplier strings for incantations
	if (upgrade->upgrade_type == UPGRADE_TYPE_INCANTATION) {
		return;
	}

	char* prefix = "+";
	char* suffix = " mgc/s";
	double value = upgrade->mps;

	switch (upgrade->effect) {
		case EFFECT_INCREASE_MPS:
			break;
		case EFFECT_MULT_ADD:
			prefix = "Mult: +";
			suffix = "%%";
			value = upgrade->mult*100;
			break;
		case EFFECT_MULT_MULT:
			prefix = "Mult: *";
			suffix = "";
			value = upgrade->mult;
			break;
		case EFFECT_CLICK_ADD:
			suffix = " mgc/click";
			break;
		case EFFECT_CLICK_MULT:
			suffix = "%% mgc/click";
			value = upgrade->mult*100;
			break;
		// ignore incantation-specific effects
	}
	snprintf(strbuf, 256, "Cost: %s mgc", human_format(upgrade->cost));
	draw_text(strbuf, bptr->centerx, bptr->centery+2-8, FONT_RPG, 16, BLACK, renderer);

	snprintf(strbuf, 256, "%s%s%s", prefix, human_format(value), suffix);
	draw_text(strbuf, bptr->centerx, bptr->centery+16-8, FONT_RPG, 16, BLACK, renderer);
}

int draw_upgrade(const Upgrade* upgrade, const int slot, SDL_Renderer* renderer, const GameState* state){
	Button* bptr = upgrade->button;
	
	// Draw background of button
	int result = draw_button_by_ref(bptr);
	if (result != 0) return result; 

	// Draw ??? text if locked & not an incantation
	if ((upgrade->button->locked) && (upgrade->upgrade_type != UPGRADE_TYPE_INCANTATION)) {
		draw_text("???", bptr->centerx, bptr->centery-12, FONT_RPG, 24, BLACK, renderer);
		return 0;
	}

	// Draw text
	draw_upgrade_text(upgrade, state, renderer);
	
	return 0;
}

int set_draw_color(SDL_Renderer* renderer, const E_ColorIndex color) {
	if (color >= NUM_COLORS) {
		LOG_E("Error: invalid color index %d in set_draw_color\n", color);
		return -1;
	}
	SDL_SetRenderDrawColor(renderer, COL[color].r, COL[color].g, COL[color].b, COL[color].a);
	return 0;
}

void draw_text(const char* text, const int x, const int y, const int font, 
				const int size, const E_ColorIndex color, SDL_Renderer* renderer) {
	static FC_Effect e;
    e.alignment = FC_ALIGN_CENTER;
    e.scale = FC_MakeScale(1.0f,1.0f);
    e.color = COL[color];
	
	const int offset = (font*NUM_PER_FONT) + (size-MIN_FONT_SIZE)/2;
	FC_DrawEffect(FONTS[offset], renderer, x, y, e, text);
}

void draw_magics_info(SDL_Renderer* renderer, Button* buttons, GameState* state) {
	static char str_buf1[256] = "";
	static char str_buf2[256] = "";
	static char mps_buf[56] = "";


	draw_text("The powerful magics of your planet are under siege! You must find and become in tune with", 
				screen_center_x(), 100-9, FONT_RPG, 18, WHITE, renderer);
	draw_text("them in order to fend off the Evil. Too bad no one told you how much you need...", 
				screen_center_x(), 122-9, FONT_RPG, 18, WHITE, renderer);
				
		
	snprintf(str_buf1, 256, "Magics: %s", human_format(round(state->magic_count)));
	// human_format doesn't play nice with snprintf if used multiple times in a single string
	snprintf(mps_buf, 56, "%s", human_format(state->magic_per_second));
	snprintf(str_buf2, 256, "Magics per second: %s (%s * %.2f * %.2f)", human_format(state->magic_per_second*state->magic_multiplier*state->meditate_multiplier), 
				mps_buf, state->magic_multiplier, state->meditate_multiplier);    
				
	draw_text(str_buf1, screen_center_x(), 30-15, FONT_RPG, 30, WHITE, renderer);
	draw_text(str_buf2, screen_center_x(), 60-15, FONT_RPG, 30, WHITE, renderer);
}

void draw_menu_buttons(SDL_Renderer* renderer, Button* buttons) {
	for (int i = 0; i < NUM_MENU_BUTTONS; ++i) {
		draw_button(MENU_BUTTON_OFFSET+i);
	}
}

void draw_menu_upgrades(SDL_Renderer* renderer, Config* config, unsigned int* upgrade_page, unsigned int* princess_page) {
	Button* buttons = config->buttons;
	Upgrade* upgrades = config->upgrades;
	Mix_Chunk** sounds = config->sounds;
	GameState* state = config->state;

	Upgrade* uptr;
	
	char pg_str_buf[4] = "";

	draw_magics_info(renderer, buttons, state);
	draw_button(MAGIC_B);
	
	int upgrade_offset = INCANTATION_OFFSET;
	int count = 10;
	switch (state->current_menu) {
		case MENU_PRINCESS:
			draw_button(NEXT_B);
			snprintf(pg_str_buf, 4, "%d", (*princess_page)+1);
			draw_text(pg_str_buf, screen_center_x()-260, screen_height()-34, FONT_RPG, 20, STAR1, renderer);
			draw_text("/ 2", screen_center_x()-232, screen_height()-34, FONT_RPG, 20, STAR1, renderer);
			upgrade_offset = (*princess_page)*10+PRINCESS_OFFSET;
			if (INCANTATION_OFFSET - upgrade_offset < 10) {
				count = INCANTATION_OFFSET - upgrade_offset;
			}
			break;
		case MENU_UPGRADES:
			draw_button(NEXT_B);
			snprintf(pg_str_buf, 4, "%d", (*upgrade_page)+1);
			draw_text(pg_str_buf, screen_center_x()-260, screen_height()-34, FONT_RPG, 20, STAR1, renderer);
			draw_text("/ 6", screen_center_x()-232, screen_height()-34, FONT_RPG, 20, STAR1, renderer);
			upgrade_offset = (*upgrade_page)*10;
			if (PRINCESS_OFFSET - upgrade_offset < 10) {
				count = PRINCESS_OFFSET - upgrade_offset;
			}
			break;
	}
	for (int i = 0; i < count; ++i) {
		uptr = &(upgrades[upgrade_offset+i]);
		draw_upgrade(uptr, i, renderer, state);
		//draw_upgrade(uptr, i, renderer, state, false);
	}

	draw_menu_buttons(renderer, buttons);
}

void draw_start_medi_button(Config* config) {
	char str_buf[256];
	Button* bptr = &(config->buttons[START_MEDI_B]);
	if (config->state->is_meditating) {		
		draw_text("Currently Meditating", screen_center_x(), bptr->centery-40-13, FONT_RPG, 26, WHITE, config->renderer);
        draw_text("You are currently meditating.", screen_center_x(), bptr->centery-20-8, FONT_RPG, 16, WHITE, config->renderer);
        draw_text("You will be unable to perform any other actions until you are finished.", 
			screen_center_x(), bptr->centery-4-8, FONT_RPG, 16, WHITE, config->renderer);
			
		snprintf(str_buf, 256, "[Time left: %s seconds]", human_format(config->state->meditate_timer/get_fps()));
        draw_text(str_buf, screen_center_x(), bptr->centery+38-10, FONT_RPG, 20, WHITE, config->renderer);
		
	} else {
		if (config->state->meditate_cooldown < 1) {
			draw_button_by_ref(bptr);
			draw_text("[Ready!]", screen_center_x(), bptr->centery+38-10, FONT_RPG, 20, BLACK, config->renderer);
		} else {
			draw_button_by_ref(bptr);
			snprintf(str_buf, 256, "[%s]",frames_to_time_str(config->state->meditate_cooldown, config->state));
			draw_text(str_buf, screen_center_x(), bptr->centery+38-10, FONT_RPG, 20, BLACK, config->renderer);			
		}		

		draw_text("Begin Meditation", screen_center_x(), bptr->centery-40-13, FONT_RPG, 26, BLACK, config->renderer);
		draw_text("Sit down, relax, and increase your alignment with the magical forces.", screen_center_x(), bptr->centery-20-8, FONT_RPG, 16, BLACK, config->renderer);
		draw_text("You will be unable to add new sources during meditation.", screen_center_x(), bptr->centery-4-8, FONT_RPG, 16, BLACK, config->renderer);       
	}
}

void draw_menu_meditate(Config* config) {
	draw_magics_info(config->renderer, config->buttons, config->state);

	if (config->state->is_meditating) {
		Button* bptr = &(config->buttons[MEDI_B]);
		draw_button_by_ref(bptr);
		if (bptr->state == STATE_BTN_IDLE) {
			draw_text(CLICK_STR, bptr->centerx, bptr->centery-15, FONT_RPG, 30, BLACK, config->renderer);
		}		
	} else {
		draw_button(MAGIC_B);
        draw_menu_buttons(config->renderer, config->buttons);
	}
	draw_start_medi_button(config);
	
}

void draw_menu_face_evil(SDL_Renderer* renderer, Config* config) {
	Button* buttons = config->buttons;
	GameState* state = config->state;
	
	Button* bptr;
	char str_buf[256];
	
	draw_magics_info(renderer, buttons, state);
	draw_button(MAGIC_B);
	
	bptr = &(buttons[FACE_EVIL_B]);
	draw_button_by_ref(bptr);
	
	draw_text("Engage with the Evil", screen_center_x(), bptr->centery-40-13, FONT_RPG, 26, WHITE, renderer);
	draw_text("WARNING!!!", screen_center_x(), bptr->centery-20-10, FONT_RPG, 20, WHITE, renderer);
	draw_text("Without true unity, this action will destroy ALL YOUR MAGIC. Proceed with caution!", 
		screen_center_x(), bptr->centery-4-8, FONT_RPG, 16, WHITE, renderer);
		
	snprintf(str_buf, 256, "Cost: %s mgc",human_format(state->magic_count));
	draw_text(str_buf, screen_center_x(), bptr->centery+18-9, FONT_RPG, 18, WHITE, renderer);
	draw_text("[???]", screen_center_x(), bptr->centery+38-10, FONT_RPG, 20, WHITE, renderer); 
	
	draw_menu_buttons(renderer, buttons);
}

void draw_screen_ending(SDL_Renderer* renderer, Config* config) {
	Button* buttons = config->buttons;
	GameState* state = config->state;
	EndState* ending = config->ending_state;

	switch(state->current_menu) {
		case MENU_END_PROMPT1:
			draw_text("You decide to confront the Evil with the magics you've attained.", screen_center_x(), 200-12, FONT_RPG, 24, WHITE, renderer);
			draw_button(END_OK_B);
			break;
		case MENU_END_PROMPT2:
			draw_text("The Evil Forces approach, now is the time to unleash the magics!", screen_center_x(), 200-12, FONT_RPG, 24, WHITE, renderer);
			draw_button(END_OK_B);
			break;
		case MENU_END_FAIL:
			draw_text("But... the Evil Forces reflect your barrage almost immediately.", screen_center_x(), 200-12, FONT_RPG, 24, WHITE, renderer);
            draw_button(END_OK_B);
			break;
		case MENU_END_WIN1:
			// Set blend mode only if it changes from the default or previous value
			// Assuming ending->expl_tx1 and ending->expl_tx2 blend modes are not set elsewhere frequently
			SDL_SetRenderTarget(renderer, ending->expl_tx1);
			if (ending->explosion_delay < 1) {				
				SDL_SetTextureAlphaMod(ending->expl_tx1, (int)ending->explosion_alpha);
				if (ending->explosion_alpha > 225.0f) {
					clear_screen(renderer, B_MENU); // Only clear if necessary
				} else {
					clear_screen(renderer, TRANSPARENT); // Assuming this is necessary every frame
				}
				set_draw_color(renderer, EXPL3);
				SDL_RenderFillCircle(renderer, screen_center_x(), screen_center_y(), (int)(ending->explosion_radius / 2.0f));
				set_draw_color(renderer, EXPL4);
				SDL_RenderFillCircle(renderer, screen_center_x(), screen_center_y(), (int)(ending->explosion_radius / 5.0f));
				
				SDL_SetRenderTarget(renderer, ending->expl_tx2);
				clear_screen(renderer, TRANSPARENT);
				SDL_SetTextureAlphaMod(ending->expl_tx2, (int)ending->explosion_alpha);
				set_draw_color(renderer, EXPL2);
				SDL_RenderFillCircle(renderer, screen_center_x(), screen_center_y(), (int)(ending->explosion_radius / 10.0f));
				set_draw_color(renderer, EXPL1);
				SDL_RenderFillCircle(renderer, screen_center_x(), screen_center_y(), (int)(ending->explosion_radius / 7.0f));

				SDL_SetRenderTarget(renderer, NULL);
				SDL_RenderCopy(renderer, ending->expl_tx1, NULL, NULL);
				SDL_RenderCopy(renderer, ending->expl_tx2, NULL, NULL);

				int ring_r = (int)(ending->explosion_radius / 5.5f);
				//set_draw_color(renderer, EXPL2);
				SDL_RenderFillRing(renderer, screen_center_x(), screen_center_y(), ring_r, ring_r - 80);
				SDL_RenderFillRing(renderer, screen_center_x(), screen_center_y(), ring_r*4, ring_r*4 - 40);
			} else {
				SDL_SetRenderTarget(renderer, ending->expl_tx1);
				clear_screen(renderer, TRANSPARENT);
				SDL_SetTextureAlphaMod(ending->expl_tx1, abs((int)ending->ship_glow_alpha));
				set_draw_color(renderer, SHIP_GLOW);
				SDL_RenderFillCircle(renderer, screen_center_x(), screen_center_y(), 240);
				SDL_SetRenderTarget(renderer, NULL);
				SDL_RenderCopy(renderer, ending->expl_tx1, NULL, NULL);

				for (int i = 0; i < 6; ++i) {
					Shape* s = &(END_SHIP_SHAPES[i]);
					draw_shape(renderer, s);
				}
			}
			break;
		case MENU_END_WIN2:
			draw_text("Congratulations!", screen_center_x(), 160-20, FONT_RPG, 40, WHITE, renderer);
            draw_text("You've harnessed enough magics to restore the planet's natural balance.", screen_center_x(), 200-12, FONT_RPG, 24, WHITE, renderer);
            draw_text("YOU WIN!!!", screen_center_x(), 230-12, FONT_RPG, 24, WHITE, renderer);
            draw_button(END_OK_B);
			break;
	}
}

void draw_screen_title(Config* config) {
	draw_text("SAVE THE MAGICS!!", screen_center_x(), 160, FONT_RPG, 80, WHITE, config->renderer);
	draw_text("A lovely magics-collecting adventure!", screen_center_x(), 234, FONT_RPG, 30, WHITE, config->renderer);
	draw_button(NEW_GAME_B);
	draw_button(LOAD_GAME_B);
	draw_button(OPTIONS_B);
}

void draw_option_aspect(const Config* const config) {
	const char* aspect_strs[2] = {
		"16:9 (Wide)",
		"4:3 (Classic)"
	};
	
	Button* bptr = get_button(OPT_ASPECT_B);
	draw_text(aspect_strs[config->aspect], bptr->centerx+200, bptr->centery-15, FONT_RPG, 30, WHITE, config->renderer);
}

void draw_option_fps(const Config* const config) {
	char str_buf[32];
	snprintf(str_buf, 32, "%d FPS", (int)get_fps());
	Button* bptr = get_button(OPT_FPS_B);
	draw_text(str_buf, bptr->centerx+200, bptr->centery-15, FONT_RPG, 30, WHITE, config->renderer);
	return;
}

void draw_option_autosave(const Config* const config) {
	char str_buf[64] = "None";
	if (config->autosave_interval) {
		snprintf(str_buf, 64, "%d minute(s)", AUTOSAVE_INTERVALS[config->autosave_interval]);
	}
	Button* bptr = get_button(OPT_AUTOSAVE_B);
	draw_text(str_buf, bptr->centerx+200, bptr->centery-15, FONT_RPG, 30, WHITE, config->renderer);
}

void draw_option_import(const Config* const config) {
	return;
}

void draw_screen_options(Config* config) {
	void (*draw_opt_funcs[4])(const Config* const) = {
		draw_option_autosave,
		draw_option_aspect,
		draw_option_fps,
		draw_option_import,
	};
	
#ifndef __MAGICSMOBILE__
	draw_text("Warning: Some settings changes will reload the game.", screen_center_x(), 70, FONT_RPG, 20, WHITE, config->renderer);
#endif

	for (int i = 0; i < NUM_OPTIONS; ++i) {
		draw_button(OPTIONS_OFFSET+i);
		draw_opt_funcs[i](config);
	}
	draw_button(OPT_CONFIRM_B);
}

void draw_screen_save(Config* config) {
	Button* bptr = NULL;
	char save_slot_buf[64] = "";
	
	if (config->state->current_menu >= MENU_SV_CNF_QUIT) {
		draw_text("Would you like to save your efforts? If not, progress may be lost!", \
			screen_center_x(), 200-15, FONT_RPG, 30, WHITE, config->renderer);
		draw_button(SAVE_YES_B);
		draw_button(SAVE_NO_B);
		if (config->state->current_menu == MENU_SV_CNF_QUIT) {
			draw_button(SAVE_CANCEL_B);
		}
	} else if (config->state->current_menu == MENU_SV_CNF_OVERWR) {
		draw_text("Overwrite this save slot? All previous progress will be lost!", \
			screen_center_x(), 200-15, FONT_RPG, 30, WHITE, config->renderer);
			draw_button(SAVE_YES_B);
			draw_button(SAVE_NO_B);
	} else {
		bool locked;
		int start_slot = (config->state->current_menu == MENU_SV_SLOT);
		for (int i = start_slot; i < 4; ++i) {
			bptr = &(config->buttons[SAVE_0_B+i]);
			if (config->saves[i].exists) {
				const char* time_str = time_to_time_str(&(config->saves[i].last_saved));
				draw_button_by_ref(bptr);
				if (i > 0) {
					snprintf(save_slot_buf, sizeof(save_slot_buf), "%d. %s", i, time_str);
					snprintf(save_slot_buf, sizeof(save_slot_buf), "%d. %s", i, time_str);
				} else {
					snprintf(save_slot_buf, sizeof(save_slot_buf), "AUTO. %s", time_str);
				}
			} else {
				bptr->color = LOCKED;
				draw_button_by_ref(bptr);
				snprintf(save_slot_buf, sizeof(save_slot_buf), "%d. NO SAVE DATA", i);
			}
			draw_text(save_slot_buf, bptr->centerx, bptr->centery-12, FONT_RPG, 24, WHITE, config->renderer);
		}
	}
}

void draw_screen_game_loop(Config* config, unsigned int* upgrade_page, unsigned int* princess_page) {
	draw_button(QUIT_B);
	draw_button(MUTE_B);
	
	// draw translucent menu box
	SDL_SetRenderDrawBlendMode(config->renderer, SDL_BLENDMODE_BLEND);
	draw_shape(config->renderer, &BG_SHAPES[BG_MENU_RECT]);				
	SDL_SetRenderDrawBlendMode(config->renderer, SDL_BLENDMODE_NONE);

	if (config->state->current_menu == MENU_MEDITATE) {
		draw_menu_meditate(config);
	} else if (config->state->current_menu == MENU_FACE_EVIL) {
		draw_menu_face_evil(config->renderer, config);
	} else {
		draw_menu_upgrades(config->renderer, config, upgrade_page, princess_page);
	}
	
}

void draw_background(Config* config) {
	draw_starfield(config->renderer);
	
	if ((config->state->current_screen == SCREEN_ENDING) && 
			(config->state->current_menu < MENU_END_WIN2)) {
		return;
	}
			
	if (config->state->is_meditating) {
		draw_background_meditate(config->renderer);
	} else {
		draw_background_planet(config->renderer);
	}
}
