#include "raylib.h"

#include "state.h"
#include "interface.h"

Texture bird_img;
Sound game_over_snd;


void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "game");
	SetTargetFPS(60);
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}

// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(RAYWHITE);

    
    StateInfo info = state_info(state) ;
    float ball_x = info->ball->rect.x ;
	//κραταμε το offset σε σχεση με το χ της μπαλας για τον σωστο σχεδιασμο των αντικειμενων 
	//που βρισκονται πολυ δεξια ,για τη μεταφραση των συντεταγμενων τους στην πιστα
    float x_offset = 100 - ball_x ;

	//σχεδιαζουμε τη μπαλα
    DrawRectangle(ball_x + x_offset, info->ball->rect.y, info->ball->rect.width, info->ball->rect.height, BLUE);

    List objs = state_objects(state, ball_x - SCREEN_WIDTH, ball_x + SCREEN_WIDTH);

    for(ListNode node = list_first(objs) ;
        node != LIST_EOF ;
        node = list_next(objs, node)      ){
            Object object = list_node_value(objs, node) ;
            if(object->type == PLATFORM){
				//οι ασταθεις πλατφορμες ειναι κοκκινες ενω οι υπολοιπες πρασινες
				if(object->unstable)
                	DrawRectangle(object->rect.x + x_offset, object->rect.y, object->rect.width, object->rect.height, RED);
				else
					DrawRectangle(object->rect.x + x_offset, object->rect.y, object->rect.width, object->rect.height, GREEN);
            }
            else if(object->type == STAR){
                DrawRectangle(object->rect.x + x_offset, object->rect.y, object->rect.width, object->rect.height, YELLOW);
            }
        }


	// Σχεδιάζουμε το σκορ και το FPS counter
	DrawText(TextFormat("%04i", info->score), 20, 20, 40, GRAY);
	DrawFPS(SCREEN_WIDTH - 80, 0);

	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	if (!info->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}

	EndDrawing();
}
