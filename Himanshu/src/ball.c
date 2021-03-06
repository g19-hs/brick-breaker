/*
 * ball.c
 *
 *  Created on: February 8th, 2015
 *      Author: Alan Larson
 */

#include "../include/ball.h"

void InitializeBall(Ball* ball) {
	ball->x_pos = DEFAULT_BALL_POS_X;
	ball->y_pos = DEFAULT_BALL_POS_Y;
	ball->x_frequency = DEFAULT_BALL_FREQ_X;
	ball->y_frequency = DEFAULT_BALL_FREQ_Y;
	ball->x_dir = DEFAULT_BALL_DIR_X;
	ball->y_dir = DEFUALT_BALL_DIR_Y;
	ball->color = DEFAULT_BALL_COLOR;

}

int CheckTouchedRenderObjectsX(Set *touchedRenderObjects,
		RenderObjectStructure *renderObjectStructure, int xStart, int yStart,
		int numPoints) {

	const int xCursor = xStart;
	const int xRenderObject = xCursor / RENDER_OBJECT_WIDTH;
	int yCursor;
	int yRenderObject;

	int renderObjectCursor;
	int renderObjectTouched = 0;

	if ((xStart < 0) || (yStart < 0) || (xStart >= SCREEN_WIDTH)
			|| ((yStart + numPoints) >= SCREEN_HEIGHT))
		return renderObjectTouched;

	for (yCursor = yStart; yCursor < (yStart + numPoints); yCursor++) {
		yRenderObject = yCursor / RENDER_OBJECT_HEIGHT;

		renderObjectCursor = (yRenderObject * NUM_RENDER_OBJECTS_WIDTH)
				+ xRenderObject;
		// Check for overflow
		if (renderObjectCursor >= NUM_RENDER_OBJECTS_TOTAL) {
			break;
		}

		if (renderObjectStructure->color[renderObjectCursor]
				!= SCREEN_BACKGROUND_COLOR) {
			renderObjectTouched++;
			AddToSet(touchedRenderObjects, renderObjectCursor);
		}

	}
	return renderObjectTouched;
}

int CheckTouchedRenderObjectsY(Set *touchedRenderObjects,
		RenderObjectStructure *renderObjectStructure, int xStart, int yStart,
		int numPoints) {

	int xCursor;
	int xRenderObject;
	const int yCursor = yStart;
	const int yRenderObject = (yCursor) / RENDER_OBJECT_HEIGHT;

	int renderObjectCursor;
	int renderObjectTouched = 0;

	if ((xStart < 0) || (yStart < 0) || ((xStart + numPoints) >= SCREEN_WIDTH)
			|| ((yStart >= SCREEN_HEIGHT)))
		return renderObjectTouched;

	for (xCursor = xStart; xCursor < (xStart + numPoints); xCursor++) {
		xRenderObject = xCursor / RENDER_OBJECT_WIDTH;

		renderObjectCursor = (yRenderObject * NUM_RENDER_OBJECTS_WIDTH)
				+ xRenderObject;
		// Check for overflow
		if (renderObjectCursor >= NUM_RENDER_OBJECTS_TOTAL) {
			break;
		}

		if (renderObjectStructure->color[renderObjectCursor]
				!= SCREEN_BACKGROUND_COLOR) {
			renderObjectTouched++;
			AddToSet(touchedRenderObjects, renderObjectCursor);
		}

	}
	return renderObjectTouched;
}
int abs_diff(int paddle_middle, int ball_middle, Ball* ball) {
	int result = (paddle_middle - ball_middle);
	if (result < 0) {
		result *= -1;
		ball->x_dir = 1;
	} else
		ball->x_dir = -1;
	return result;

}
int collision_m[7] = { BALL_SPEED_1, BALL_SPEED_2, BALL_SPEED_3, BALL_SPEED_4,
		BALL_SPEED_5, BALL_SPEED_6, BALL_SPEED_7 };
