#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "breakout.h"
#include "powerup.h"
#include "sound.h"
#include "color.h"

// GLOBAL CONSTANTS

const uint8_t *	keystate;

// GLOBAL VARIABLES

SDL_Window * 	window;
SDL_Renderer * 	renderer;

int block_colors[numtypes] = { BLACK, YELLOW, GREEN, BLUE, RED };

blocktype_t board_setup[BLOCK_ROWS][BLOCK_COLS] =
{
	{4,4,4,4,4,4,1,1,4,4,4,4,4,4},
	{4,4,4,4,4,4,1,1,4,4,4,4,4,4},
	{3,3,3,3,3,3,1,1,3,3,3,3,3,3},
	{3,3,3,3,3,3,1,1,3,3,3,3,3,3},
	{2,2,2,2,2,2,1,1,2,2,2,2,2,2},
	{2,2,2,2,2,2,1,1,2,2,2,2,2,2},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

enum		{ game, gameover } gamestate;
enum		{ serve, play } playstate;
bool		paused = false;
block_t 	board[BLOCK_ROWS][BLOCK_COLS];


entity_t	player;
entity_t	*firstball;		// ball list
entity_t	*firstlaser;	// laser list
int			cooldown;
int			ballcount = 0;

int 		collisions[2][2]; // use [dx>0][dy>0]

enum { l_lives, l_paused, NUMLABELS };
textlabel_t labels[NUMLABELS];


//
//	Quit
//	Free/Close/Destroy/Clean up all resources and optionally
//	print and error message
//
void Quit (const char * error)
{
	RemoveAllPowerups();
	E_RemoveAll(&firstball);
	S_ShutdownSound();
	T_ShutdownText();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	if (error && *error) {
		puts(error);
		exit(1);
	}
	exit(0);
}



void NewLaser (float x, float y)
{
	entity_t * laser;
	
	laser = E_AddEntity(&firstlaser);
	laser->width = 4;
	laser->height = 8;
//	laser->dy = LASER_SPEED;
	E_SetPosition(laser, x, y);
//	laser->dx = 0;
//	laser->dy = -LASER_SPEED;
    laser->vel.x = 0;
    laser->vel.y = -LASER_SPEED;
}



void NewBall (float x, float y, float dx)
{
	entity_t * ball;
	
	ball = E_AddEntity(&firstball);
	ball->width = BALL_SIZE;
	ball->height = BALL_SIZE;
//	ball->dx = dx;
    ball->vel.x = dx;
	E_SetPosition(ball, x, y);
}


void SetPlayerLives (int newval)
{
	player.lives = newval;
	SetColor(&bright_colors[WHITE]); // update label
	labels[l_lives] = T_MakeLabel_d("Lives ", player.lives, 0, 0);
}


void InitBoard( )
{
	block_t * bl;
	
	for ( int y = 0 ; y < BLOCK_ROWS ; y++ ) {
		for ( int x = 0 ; x < BLOCK_COLS ; x++ ) {
			bl          = &board[y][x];
			bl->type    = board_setup[y][x];
			bl->rect.x  = x * BLOCK_W + 1;
			bl->rect.y  = y * BLOCK_H + BOARD_Y + 1;
			bl->rect.w  = BLOCK_W - 2;
			bl->rect.h  = BLOCK_H - 2;
			bl->top 	= bl->rect.y;
			bl->left 	= bl->rect.x;
			bl->right 	= bl->rect.x + bl->rect.w;
			bl->bottom 	= bl->rect.y + bl->rect.h;
		}
	}
}



//
//  InitGame
//  Reset player and ball positions, etc.
//
void InitGame( int lives )
{
	// player
	memset( &player, 0, sizeof(entity_t) );
	player.width = PLAYER_W;
	player.height = PLAYER_H;
	E_SetPosition( &player, WINDOW_W / 2 - player.width / 2, PLAYER_Y );
	SetPlayerLives( lives );
	
	// ball
	E_RemoveAll( &firstball );
	ballcount = 0;
	NewBall( player.right - player.width / 4, player.top - BALL_SIZE / 2, BALL_SPEED );
}




//
//	DoBlockCollision
//	Block 'bl' has been hit: check from which direction
// 	and update ball position and movement accordingly.
//
void DoBlockCollision( block_t * bl, entity_t * ball )
{
	int 		xdiff, xmin, xmax;
	int 		ydiff, ymax, ymin;
	int 		dir;
	float 		setx, sety;
	const int 	diffs[2] = { left|right, top|bottom }; // use xdiff>ydiff

	S_PlaySound( SND_BRICK );
	
	if ( player.effects & EF_SUPER ) {
		bl->type = dead;
		SDL_Delay( 50 );
		return;
	}

	// calculate hit direction
	ymin = (bl->top > ball->top) ? bl->top : ball->top;
	ymax = (bl->bottom < ball->bottom) ? bl->bottom : ball->bottom;
	ydiff = ymax - ymin;
	xmin = (bl->left > ball->left) ? bl->left : ball->left;
	xmax = (bl->right < ball->right) ? bl->right : ball->right;
	xdiff = xmax - xmin;
    dir = collisions[ball->vel.x>0][ball->vel.y>0] & diffs[xdiff>ydiff];
	
	// correct position
    setx = ball->pos.x;
	sety = ball->pos.y;
	switch ( dir ) {
		case top:		sety = bl->top      - BALL_SIZE / 2; break;
		case bottom:	sety = bl->bottom   + BALL_SIZE / 2; break;
		case left:		setx = bl->left     - BALL_SIZE / 2; break;
		case right:		setx = bl->right    + BALL_SIZE / 2; break;
		default: puts("DoBlockCollision: Error. Weird hit direction!");
			break;
	}
	E_SetPosition( ball, setx, sety );
	
	// adjust motion
    ball->vel.x = (dir == left || dir == right) ? -ball->vel.x : ball->vel.x;
    ball->vel.y = (dir == top || dir == bottom) ? -ball->vel.y : ball->vel.y;
	
	bl->type--;
	if ( bl->type == dead ) {
		DropPowerup( bl );
	}
}




#pragma mark - UPDATE

void UpdateBall( entity_t * ball )
{
	int 		x, y;
	block_t * 	bl;
	
	if ( playstate == serve ) {
        // ball moves along with paddle during serve
        ball->vel = player.vel;
        E_Accellerate( ball );
        ball->vel = Vector(0, 0);
		return;
	}
	
    E_Accellerate(ball);
	
	// off bottom?
	if ( ball->top > WINDOW_H )
	{
		S_PlaySound(SND_GOAL);
//		E_RemoveEntity(ball, &firstball);
		ball->active = false;
		ballcount--;
		
		if ( !ball->active && !firstball->next ) {
//		if (firstball == NULL) { // no more balls!
			RemoveAllPowerups();
			if (--player.lives < 0) {
				gamestate = gameover;
			} else {
				playstate = serve;
				InitGame(player.lives);
			}
 		}
	}
	
	// hit side?
	if (ball->left < 0 || ball->right > WINDOW_W) {
        clamp(ball->pos.x, BALL_MIN_X, BALL_MAX_X);
        E_SetPosition(ball, ball->pos.x, ball->pos.y);
        ball->vel.x = -ball->vel.x;
		S_PlaySound(SND_SIDEHIT);
	}
	// hit top?
	if (ball->top < 0) {
        E_SetPosition(ball, ball->pos.x, BALL_MIN_Y);
        ball->vel.y = -ball->vel.y;
		S_PlaySound(SND_SIDEHIT);
	}
	
	// paddle collision?
	if (E_Collision(ball, &player)) {
        ball->pos.y = player.pos.y - BALL_SIZE; // correct position
		if (player.effects & EF_STICKY) { // sticky
			ball->effects |= EF_STICKY;
            ball->vel.x = 0;
            ball->vel.y = 0;
            ball->pos.y = player.pos.y - BALL_SIZE;
			S_PlaySound(SND_BRICK);
		} else { // bounce
            ball->vel.y = -ball->vel.y; // change direction
			S_PlaySound(SND_PADHIT);
		}
	}
	
	// block collision
	for (y=0 ; y<BLOCK_ROWS ; y++) {
		for (x=0 ; x<BLOCK_COLS ; x++)
		{
			bl = &board[y][x];
			if (bl->type == dead)
				continue;
			
			if (ball->top < bl->bottom &&
				ball->bottom > bl->top &&
				ball->right > bl->left &&
				ball->left < bl->right)
			{
				DoBlockCollision(bl, ball);
			}
		}
	}
	
	// handle multi-ball
	if (ball->effects & EF_MULTI) {
		vector_t new = ball->vel;
		RotateVector(&new, 10);
		RotateVector(&ball->vel, -10);
        NewBall(ball->pos.x, ball->pos.y, new.x);
		ball->effects &= ~EF_MULTI;
	}
}



void UpdatePlayer ()
{
	int minx, maxx;
	entity_t * ball;

    E_Accellerate(&player);
	
	// correct position
	minx = player.rect.w / 2;
	maxx = WINDOW_W - player.rect.w / 2;
	if (player.pos.x < minx || player.pos.x > maxx) {
		clamp(player.pos.x, minx, maxx);
		E_SetPosition(&player, player.pos.x, player.pos.y);
		player.vel = Vector(0, 0);
	}
	
	ball = firstball;
	while (ball) {
		if (ball->effects & EF_STICKY) {
			AddVector(&ball->pos, &player.vel);
		}
		ball = ball->next;
	}
}

void UpdateLasers ()
{
	entity_t * l;
	block_t * bl;
	
	l = firstlaser;
	while (l)
	{
		if (l->active)
		{
            E_Accellerate(l);
			
			if (l->bottom < 0) {
				l->active = false;
			}
			
			// block collision
			for ( int y = 0; y < BLOCK_ROWS; y++ ) {
				for ( int x = 0; x < BLOCK_COLS; x++ ) {
					bl = &board[y][x];
					if (bl->type == dead)
						continue;
					
					if (l->top < bl->bottom &&
						l->bottom > bl->top &&
						l->right > bl->left &&
						l->left < bl->right)
					{
						l->active = false;
						S_PlaySound(SND_BRICK);
						bl->type--;
						if ( bl->type == dead ) {
							DropPowerup( bl );
						}
					}
				}
			}
		}
		l = l->next;
	}
}



#pragma mark - DRAWING


void DrawShadow (SDL_Rect rect)
{
	rect.x += SHAWDOW_OFFSET;
	rect.y += SHAWDOW_OFFSET;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SHADOW_OPACITY);
	SDL_RenderFillRect(renderer, &rect);
}



