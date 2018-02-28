#include "StateMachine.h"

/*
state_game* newGamestate_game() {
	state_game* gamestate_game = (state_game*) malloc(sizeof(state_game));

	gamestate_game->gameState = MAIN_MENU;

	return gamestate_game;
}*/
/*
void check_menu(event_game evt, state_game* gamestate_game) {

	switch (gamestate_game->gameState) {

	case MAIN_MENU:
		if (evt == PLAY)
			gamestate_game->gameState = GAME;
		else if (evt == HIGHSCORE)
			gamestate_game->gameState = SCORES;

	case GAME:
		if (evt == COLLISION)
			gamestate_game->gameState = GAMEOVER;
		else if (evt == ESC)
			gamestate_game->gameState = GAMEOVER;

	case GAMEOVER:
		 if (evt == HOME)
			gamestate_game->gameState = MAIN_MENU;

		break;
	default:
		break;
	}

	return;
}

void deleteGamestate_game(state_game* gamestate_game) {
	free(gamestate_game);
	return;
}
*/
