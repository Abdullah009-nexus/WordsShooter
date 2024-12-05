//============================================================================
// Name        : cookie-crush.cpp
// Author      : Abdullah Shoaib
// Roll No.	   : I21-0645
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Cookie  Crush...
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

//#include <GL/gl.h>
//#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include "util.h"
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 30

string * dictionary;
int dictionarysize = 370099;
#define KEY_ESC 27 // A

string feedbackMessage; // Holds the feedback message (e.g., "Word Found!" or "Invalid Word!")
int feedbackTimer = 0;  // Timer to show feedback for a limited time (in frames)


// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...
struct MovingLetter {
    int alphabet; // The alphabet index
    float x, y;   // Position in pixels
    float dx, dy; // Direction/speed per frame
    bool active;  // Is the letter currently moving
};

MovingLetter shooterLetter;


//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBindTexture(GL_TEXTURE_2D, (cname >= 0 && cname < nalphabets) ? mtid[cname] : 0);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
int GetAlphabet() {
	return GetRandInRange(1, 26);
}

void Pixels2Cell(int px, int py, int &cx, int &cy) {
    cx = px / (2 * bradius);
    cy = (height - py) / (2 * bradius) - 1;
}

void Cell2Pixels(int cx, int cy, int &px, int &py) {
    px = cx * (2 * bradius) + bradius - awidth / 2;
    py = cy * (2 * bradius) + bradius - aheight / 2;
}