void DrawBoard ()
{
	int 		x, y;
	block_t * 	bl;
	
	for (y=0 ; y<BLOCK_ROWS ; y++) {
		for (x=0 ; x<BLOCK_COLS ; x++)
		{
			bl = &board[y][x];
			if (!bl->type)
				continue;
            DrawShadow(bl->rect);
			SetColor(&dark_colors[bl->type]);
			SDL_RenderFillRect(renderer, &bl->rect);
			SetColor(&bright_colors[bl->type]);
			SDL_RenderDrawRect(renderer, &bl->rect);
		}
	}
}

void DrawBalls ()
{
	entity_t * ball;
	
    ball = firstball;
    while (ball) {
        if (player.effects & EF_SUPER) {
            int c = rand() % NUMCOLORS;
            E_DrawEntity(ball, c);
        } else {
            E_DrawEntity(ball, WHITE);
        }
        ball = ball->next;
    }
}


void DrawGun (float x)
{
	SDL_Rect base = { x, player.top - PLAYER_H, PLAYER_H, PLAYER_H };
	SDL_Rect tip = { x+PLAYER_H/4+1, player.top-(PLAYER_H*2), PLAYER_H/2-1, PLAYER_H};
	SDL_SetRenderDrawColor(renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &base);
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &tip);
}



