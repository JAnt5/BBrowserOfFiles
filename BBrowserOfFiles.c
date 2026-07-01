#include <curses.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <term.h>
#include <unistd.h>

int SCREEN_WIDTH = 100;
const int MENU_HEIGHT = 6;
int folder_error = 0;


WINDOW *create_newwin(int height, int width, int starty, int startx) {
  WINDOW *local_win;

  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);
  wrefresh(local_win);

  return local_win;
}
void destroy_win(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);
}
WINDOW *displayTopWindow(int row, int col) {
  int height = row - MENU_HEIGHT;
  int width = col - 1;
  int starty = row - height - MENU_HEIGHT;
  int startx = col - width;
  refresh();
  WINDOW *window;
  window = create_newwin(height, width, starty, startx);

  wrefresh(window);
  return window;
}


WINDOW *displayTerminalMenu(int row, int col, char *input) {
  int height = MENU_HEIGHT;
  int width = col - 1;
  int starty = (row - height);
  int startx = (col - width);
  refresh();
  WINDOW *my_win;
  my_win = create_newwin(height, width, starty, startx);
  wborder(my_win, '|', '|', '-', '-', '*', '*', '*', '*');
  mvwprintw(my_win, 1, 2, "Type 'help' to view controls");
  mvwprintw(my_win, 3, 2, "Command:");
  mvwchgat(my_win, 3, 2, 8, A_BLINK, 1,
           NULL); // Command: v tej funkciji rabis napisat koliko characterjev
                  // od koordinate bo blinkal zato je 8

  attron(A_BOLD);
  mvwprintw(my_win, 3, 12, "%s", input);
  attroff(A_BOLD);

  wrefresh(my_win);

  // destroy_win(my_win);
  return my_win;
}
WINDOW *displayHelpWindow(int row, int col, WINDOW *window) {

  int x = 2, y = 2;
  mvwprintw(window, x, y, "> help -> Brings up this page");
  mvwprintw(window, x += 2, y, "> exit -> Quits the program");
  mvwprintw(window, x += 2, y, "> cd 'path' -> Move to that specific path");
  mvwprintw(window, x += 2, y, ">'arrow up' -> Move the cursor up");
  mvwprintw(window, x += 2, y, ">'arrow down' -> Move the cursor down");

  wrefresh(window);
  return window;
}

WINDOW *displayWelcomeScreen(int row, int col, WINDOW *window) {
  int x = 2, y = 2;
  mvwprintw(window, x, y, "> WELCOME TO BBROWSER OF FILES");
  mvwprintw(window, x += 2, y, "> type 'help' to view commands");
  mvwprintw(window, x += 2, y,
            "> type cd 'path' -> To begin and view a specific folder");

  wrefresh(window);
  return window;
}
WINDOW *FileBrowser(int row, int col, char *path, WINDOW *window) {
    char dir[100];
    char *files[256] = {};
    struct dirent *de;
    path += 3; // odreže vn cd  pa presledek

    if (chdir(path) != 0){
      //mvwprintw(window,2,2,"Chdir failed" );
      //wrefresh(window);

      //return window;
    }  

    mvwprintw(window, 2, 2, "Current working directory: %s",getcwd(dir,100));

    DIR *dr = opendir(dir);
    int x = 2;
    int y = 2;
    if (dr == NULL)
    {
        mvwprintw(window,2,2,"Could not open current directory" );
        wrefresh(window);
        return 0;
    }

    while((de = readdir(dr)) != NULL){
          mvwprintw(window,y+=2, x, "%s", de ->d_name);
    }
    
    wrefresh(window);

    return window;

}

int main() {
  int run = 1;
  char ch[100];
  int row, col;
  WINDOW *menu, *help_menu, *helper_window, *welcome_screen, *file_browser;
  initscr();
  // raw();
  cbreak();
  keypad(stdscr, TRUE);
  echo();

  // Setup for the program
  getmaxyx(stdscr, row, col); /* get the number of rows and columns */
  menu = displayTerminalMenu(row, col, " ");
  welcome_screen = displayWelcomeScreen(row, col, displayTopWindow(row, col));
  while (run) {

    getmaxyx(stdscr, row, col); /* get the number of rows and columns */
    wmove(menu,3,12);
    wclrtoeol(menu);
    wrefresh(menu);
    wgetstr(menu, ch);
    if (strcmp(ch, "exit") == 0) {
      run = 0;
    } else if (strcmp(ch, "help") == 0) {
      help_menu = displayHelpWindow(row, col, displayTopWindow(row, col));
    } else{
        
        file_browser =FileBrowser(row, col,ch, displayTopWindow(row, col));
    }
    menu = displayTerminalMenu(row, col, ch);

    refresh(); 
    
  }

  endwin(); 
  return 0;
}