void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0); //changed from -1 to 0
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
/*
* Main Canvas drawing function.
* */
void DisplayFunction() {
    // Ensure the board is initialized
    if (board == nullptr) {
        board = new int*[nycells + 1];
        for (int i = 0; i <= nycells; ++i) {
            board[i] = new int[nxcells];
            for (int j = 0; j < nxcells; ++j) {
                board[i][j] = (i < nfrows) ? GetAlphabet() : -1; // Fill top rows with alphabets
            }
        }
        // Shooter's alphabet
        board[nycells][nxcells / 2] = GetAlphabet();
    }

    glClearColor(1, 1.0, 1.0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the board
    for (int row = 0; row < nycells; ++row) {
        for (int col = 0; col < nxcells; ++col) {
            if (board[row][col] != -1) {
                DrawAlphabet((alphabets)board[row][col], col * (2 * bradius), height - (row + 1) * (2 * bradius));
            }
        }
    }

    // Draw the shooter
    if (shooterLetter.active) {
        DrawAlphabet((alphabets)shooterLetter.alphabet, shooterLetter.x, shooterLetter.y);
    } else {
        // Draw the shooter at the bottom center
        int shooter_px = (nxcells / 2) * (2 * bradius) - awidth / 2;
        int shooter_py = 0;
        DrawAlphabet((alphabets)board[nycells][nxcells / 2], shooter_px, shooter_py);
    }

    // Display score
    DrawString(40, height - 20, width, height + 5, "Score: " + to_string(score), colors[BLUE_VIOLET]);

    glutSwapBuffers();
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
    // Shooter's current position (in the last row)
    int shooter_col = nxcells / 2;  // Default center column
    for (int i = 0; i < nxcells; ++i) {
        if (board[nycells][i] != -1) {
            shooter_col = i;
            break;
        }
    }

    if (key == GLUT_KEY_LEFT) {
        // Move shooter left if not at the leftmost column
        if (shooter_col > 0) {
            board[nycells][shooter_col - 1] = board[nycells][shooter_col];
            board[nycells][shooter_col] = -1; // Clear the previous position
        }
    }
    else if (key == GLUT_KEY_RIGHT) {
        // Move shooter right if not at the rightmost column
        if (shooter_col < nxcells - 1) {
            board[nycells][shooter_col + 1] = board[nycells][shooter_col];
            board[nycells][shooter_col] = -1; // Clear the previous position
        }
    }
    else if (key == GLUT_KEY_UP) {
        // Optional: Add functionality for up arrow key (e.g., increase speed, special action)
        // Example: Shooter shoots faster or performs some unique action
        cout << "Up arrow pressed. Implement desired functionality here." << endl;	
    }
    else if (key == GLUT_KEY_DOWN) {
        // Optional: Add functionality for down arrow key (e.g., slow down, defensive action)
        cout << "Down arrow pressed. Implement desired functionality here." << endl;
    }

    // Redraw the screen after handling key input
    glutPostRedisplay();
}

/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void HomePage() {
    // Set background color
    glClearColor(0.2, 0.2, 0.2, 1.0); // Dark gray background
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Title
    DrawString(width / 2 - 100, height - 100, width, height, "Word Shooter", colors[WHITE]);

    // Draw Menu Options
    DrawString(width / 2 - 50, height / 2 + 50, width, height, "1. Start", colors[GREEN]);
    DrawString(width / 2 - 50, height / 2, width, height, "2. Instructions", colors[BLUE]);
    DrawString(width / 2 - 50, height / 2 - 50, width, height, "3. Exit", colors[RED]);

    glutSwapBuffers(); // Display the menu
}

void InstructionsPage() {
    glClearColor(0.2, 0.2, 0.2, 1.0); // Dark gray background
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Title
    DrawString(width / 2 - 100, height - 100, width, height, "Instructions", colors[WHITE]);

    // Draw Instructions
    DrawString(50, height - 150, width, height, "1. Use mouse to aim and shoot letters.", colors[BLUE]);
    DrawString(50, height - 200, width, height, "2. Form valid words to earn points.", colors[GREEN]);
    DrawString(50, height - 250, width, height, "3. Avoid letting the board fill up.", colors[RED]);
    DrawString(50, height - 300, width, height, "4. Press ESC to exit the game at any time.", colors[WHITE]);

    // Back to Home
    DrawString(width / 2 - 50, 50, width, height, "Press B to go back", colors[YELLOW]);

    glutSwapBuffers();
}



void MouseMoved(int x, int y) {
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls
	 // Convert mouse position to board coordinates
    int targetCellX, targetCellY;
    Pixels2Cell(x, y, targetCellX, targetCellY);

    // Potentially update shooter or aim direction
    // You could visualize an aim line or rotate a shooter sprite

}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */
void MoveAlphabet(int targetCellX, int targetCellY) {
    static int currentX = nxcells / 2; // Shooter's initial position (center of board)
    static int currentY = nycells - 1; // Shooter's row (bottom-most row)

    // Convert target cell coordinates to pixel coordinates
    int targetPixelX, targetPixelY;
    Cell2Pixels(targetCellX, targetCellY, targetPixelX, targetPixelY);

    // Convert current position to pixel coordinates
    int currentPixelX, currentPixelY;
    Cell2Pixels(currentX, currentY, currentPixelX, currentPixelY);

    // Animation: Incrementally move the alphabet
    while (currentPixelX != targetPixelX || currentPixelY != targetPixelY) {
        // Update current position towards target
        if (currentPixelX < targetPixelX) currentPixelX++;
        if (currentPixelX > targetPixelX) currentPixelX--;
        if (currentPixelY < targetPixelY) currentPixelY++;
        if (currentPixelY > targetPixelY) currentPixelY--;

        // Redraw the alphabet at the new position
        DrawAlphabet((alphabets)GetAlphabet(), currentPixelX, currentPixelY, awidth, aheight);

        // Delay to simulate smooth movement
        glutPostRedisplay();
        usleep(1000); // Adjust delay for smoother animations
    }

    // After reaching the target position, update the board
    board[currentY][currentX] = -1; // Clear the previous position
    board[targetCellY][targetCellX] = GetAlphabet(); // Set new position

    currentX = targetCellX; // Update shooter position
    currentY = targetCellY;

    // Provide the shooter with a new random alphabet for the next shot
    board[nycells][nxcells / 2] = GetAlphabet(); // Generate a new letter for shooter
}

bool IsWordInDictionary(const char* word) {
    for (int i = 0; i < dictionarysize; ++i) {
        if (dictionary[i] == word) {
            return true;
        }
    }
    return false;
}
void HandleWordDetection(int row, int col) {
    // Directions for horizontal, vertical, and diagonal checks
    int directions[8][2] = {
        {-1, 0}, {1, 0},  // Vertical
        {0, -1}, {0, 1},  // Horizontal
        {-1, -1}, {1, 1}, // Diagonal /
        {-1, 1}, {1, -1}  // Diagonal 
    };

    char word[10];              // Array to store a word (max 9 characters + '\0')
    int cells_to_clear[100][2]; // Array to store cells that form valid words
    int clear_count = 0;        // Number of cells to clear

    bool wordFound = false;

    for (int dir = 0; dir < 4; ++dir) { // Check in 4 main directions
        int temp_cells[10][2]; // Temporary storage for cells in the current direction
        int temp_count = 0;    // Number of cells in the current word
        int word_length = 0;   // Length of the current word

        // Check in the positive direction
        for (int step = 0; step < 5; ++step) { // Limit to max 5 cells in a direction
            int r = row + step * directions[dir * 2][0];
            int c = col + step * directions[dir * 2][1];

            // Stop if out of bounds or cell is empty
            if (r < 0 || r >= nycells || c < 0 || c >= nxcells || board[r][c] == -1)
                break;

            word[word_length] = 'A' + board[r][c];
            temp_cells[temp_count][0] = r;
            temp_cells[temp_count][1] = c;
            word_length++;
            temp_count++;
        }

        // Null-terminate the word
        word[word_length] = '\0';

        // Check if the word is valid
        if (word_length >= 2 && IsWordInDictionary(word)) {
            wordFound = true;
            for (int i = 0; i < temp_count; ++i) {
                cells_to_clear[clear_count][0] = temp_cells[i][0];
                cells_to_clear[clear_count][1] = temp_cells[i][1];
                clear_count++;
            }
        }

        // Reset for the negative direction
        temp_count = 0;
        word_length = 0;

        for (int step = 0; step < 5; ++step) { // Limit to max 5 cells in the opposite direction
            int r = row + step * directions[dir * 2 + 1][0];
            int c = col + step * directions[dir * 2 + 1][1];

            // Stop if out of bounds or cell is empty
            if (r < 0 || r >= nycells || c < 0 || c >= nxcells || board[r][c] == -1)
                break;

            word[word_length] = 'A' + board[r][c];
            temp_cells[temp_count][0] = r;
            temp_cells[temp_count][1] = c;
            word_length++;
            temp_count++;
        }

        // Null-terminate the word
        word[word_length] = '\0';

        // Check if the word is valid
        if (word_length >= 2 && IsWordInDictionary(word)) {
            wordFound = true;
            for (int i = 0; i < temp_count; ++i) {
                cells_to_clear[clear_count][0] = temp_cells[i][0];
                cells_to_clear[clear_count][1] = temp_cells[i][1];
                clear_count++;
            }
        }
    }

    // Clear the cells that form valid words and update the score
    for (int i = 0; i < clear_count; ++i) {
        int r = cells_to_clear[i][0];
        int c = cells_to_clear[i][1];
        board[r][c] = -1; // Clear the cell
        score++;          // Increment score
    }

    // Update feedback message
    if (wordFound) {
        feedbackMessage = "Word Found!";
    } else {
        feedbackMessage = "Invalid Word!";
    }

    // Set feedback timer
    feedbackTimer = 2 * FPS;
}

void MouseClicked(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        // Calculate the target cell based on mouse click coordinates
        int target_col = x / (2 * bradius);
        int target_row = (height - y) / (2 * bradius) - 1;

        // Ensure the target position is within bounds and shooter is not already moving
        if (target_row >= 0 && target_row < nycells && target_col >= 0 && target_col < nxcells && !shooterLetter.active) {
            // Check if the target cell is empty
            if (board[target_row][target_col] == -1) {
                // Set up the movement of the shooter letter towards the target
                shooterLetter.active = true;
                shooterLetter.alphabet = board[nycells][nxcells / 2]; // Shooter's alphabet

                // Shooter's starting position (bottom center)
                shooterLetter.x = (nxcells / 2) * (2 * bradius) - awidth / 2;
                shooterLetter.y = 0;

                // Compute direction vector towards target
                float target_x = target_col * (2 * bradius) - awidth / 2;
                float target_y = height - (target_row + 1) * (2 * bradius) - aheight / 2;

                float dir_x = target_x - shooterLetter.x;
                float dir_y = target_y - shooterLetter.y;

                // Normalize direction vector
                float magnitude = sqrt(dir_x * dir_x + dir_y * dir_y);
                shooterLetter.dx = (dir_x / magnitude) * 10; // Adjust speed as necessary
                shooterLetter.dy = (dir_y / magnitude) * 10;

                // Clear the shooter cell and prepare for new shooter
                board[nycells][nxcells / 2] = -1;
            }
        }
    }

    // Redraw the screen after mouse click
    glutPostRedisplay();
}