void CheckPaddleCollision(Paddle* paddle, Ball* ball, int* hitX, int* hitY,
		int newX, int newY) {
	// Check for Vertical collision with paddle
	if ((newX + BALL_OBJECT_WIDTH) > paddle->x_pos) {
		if (newX < (paddle->x_pos + DEFAULT_PADDLE_WIDTH)) {
			// Ball moving downwards
			if (ball->y_dir == 1) {
				if (newY + BALL_OBJECT_HEIGHT == INITIAL_PADDLE_Y_POS) {
					(*hitY)++;
					int paddle_middle = (paddle->x_pos
							+ DEFAULT_PADDLE_WIDTH / 2);
					int ball_middle = newX + (BALL_OBJECT_WIDTH / 2);
					int abs_dist = abs_diff(paddle_middle, ball_middle, ball);
					ball->x_frequency = collision_m[(abs_dist / 4)];
					printf("%d\n", collision_m[(abs_dist / 4)]);

				}

			} // Ball moving upwards
			else {
				if (newY == (INITIAL_PADDLE_Y_POS + DEFAULT_PADDLE_HEIGHT)) {
					(*hitY)++;
				}
			}
		}

	}

	// Check for horizontal collison with paddle
	if ((newY + BALL_OBJECT_HEIGHT) > INITIAL_PADDLE_Y_POS) {
		if (newY < (INITIAL_PADDLE_Y_POS + DEFAULT_PADDLE_HEIGHT)) {
			// Ball moving towards right
			if ((ball->x_dir) == 1) {
				if ((newX + BALL_OBJECT_WIDTH) == paddle->x_pos) {
					(*hitX)++;
				}
			} // Ball moving towards left
			else {
				if (newX == (paddle->x_pos + DEFAULT_PADDLE_WIDTH)) {
					(*hitX)++;
				}
			}
		}

	}

}
int MoveBall(RenderObjectStructure *renderObjectStructure,
		BlockObjectStructure *blockObjectStructure, Paddle* paddle, Ball* ball,
		unsigned int currentFrame, MusicData* blockHitSound) {

	// Set will contain the index all the touched render objects
	Set touchedRenderObjects;
	int xMoving;
	int yMoving;

	// Keep track of the potential new positions
	int newX;
	int newY;
	// Keep track if the ball hits an object or not
	int hitX = 0;
	int hitY = 0;

	// Count the number of points touched
	int pointsTouchedX = 0;
	int pointsTouchedY = 0;

	int setCursor;
	int paddle_posi;
	// Clear the set, no objects have been hit
	ClearSet(&touchedRenderObjects);

	// Check if x/y are moving on the ball
	xMoving = IS_MOVING(ball->x_frequency, currentFrame);
	yMoving = IS_MOVING(ball->y_frequency, currentFrame);

	// Ball hasn't moved, return from the function
	if (!xMoving && !yMoving)
		return 1;

	// Compute new positions
	//paddle_posi = paddleposition(spi_read);

	newX = ball->x_pos + (xMoving ? ball->x_dir : 0);
	newY = ball->y_pos + (yMoving ? ball->y_dir : 0);

	// Check for box collisions
	if (xMoving) {
		pointsTouchedX = CheckTouchedRenderObjectsX(&touchedRenderObjects,
				renderObjectStructure,
				newX + ((ball->x_dir == 1) ? (BALL_OBJECT_WIDTH) : 0), // If ball moved right, check right side
				newY,
				BALL_OBJECT_HEIGHT + 1);
	}
	if (yMoving) {
		pointsTouchedY = CheckTouchedRenderObjectsY(&touchedRenderObjects,
				renderObjectStructure, newX, // If ball moved right, check right side
				newY + ((ball->y_dir == 1) ? (BALL_OBJECT_HEIGHT) : 0),
				BALL_OBJECT_WIDTH + 1);
	}
	if ((pointsTouchedY == pointsTouchedX) && pointsTouchedX
			&& pointsTouchedY) {
		hitX++;
		hitY++;
		printf("Block Corner hit!\n");
		//play_sound(blockHitSound);

	} else if (pointsTouchedY < pointsTouchedX) {
		hitX++;
		printf("Block X hit!\n");
		//play_sound(blockHitSound);
	} else if (pointsTouchedY > pointsTouchedX) {
		hitY++;
		printf("Block Y hit!\n");
		//play_sound(blockHitSound);
	}

	// Check for wall collisions
	if (CHECK_WALL_COLLISION_X(newX)) {
		hitX++;
		printf("Wall hit X!\n");
		//play_sound(blockHitSound);
	}

	if (CHECK_BOTTOM_WALL(newY)) {
		hitY++;
		//return 0;
	} else if (CHECK_TOP_WALL(newY)) {
		hitY++;
		printf("Wall hit Y!\n");
		//play_sound(blockHitSound);
	}

	//Check for paddle collision
	CheckPaddleCollision(paddle, ball, &hitX, &hitY, newX, newY);

	//	Check paddle collision

	// Erase the ball at its current position
	EraseBall(ball);

	// If ball doesn't hit anything X wise, let it move to the new Yposition
	if (!hitX) {
		ball->x_pos = newX;
	}
	// Reverse the X direction if it did hit something
	else {
		ball->x_dir *= -1;
	}

	// If ball doesn't hit anything Y wise, let it move to the new Y position
	if (!hitY) {
		ball->y_pos = newY;
	}
	// Reverse the Y direction if it did hit something
	else {
		ball->y_dir *= -1;
	}

	// Update the block structure based off the Render Objects in the Set that got hit by the ball
	for (setCursor = 0; setCursor < touchedRenderObjects.numValues;
			setCursor++) {
		HitBlockFromRenderObject(blockObjectStructure, renderObjectStructure,
				touchedRenderObjects.values[setCursor]);
	}

	// Draw the ball at its new position
	//

	DrawBall(ball);
	return 1;
}