void DrawPlayer ()
{
	if (player.effects & EF_STICKY) {
		E_DrawEntity(&player, GREEN);
	} else {
		E_DrawEntity(&player, WHITE);
	}
	
	if (player.effects & EF_LASER)
	{
		DrawGun(player.left);
		DrawGun(player.right - PLAYER_H);
	}
}

void DrawLasers ()
{
	entity_t * l;
	
	SetBrightColor(GREEN);
	l = firstlaser;
	while (l) {
		if (l->active) {
			SDL_RenderFillRect(renderer, &l->rect);
		}
		l = l->next;
	}
}




void DrawLabels ()
{
    for ( int i = 0 ; i < NUMLABELS ; i++ ) {
        if ( i == l_paused ) {
            if ( paused && SDL_GetTicks() % 600 < 300 )
                T_RenderLabel( &labels[i] );
            else continue;
        }
        T_RenderLabel( &labels[i] );
    }
}




#pragma mark -

//
//	DoKeyDown
//	Game Loop key down processing
//
void DoKeyDown( SDL_Keycode key )
{
	entity_t * ball;
	
	switch ( key )
	{
        case SDLK_ESCAPE: {
			Quit(NULL);
			break;
        }
			
        case SDLK_p: {
			paused = !paused;
			break;
        }
			
        case SDLK_SPACE: {
			if ( playstate == serve ) {
				ball = firstball;
				ball->vel = Vector( BALL_SPEED, -BALL_SPEED );
				playstate = play;
			} else if ( playstate == play ) { // release all sticky balls
				ball = firstball;
				while ( ball ) {
					if ( ball->effects & EF_STICKY ) {
						ball->vel = Vector( BALL_SPEED, -BALL_SPEED );
						ball->effects &= ~EF_STICKY;
					}
					ball = ball->next;
				}
			}
			break;
        }
			
		default:
			break;
	}
}