/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
    if (key == '1') {
        // Start the game
        cout << "Starting game..." << endl;
        glutDisplayFunc(DisplayFunction); // Switch to the main game
        glutPostRedisplay();
    } else if (key == '2') {
        // Show instructions
        cout << "Showing instructions..." << endl;
        glutDisplayFunc(InstructionsPage); // Switch to instructions
        glutPostRedisplay();
    } else if (key == '3') {
        // Exit the game
        cout << "Exiting game..." << endl;
        exit(0);
    }
}


/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) {
    static int time_left = 12000; // Game duration in seconds
    if (--time_left <= 0) {
        cout << "Game Over! Final Score: " << score << endl;
        exit(0); // End the game
    }

    // Update feedback timer
    if (feedbackTimer > 0) {
        feedbackTimer--;
    }

    // Update moving shooter letter
    if (shooterLetter.active) {
        shooterLetter.x += shooterLetter.dx;
        shooterLetter.y += shooterLetter.dy;

        // Bounce off left and right walls
        if (shooterLetter.x <= 0) {
            shooterLetter.x = 0;
            shooterLetter.dx = -shooterLetter.dx;
        }
        if (shooterLetter.x >= width - awidth) {
            shooterLetter.x = width - awidth;
            shooterLetter.dx = -shooterLetter.dx;
        }

        // Check for collision with cells
        int cell_col = (int)(shooterLetter.x / (2 * bradius));
        int cell_row = (int)((height - shooterLetter.y) / (2 * bradius)) - 1;

        // Ensure within bounds
        if (cell_row >= 0 && cell_row < nycells && cell_col >= 0 && cell_col < nxcells) {
    		// If collision with non-empty cell
    		if (board[cell_row][cell_col] != -1) {
        	// Place the letter in the cell below
        	if (cell_row + 1 < nycells) {
           		board[cell_row + 1][cell_col] = shooterLetter.alphabet;
            	HandleWordDetection(cell_row + 1, cell_col);
        	}
        shooterLetter.active = false; // Deactivate the shooter
    }
}

        // If reached top
        if (shooterLetter.y >= height - aheight) {
            int cell_row = 0;
            int cell_col = (int)(shooterLetter.x / (2 * bradius));
            if (cell_col >= 0 && cell_col < nxcells) {
                board[cell_row][cell_col] = shooterLetter.alphabet;
                HandleWordDetection(cell_row, cell_col);
            }
            shooterLetter.active = false;
        }
    }

    // Display remaining time on the screen (optional: can be handled in DisplayFunction)
    // DrawString(width - 200, height - 20, width, height, "Time: " + to_string(time_left) + "s", colors[RED]);

    // Redraw
    glutPostRedisplay();
    glutTimerFunc(500 / FPS, Timer, 0); // Set next timer
}

/*
* our gateway main function
* */
int main(int argc, char* argv[]) {
    InitRandomizer(); // Seed the random number generator

    // Dictionary for matching the words. It contains the 370099 words.
    dictionary = new string[dictionarysize];
    ReadWords("words_alpha.txt", dictionary); // Load dictionary from file

    // Print first 5 words from the dictionary for debugging
    for (int i = 0; i < 5; ++i) {
        cout << "Word " << i << " = " << dictionary[i] << endl;
    }

    // Initialize shooterLetter
    shooterLetter.active = false;

    // Initialize GLUT
    glutInit(&argc, argv);                         // Initialize the graphics library
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  // Using double buffering and RGBA mode
    glutInitWindowPosition(50, 50);                // Set the initial position of the window
    glutInitWindowSize(width, height);             // Set the size of the window
    glutCreateWindow("Word Shooter Game");         // Set the title of the game window

    // Register textures
    RegisterTextures();

    // Register callback functions
    glutDisplayFunc(HomePage);          // Set the home page as the initial display function
    glutSpecialFunc(NonPrintableKeys);  // Function to call for non-printable keys
    glutKeyboardFunc(PrintableKeys);    // Function to call for printable keys
    glutMouseFunc(MouseClicked);        // Function to call for mouse clicks
    glutPassiveMotionFunc(MouseMoved);  // Function to call when mouse moves without clicking

    // Set the timer callback
    glutTimerFunc(1000 / FPS, Timer, 0);

    // Start the main loop
    glutMainLoop();

    // Cleanup (unreachable in current setup but good practice)
    for (int i = 0; i <= nycells; ++i) {
        delete[] board[i];
    }
    delete[] board;
    delete[] dictionary;

    return 0;
}


#endif /* WORD_SHOOTER_CPP */