//
//	TitleLoop
//	Display the title screen and wait for key press
//
#define TITLE_X		64
#define TITLE_Y 	128
#define NUM_LABELS	7
#define row(x)		TITLE_Y + (font.size*font.scale) * x
void TitleLoop ()
{
	SDL_Event 	event;
	int 		i;
	textlabel_t	labels[NUM_LABELS];
	
	const char * labeltext[NUM_LABELS] = {
		"Thomas Foster",
		"GAME PORTFOLIO",
		"==============",
		"02 Breakout",
		"Press Any Key to Play", // 4
		"CONTACT: (716) 410-4450",
		"foster.pianist@gmail.com"
	};
	int rows[NUM_LABELS] = { 0, 2, 3, 5, 7, 9, 10 };
	
	for (i=0 ; i<NUM_LABELS ; i++) {
		labels[i] = T_MakeLabel(labeltext[i], TITLE_X, row(rows[i]));
	}
	
	while (1)
	{
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) Quit(NULL);
			if (event.type == SDL_KEYDOWN) return;
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		for (i=0 ; i<NUM_LABELS ; i++) {
			if (i == 4 && SDL_GetTicks() % 600 < 300) continue;
			T_RenderLabel(&labels[i]);
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}
}


void GameOverLoop ()
{
	SDL_Event 	event;
	SetBrightColor(WHITE);
	textlabel_t go = T_MakeLabel("GAME OVER!", CENTERED, CENTERED);
	
	do
	{
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) Quit(NULL);
			if (event.type == SDL_KEYDOWN) {
				gamestate = game;
			}
		}
		
		SetBrightColor(BLACK);
		SDL_RenderClear(renderer);
		T_RenderLabel(&go);
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	} while (gamestate == gameover);
}




void GameLoop ()
{
	SDL_Event   event;
	entity_t *  ball;
	
	playstate = serve;
	InitBoard();
	InitGame(3);
	
	// game loop
	do
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				Quit(NULL);
			}
			if (event.type == SDL_KEYDOWN) {
				DoKeyDown(event.key.keysym.sym);
			}
		}
		
		// update player speed
		player.vel = Vector(0, 0);
		if ( keystate[SDL_SCANCODE_LEFT] ) {
			player.vel.x = -PLAYER_SPEED;
		}
		if ( keystate[SDL_SCANCODE_RIGHT] ) {
			player.vel.x = PLAYER_SPEED;
		}
        
        // shoot lasers?
		if ( keystate[SDL_SCANCODE_UP] &&
			player.effects & EF_LASER &&
			cooldown <= 0)
		{
			NewLaser(player.left+PLAYER_H/2, player.top-12);
			NewLaser(player.right-PLAYER_H/2, player.top-12);
			cooldown = LASER_DELAY;
		}
			
		
        //
		// UPDATE GAME
        //

		if (!paused)
		{
			if (cooldown) {
				--cooldown;
			}
			UpdatePowerups(&player);
			UpdatePlayer();
			
			ball = firstball;
			while (ball) {
				if (ball->active) {
					UpdateBall(ball);
				}
				ball = ball->next;
			}
			UpdateLasers();
			
			// remove inactive balls
			ball = firstball;
			while (ball) {
				if (!ball->active) {
					E_RemoveEntity(ball, &firstball);
				}
				ball = ball->next;
			}
		}
		
        
        //
		// RENDER
        //
		
        SDL_SetRenderDrawColor(renderer, 0x04, 0x14, 0x40, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
        
		DrawBoard();
		DrawBalls();
		DrawPlayer();
		DrawPowerups();
		DrawLasers();
        DrawLabels();
        
		SDL_RenderPresent(renderer);
	} while (gamestate == game);
}




int main ()
{
	srand( (unsigned)time(NULL) ); // TODO: better prng
	keystate = SDL_GetKeyboardState(NULL);
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Quit("main: Error. Could not init SDL");
	}
	
	window = SDL_CreateWindow("Blockawocka", 0, 0, WINDOW_W, WINDOW_H, 0);
	if (!window) Quit("main: Error. Could not init window!");
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) Quit("main: Error. Could not init renderer!");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	T_InitText(renderer);
	S_InitSound();
	InitPowerupImages();
	
	SetBrightColor(WHITE);
	labels[l_paused] = T_MakeLabel("PAUSED", CENTERED, CENTERED);
		
	// init collision look-up table
	for (int y=0 ; y<2 ; y++) {
		for (int x=0 ; x<2 ; x++) {
			int cy = y==0 ? left : right;
			int cx = x==0 ? top : bottom;
			collisions[y][x] = (0xF^cx) & (0xF^cy);
			printf("col[%d][%d]: %d\n",y,x,collisions[y][x]);
		}
	}
	

//	TitleLoop();
	
	while (1) {
		switch (gamestate) {
			case game:
				GameLoop();
				break;
			case gameover:
				GameOverLoop();
				break;
			default:
				break;
		}
	}

}
